#ifndef IPROPERTY_H
#define IPROPERTY_H

#include "Event.h"

class IProperty {
public:
    virtual ~IProperty() = default;
    virtual Event<IProperty *> &OnModified() = 0;
};

#endif // IPROPERTY_H
