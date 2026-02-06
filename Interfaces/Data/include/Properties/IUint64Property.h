#ifndef IUINT64PROPERTY_H
#define IUINT64PROPERTY_H

#include "IProperty.h"

class IUint64Property : public IProperty<uint64_t> {
public:
    IUint64Property(uint64_t value, const uint64_t &minValue, const uint64_t &maxValue, EPropertyType propertyType)
            : IProperty<uint64_t>(value, minValue, maxValue, propertyType) {
        DataType = EPropertyDataType::Uint64;
    }
};

#endif //IUINT64PROPERTY_H
