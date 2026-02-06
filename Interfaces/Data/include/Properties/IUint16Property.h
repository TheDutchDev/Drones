#ifndef IUINT16PROPERTY_H
#define IUINT16PROPERTY_H

#include "IProperty.h"

class IUint16Property : public IProperty<uint16_t> {
public:
    IUint16Property(uint16_t value, const uint16_t &minValue, const uint16_t &maxValue, EPropertyType propertyType)
            : IProperty<uint16_t>(value, minValue, maxValue, propertyType) {
        DataType = EPropertyDataType::Uint16;
    }
};

#endif //IUINT16PROPERTY_H
