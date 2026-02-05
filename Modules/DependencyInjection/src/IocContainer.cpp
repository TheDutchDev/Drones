#include "IocContainer.h"

static IocContainer containerInstance{};
IocContainer *container = &containerInstance;
