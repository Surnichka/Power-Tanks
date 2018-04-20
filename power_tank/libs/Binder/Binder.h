#include "GenericFunctionBinder.h"
#include "../UniStream/UniSerializer.h"
#include "../UniStream/UniDeserializer.h"

inline GenericFunctionBinder<oUniStream, iUniStream>& GetBinder()
{
    static GenericFunctionBinder<oUniStream, iUniStream> binder;
    return binder;
}
