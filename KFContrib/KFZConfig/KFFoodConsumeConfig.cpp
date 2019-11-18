#include "KFFoodConsumeConfig.hpp"

namespace KFrame
{
    void KFFoodConsumeConfig::ReadSetting( KFNode& xmlnode, KFFoodConsumeSetting* kfsetting )
    {
        kfsetting->_step_count = xmlnode.GetUInt32( "StepCount", true );
        kfsetting->_food_num = xmlnode.GetUInt32( "FoodNum", true );
        kfsetting->_hp_num = xmlnode.GetUInt32( "HpNum", true );
    }
}