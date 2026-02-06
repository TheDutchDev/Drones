#ifndef IUINT32PROPERTY_H
#define IUINT32PROPERTY_H

#include "IProperty.h"

class IUint32Property : public IProperty<uint32_t> {
public:
    IUint32Property(uint32_t value, const uint32_t &minValue, const uint32_t &maxValue, EPropertyType propertyType)
            : IProperty<uint32_t>(value, minValue, maxValue, propertyType) {
        DataType = EPropertyDataType::Uint32;
    }
};

#endif //IUINT32PROPERTY_H
