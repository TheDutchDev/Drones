#ifndef IINT16PROPERTY_H
#define IINT16PROPERTY_H

#include "IProperty.h"

class IInt16Property : public IProperty<int16_t> {
public:
    IInt16Property(int16_t value, const int16_t &minValue, const int16_t &maxValue, EPropertyType propertyType)
            : IProperty<int16_t>(value, minValue, maxValue, propertyType) {
        DataType = EPropertyDataType::Int16;
    }
};

#endif //IINT16PROPERTY_H
