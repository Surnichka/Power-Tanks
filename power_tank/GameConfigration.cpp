#include "GameConfigration.h"

GameConfiguration &GetCfg()
{
    static GameConfiguration gameCfg;
    return gameCfg;
}
