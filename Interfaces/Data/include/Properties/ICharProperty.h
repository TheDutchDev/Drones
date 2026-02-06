#ifndef ICHARPROPERTY_H
#define ICHARPROPERTY_H

#include "IProperty.h"

class ICharProperty : public IProperty<const char *> {
public:
    ICharProperty(const char *value, EPropertyType propertyType)
            : IProperty<const char *>(value, "", "", propertyType) {
        DataType = EPropertyDataType::Char;
    }
};

#endif //ICHARPROPERTY_H
