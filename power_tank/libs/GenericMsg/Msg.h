#pragma once
#include "GenericMsg.h"
#include "../UniStream/UniSerializer.h"
#include "../UniStream/UniDeserializer.h"

namespace msg
{
    using UniMsg = GenericMsg<oUniStream, iUniStream>;
}
