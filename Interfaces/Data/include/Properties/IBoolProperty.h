#ifndef IBOOLPROPERTY_H
#define IBOOLPROPERTY_H

#include "IProperty.h"

class IBoolProperty : public IProperty<bool> {
public:
    IBoolProperty(bool value, EPropertyType propertyType) : IProperty<bool>(value, false, true, propertyType) {
        DataType = EPropertyDataType::Bool;
    }
};

#endif //IBOOLPROPERTY_H
