#ifndef IINT8PROPERTY_H
#define IINT8PROPERTY_H

#include "IProperty.h"

class IInt8Property : public IProperty<int8_t> {
public:
    IInt8Property(int8_t value, const int8_t &minValue, const int8_t &maxValue, EPropertyType propertyType)
            : IProperty<int8_t>(value, minValue, maxValue, propertyType) {
        DataType = EPropertyDataType::Int8;
    }
};

#endif //IINT8PROPERTY_H
