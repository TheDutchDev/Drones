#ifndef IUINT8PROPERTY_H
#define IUINT8PROPERTY_H

#include "IProperty.h"

class IUint8Property : public IProperty<uint8_t> {
public:
    IUint8Property(uint8_t value, const uint8_t &minValue, const uint8_t &maxValue, EPropertyType propertyType)
            : IProperty<uint8_t>(value, minValue, maxValue, propertyType) {
        DataType = EPropertyDataType::Uint8;
    }
};

#endif //IUINT8PROPERTY_H
