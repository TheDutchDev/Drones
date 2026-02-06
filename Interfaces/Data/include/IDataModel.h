#ifndef IDATAMODEL_H
#define IDATAMODEL_H

#include <algorithm>
#include <IPropertyBase.h>
#include <EEventOwner.h>
#include <functional>

#include "ILogger.h"

class IDataModel {
    public:
        IDataModel() {
            OnPropertyModified = PropertyModifiedDelegate::Make();
        }

        virtual ~IDataModel() = default;

        void RegisterProperty(IPropertyBase& property) {
            property.OnModified->Subscribe(EEventOwner::DataModule, this, &IDataModel::PropertyModifiedCallback);
            Properties.push_back(&property);
        }

        void UpdateProperty(const EPropertyType propertyType, const uint8_t* value, const size_t size) {
            const auto property = GetPropertyOfType(propertyType);

            if(property == nullptr)
                return;

            property->SetValueFromByteArray(value, size);
        }

        IPropertyBase* GetPropertyOfType(const EPropertyType propertyType) {
            for(const auto &model: _dataModels) {
                if(const auto property = model->GetPropertyOfType(propertyType)) {
                    return property;
                }
            }

            return GetLocalPropertyOfType(propertyType);
        }

        virtual IPropertyBase* GetLocalPropertyOfType(const EPropertyType propertyType) {
            const auto iterator = std::ranges::find_if(Properties,
                                                       [&](const IPropertyBase* propertyBase) {
                                                           return propertyBase->PropertyType == propertyType;
                                                       });

            if(iterator != Properties.end()) {
                return *iterator;
            }

            return nullptr;
        }

        std::shared_ptr<PropertyModifiedDelegate> OnPropertyModified;
        std::vector<IPropertyBase*> Properties;
    protected:
        virtual void PropertyModifiedCallback(IPropertyBase* propertyBase) {
            OnPropertyModified->Emit(propertyBase);
        }


        virtual void HookDataModels() {
            for(const auto &model: _dataModels) {
                model->OnPropertyModified->Subscribe(EEventOwner::DataModule, this, &IDataModel::PropertyModifiedCallback);
            }
        }

        std::vector<std::shared_ptr<IDataModel>> _dataModels;
};

#endif //IDATAMODEL_H
