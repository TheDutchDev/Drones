#ifndef IFLOATPROPERTY_H
#define IFLOATPROPERTY_H

#include "IProperty.h"

class IFloatProperty : public IProperty<float> {
public:
    IFloatProperty(float value, const float &minValue, const float &maxValue, EPropertyType propertyType)
            : IProperty<float>(value, minValue, maxValue, propertyType) {
        DataType = EPropertyDataType::Float;
    }
};

#endif //IFLOATPROPERTY_H
