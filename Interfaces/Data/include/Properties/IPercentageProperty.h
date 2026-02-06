#ifndef IPERCENTAGEPROPERTY_H
#define IPERCENTAGEPROPERTY_H

#include "IFloatProperty.h"

class IPercentageProperty : public IFloatProperty {
public:
    IPercentageProperty(float value, const float &minValue, const float &maxValue, EPropertyType propertyType)
            : IFloatProperty(value, minValue, maxValue, propertyType) {
    }

    float ToFactor() {
        return Value() / 100.0f;
    }
};

#endif //IPERCENTAGEPROPERTY_H
