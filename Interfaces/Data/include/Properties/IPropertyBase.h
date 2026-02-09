#ifndef IPROPERTYBASE_H
#define IPROPERTYBASE_H

#include "EDataType.h"
#include "EPropertyType.h"
#include "EPropertyDataType.h"
#include "Event.h"

class IPropertyBase; // Forward declaration

#define MAX_PROPERTY_EVENT_SUBSCRIPTIONS 5
typedef Event<MAX_PROPERTY_EVENT_SUBSCRIPTIONS, IPropertyBase*> PropertyModifiedDelegate;

class IPropertyBase {
    public:
        virtual ~IPropertyBase() = default;
        virtual uint8_t* ValuePtr() = 0;
        virtual void SetValueFromByteArray(const uint8_t* data, size_t size) = 0;
        [[nodiscard]] virtual EDataType GetGenericDataType() const = 0;
        [[nodiscard]] virtual uint8_t GetDataSize() const = 0;
        EPropertyType PropertyType = EPropertyType::Unknown;
        EPropertyDataType DataType = EPropertyDataType::Unknown;
        std::shared_ptr<PropertyModifiedDelegate> OnModified;
};

#endif //IPROPERTYBASE_H
