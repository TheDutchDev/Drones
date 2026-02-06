#ifndef IINT32PROPERTY_H
#define IINT32PROPERTY_H

#include "IProperty.h"

class IInt32Property : public IProperty<int32_t> {
public:
    IInt32Property(int32_t value, const int32_t &minValue, const int32_t &maxValue, EPropertyType propertyType)
            : IProperty<int32_t>(value, minValue, maxValue, propertyType) {
        DataType = EPropertyDataType::Int32;
    }
};

#endif //IINT32PROPERTY_H
