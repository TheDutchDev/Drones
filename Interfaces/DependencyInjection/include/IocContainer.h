#ifndef IOCCONTAINER_H
#define IOCCONTAINER_H

#include <cstdint>
#include <memory>
#include <stdexcept>
#include <type_traits>
#include <typeinfo>
#include <unordered_map>
#include <vector>
#include "EServiceLifetime.h"

class IocContainer {
public:
    /**
     * @brief Tag type for keyed dependency injection
     * @tparam Interface The interface to resolve
     * @tparam Implementation The implementation type used as key
     */
    template<typename Interface, typename Implementation>
    struct Keyed {
        using InterfaceType = Interface;
        using ImplementationType = Implementation;
    };

    /**
     * @brief Wrapper type for keyed overrides
     * @tparam Interface The interface to override
     * @tparam Implementation The implementation type used as key
     */
    template<typename Interface, typename Implementation>
    struct KeyedOverride {
        std::shared_ptr<Interface> Value;
    };

    /**
     * @brief Helper to build a keyed override instance
     * @tparam Interface The interface to override
     * @tparam Implementation The implementation type used as key
     * @param instance The instance to provide as override
     */
    template<typename Interface, typename Implementation>
    static std::shared_ptr<KeyedOverride<Interface, Implementation>> OverrideKeyed(
        std::shared_ptr<Interface> instance
    ) {
        auto wrapper = std::make_shared<KeyedOverride<Interface, Implementation>>();
        wrapper->Value = std::move(instance);
        return wrapper;
    }

    /**
     * @brief Register a singleton service by interface
     * @tparam Interface The interface that this singleton implements from
     * @tparam Implementation The implementation (class) of this singleton
     * @tparam Dependencies Any dependencies that the implementation requires
     */
    template<typename Interface, typename Implementation, typename... Dependencies>
    void RegisterSingletonWithInterface() {
        RegisterWithInterface<Interface, Implementation, Dependencies...>(EServiceLifetime::Singleton);
    }

    /**
     * @brief Register a transient service by interface
     * @tparam Interface The interface that this transient implements from
     * @tparam Implementation The implementation (class) of this transient
     * @tparam Dependencies Any dependencies that the implementation requires
     */
    template<typename Interface, typename Implementation, typename... Dependencies>
    void RegisterTransientWithInterface() {
        RegisterWithInterface<Interface, Implementation, Dependencies...>(EServiceLifetime::Transient);
    }

    /**
     * @brief Register a singleton service by interface with an implementation key
     * @tparam Interface The interface that this singleton implements from
     * @tparam Implementation The implementation (class) of this singleton
     * @tparam Dependencies Any dependencies that the implementation requires
     */
    template<typename Interface, typename Implementation, typename... Dependencies>
    void RegisterSingletonWithInterfaceKeyed() {
        RegisterWithInterfaceKeyed<Interface, Implementation, Dependencies...>(EServiceLifetime::Singleton);
    }

    /**
     * @brief Register a transient service by interface with an implementation key
     * @tparam Interface The interface that this transient implements from
     * @tparam Implementation The implementation (class) of this transient
     * @tparam Dependencies Any dependencies that the implementation requires
     */
    template<typename Interface, typename Implementation, typename... Dependencies>
    void RegisterTransientWithInterfaceKeyed() {
        RegisterWithInterfaceKeyed<Interface, Implementation, Dependencies...>(EServiceLifetime::Transient);
    }

    /**
     * @brief Register a singleton service by implementation
     * @tparam Implementation The implementation (class) of this singleton
     * @tparam Dependencies Any dependencies that the implementation requires
     */
    template<typename Implementation, typename... Dependencies>
    void RegisterSingletonClass() {
        RegisterWithInterface<Implementation, Implementation, Dependencies...>(EServiceLifetime::Singleton);
    }

    /**
    * @brief Register a transient service by implementation
    * @tparam Implementation The implementation (class) of this transient
    * @tparam Dependencies Any dependencies that the implementation requires
    */
    template<typename Implementation, typename... Dependencies>
    void RegisterTransientClass() {
        RegisterWithInterface<Implementation, Implementation, Dependencies...>(EServiceLifetime::Transient);
    }

    /**
     * @brief Register a base interface that resolves to an existing derived service
     * @tparam PrimaryInterface The derived interface already registered in the container
     * @tparam AdditionalInterface A base interface that should resolve to the same instance
     */
    template<typename PrimaryInterface, typename AdditionalInterface>
    void RegisterAdditional() {
        static_assert(
            std::is_base_of<AdditionalInterface, PrimaryInterface>::value,
            "RegisterAdditional requires AdditionalInterface to be a base of PrimaryInterface"
        );

        auto primaryType = ResolveAliasType(TypeId<PrimaryInterface>());
        auto primaryIt = _serviceRegistry.find(primaryType);
        if (primaryIt == _serviceRegistry.end()) {
            throw std::runtime_error("Type " + std::string(typeid(PrimaryInterface).name()) + " not registered in IoC container");
        }

        auto additionalType = TypeId<AdditionalInterface>();
        if (_serviceRegistry.find(additionalType) != _serviceRegistry.end()) {
            throw std::runtime_error("Type " + std::string(typeid(AdditionalInterface).name()) + " already registered in IoC container");
        }

        if (ResolveAliasType(primaryType) == additionalType) {
            throw std::runtime_error("IocContainer: alias loop detected for type: " + std::string(typeid(AdditionalInterface).name()));
        }

        _serviceAliases[additionalType] = primaryType;

        Service service;
        service.Lifetime = primaryIt->second.Lifetime;
        service.Creator = &CreateAlias<PrimaryInterface>;
        service.CreatorWithOverrides = &CreateAliasWithOverrides<PrimaryInterface>;
        service.DependencyTypes = {};
        _serviceRegistry[additionalType] = service;
    }

    /**
     * @brief Resolve a service by interface or implementation type
     * @tparam Interface The interface or implementation that it was registered with
     * @return When singleton: always the same instance of the given type. When transient: always a new instance of the given type
     */
    template<typename Interface>
    std::shared_ptr<Interface> Resolve() {
        auto& service = GetService<Interface>();

        if (service.Lifetime == EServiceLifetime::Singleton) {
            if (!service.Instance) {
                service.Instance = service.Creator(this);
            }
            return std::static_pointer_cast<Interface>(service.Instance);
        }

        return std::static_pointer_cast<Interface>(service.Creator(this));
    }

    /**
     * @brief Resolve a keyed service by interface and implementation type
     * @tparam Interface The interface the service was registered with
     * @tparam Implementation The implementation type used as key
     * @return When singleton: always the same instance of the given type. When transient: always a new instance of the given type
     */
    template<typename Interface, typename Implementation>
    std::shared_ptr<Interface> ResolveKeyed() {
        auto& service = GetKeyedService<Interface, Implementation>();

        if (service.Lifetime == EServiceLifetime::Singleton) {
            if (!service.Instance) {
                service.Instance = service.Creator(this);
            }
            return std::static_pointer_cast<Interface>(service.Instance);
        }

        return std::static_pointer_cast<Interface>(service.Creator(this));
    }

    /**
     * @brief Resolve a service by interface or implementation type omitting specified overrides
     * @tparam Interface The interface or implementation that it was registered with
     * @tparam Overrides The overrides the container should NOT resolve
     * @return When singleton: throws error since singletons can only have one instance. When transient: always a new instance of the given type
     */
    template<typename Interface, typename... Overrides>
    std::shared_ptr<Interface> Resolve(std::shared_ptr<Overrides>... provided) {
        auto& service = GetService<Interface>();

        if (service.Lifetime == EServiceLifetime::Singleton) {
            if (service.Instance)
                std::static_pointer_cast<Interface>(service.Instance);
        }

        std::vector<std::pair<const std::type_info*, std::shared_ptr<void>>> overrides = {
            { TypeId<Overrides>(), provided }...
        };

        return std::static_pointer_cast<Interface>(
            service.CreatorWithOverrides(this, overrides)
        );
    }

    /**
     * @brief Resolve a keyed service by interface and implementation type omitting specified overrides
     * @tparam Interface The interface the service was registered with
     * @tparam Implementation The implementation type used as key
     * @tparam Overrides The overrides the container should NOT resolve
     * @return When singleton: throws error since singletons can only have one instance. When transient: always a new instance of the given type
     */
    template<typename Interface, typename Implementation, typename... Overrides>
    std::shared_ptr<Interface> ResolveKeyed(std::shared_ptr<Overrides>... provided) {
        auto& service = GetKeyedService<Interface, Implementation>();

        if (service.Lifetime == EServiceLifetime::Singleton) {
            if (service.Instance)
                std::static_pointer_cast<Interface>(service.Instance);
        }

        std::vector<std::pair<const std::type_info*, std::shared_ptr<void>>> overrides = {
            { TypeId<Overrides>(), provided }...
        };

        return std::static_pointer_cast<Interface>(
            service.CreatorWithOverrides(this, overrides)
        );
    }

private:
    struct Service {
        EServiceLifetime Lifetime;
        std::shared_ptr<void> Instance;
        std::shared_ptr<void> (*Creator)(IocContainer*);
        std::shared_ptr<void> (*CreatorWithOverrides)(IocContainer*, const std::vector<std::pair<const std::type_info*, std::shared_ptr<void>>>& overrides);
        std::vector<const std::type_info*> DependencyTypes;
    };

    struct TypeInfoHash {
        size_t operator()(const std::type_info* type) const noexcept {
            return std::hash<std::uintptr_t>()(reinterpret_cast<std::uintptr_t>(type));
        }
    };

    struct TypeInfoEqual {
        bool operator()(const std::type_info* lhs, const std::type_info* rhs) const noexcept {
            return lhs == rhs;
        }
    };

    std::unordered_map<const std::type_info*, Service, TypeInfoHash, TypeInfoEqual> _serviceRegistry;
    std::unordered_map<const std::type_info*, std::unordered_map<const std::type_info*, Service, TypeInfoHash, TypeInfoEqual>, TypeInfoHash, TypeInfoEqual> _keyedServiceRegistry;
    std::unordered_map<const std::type_info*, const std::type_info*, TypeInfoHash, TypeInfoEqual> _serviceAliases;

    template<typename Interface>
    Service& GetService( ) {
        auto requestedType = TypeId<Interface>();
        auto it = _serviceRegistry.find(requestedType);
        if (it != _serviceRegistry.end()) {
            return it->second;
        }

        auto resolvedType = ResolveAliasType(requestedType);
        it = _serviceRegistry.find(resolvedType);
        if (it == _serviceRegistry.end()) {
            throw std::runtime_error("Type " + std::string(typeid(Interface).name()) + " not registered in IoC container");
        }

        return it->second;
    }

    template<typename Interface, typename Implementation>
    Service& GetKeyedService() {
        auto it = _keyedServiceRegistry.find(TypeId<Interface>());
        if (it == _keyedServiceRegistry.end()) {
            throw std::runtime_error("Type " + std::string(typeid(Interface).name()) + " not registered in IoC container");
        }

        auto keyed = it->second.find(TypeId<Implementation>());
        if (keyed == it->second.end()) {
            throw std::runtime_error("Type " + std::string(typeid(Implementation).name()) + " not registered in IoC container");
        }

        return keyed->second;
    }

    template<typename Interface, typename Implementation, typename... Dependencies>
    void RegisterWithInterface(EServiceLifetime lifetime) {
        Service service;
        service.Lifetime = lifetime;
        service.Creator = &Create<Implementation, Dependencies...>;
        service.CreatorWithOverrides = &CreateWithOverrides<Implementation, Dependencies...>;
        service.DependencyTypes = { TypeId<Dependencies>()... };
        _serviceRegistry[TypeId<Interface>()] = service;
        _serviceAliases.erase(TypeId<Interface>());
    }

    template<typename Interface, typename Implementation, typename... Dependencies>
    void RegisterWithInterfaceKeyed(EServiceLifetime lifetime) {
        Service service;
        service.Lifetime = lifetime;
        service.Creator = &Create<Implementation, Dependencies...>;
        service.CreatorWithOverrides = &CreateWithOverrides<Implementation, Dependencies...>;
        service.DependencyTypes = { TypeId<Dependencies>()... };
        _keyedServiceRegistry[TypeId<Interface>()][TypeId<Implementation>()] = service;
    }

    template<typename Implementation, typename... Dependencies>
    static std::shared_ptr<void> Create(IocContainer* container) {
        return std::make_shared<Implementation>(ResolveDependency<Dependencies>(container)...);
    }

    template<typename Implementation, typename... Dependencies>
    static std::shared_ptr<void> CreateWithOverrides(
        IocContainer* container,
        const std::vector<std::pair<const std::type_info*, std::shared_ptr<void>>>& overrides
    ) {
        return std::make_shared<Implementation>(
            ResolveOrOverride<Dependencies>(container, overrides)...);
    }

    template<typename PrimaryInterface>
    static std::shared_ptr<void> CreateAlias(IocContainer* container) {
        auto instance = container->Resolve<PrimaryInterface>();
        return std::static_pointer_cast<void>(instance);
    }

    template<typename PrimaryInterface>
    static std::shared_ptr<void> CreateAliasWithOverrides(
        IocContainer* container,
        const std::vector<std::pair<const std::type_info*, std::shared_ptr<void>>>& overrides
    ) {
        (void)overrides;
        return CreateAlias<PrimaryInterface>(container);
    }

    template<typename T>
    static auto ResolveDependency(IocContainer* container) {
        if constexpr (IsKeyed<T>::value) {
            using Interface = typename T::InterfaceType;
            using Implementation = typename T::ImplementationType;
            return container->ResolveKeyed<Interface, Implementation>();
        } else {
            return container->Resolve<T>();
        }
    }

    template<typename T>
    static auto ResolveOrOverride(
        IocContainer* container,
        const std::vector<std::pair<const std::type_info*, std::shared_ptr<void>>>& overrides
    ) {
        if constexpr (IsKeyed<T>::value) {
            using Interface = typename T::InterfaceType;
            using Implementation = typename T::ImplementationType;
            auto key = TypeId<KeyedOverride<Interface, Implementation>>();
            for (const auto& [type, instance] : overrides) {
                if (type == key) {
                    auto wrapper = std::static_pointer_cast<KeyedOverride<Interface, Implementation>>(instance);
                    return wrapper ? wrapper->Value : std::shared_ptr<Interface>{};
                }
            }
            return container->ResolveKeyed<Interface, Implementation>();
        }
        auto key = TypeId<T>();
        for (const auto& [type, instance] : overrides) {
            if (type == key) {
                return std::static_pointer_cast<T>(instance);
            }
        }
        for (const auto& [type, instance] : overrides) {
            if (container->IsAdditionalAliasMatch(key, type)) {
                return std::static_pointer_cast<T>(instance);
            }
        }
        return container->Resolve<T>();
    }

    template<typename T>
    struct IsKeyed {
        static constexpr bool value = false;
    };

    template<typename Interface, typename Implementation>
    struct IsKeyed<Keyed<Interface, Implementation>> {
        static constexpr bool value = true;
    };

    template<typename T>
    static const std::type_info* TypeId() {
        return &typeid(T);
    }

    const std::type_info* ResolveAliasType(const std::type_info* type) const {
        auto current = type;
        size_t remaining = _serviceAliases.size() + 1;
        while (remaining-- > 0) {
            auto it = _serviceAliases.find(current);
            if (it == _serviceAliases.end()) {
                return current;
            }
            current = it->second;
        }
        throw std::runtime_error("IocContainer: alias resolution exceeded expected depth");
    }

    bool IsAdditionalAliasMatch(const std::type_info* dependencyType, const std::type_info* overrideType) const {
        if (overrideType == dependencyType) {
            return true;
        }
        return ResolveAliasType(dependencyType) == overrideType;
    }
};

extern IocContainer* container;

#endif // IOCCONTAINER_H
