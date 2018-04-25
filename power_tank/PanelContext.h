#pragma once
#include "libs/GenericMsg/Msg.h"

inline msg::UniMsg& GetPanelContext()
{
    static msg::UniMsg panelContext;
    return panelContext;
};
