#ifndef ITYPEDPROPERTY_H
#define ITYPEDPROPERTY_H

#include "IProperty.h"

template<typename T>
class ITypedProperty : public IProperty {
public:
    virtual T Get() const = 0;
    virtual void Modify(const T &value) = 0;
};

#endif // ITYPEDPROPERTY_H
