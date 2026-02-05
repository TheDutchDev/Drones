#ifndef IOCCONTAINER_H
#define IOCCONTAINER_H

#include <memory>
#include <unordered_map>
#include <vector>
#include <stdexcept>
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
            throw std::runtime_error("IocContainer: Resolving a new instance of singleton type: " + std::string(typeid(Interface).name()));
        }

        std::vector<std::pair<void*, std::shared_ptr<void>>> overrides = {
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
            throw std::runtime_error("IocContainer: Resolving a new instance of singleton type: " + std::string(typeid(Interface).name()));
        }

        std::vector<std::pair<void*, std::shared_ptr<void>>> overrides = {
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
        std::shared_ptr<void> (*CreatorWithOverrides)(IocContainer*, const std::vector<std::pair<void*, std::shared_ptr<void>>>& overrides);
        std::vector<void*> DependencyTypes;
    };

    std::unordered_map<void*, Service> _serviceRegistry;
    std::unordered_map<void*, std::unordered_map<void*, Service>> _keyedServiceRegistry;

    template<typename Interface>
    Service& GetService( ) {
        auto it = _serviceRegistry.find(TypeId<Interface>());
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
        const std::vector<std::pair<void*, std::shared_ptr<void>>>& overrides
    ) {
        return std::make_shared<Implementation>(
            ResolveOrOverride<Dependencies>(container, overrides)...);
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
        const std::vector<std::pair<void*, std::shared_ptr<void>>>& overrides
    ) {
        if constexpr (IsKeyed<T>::value) {
            using Interface = typename T::InterfaceType;
            using Implementation = typename T::ImplementationType;
            void* key = TypeId<Interface>();
            for (const auto& [type, instance] : overrides) {
                if (type == key) {
                    return std::static_pointer_cast<Interface>(instance);
                }
            }
            return container->ResolveKeyed<Interface, Implementation>();
        }
        void* key = TypeId<T>();
        for (const auto& [type, instance] : overrides) {
            if (type == key) {
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
    static void* TypeId() {
        static char id;
        return &id;
    }
};

extern IocContainer* container;

#endif // IOCCONTAINER_H
