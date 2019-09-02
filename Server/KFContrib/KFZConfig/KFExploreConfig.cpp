#include "KFExploreConfig.hpp"

namespace KFrame
{
    void KFExploreConfig::ReadSetting( KFNode& xmlnode, KFExploreSeting* kfsetting )
    {
        kfsetting->_victory_item_drop_id = xmlnode.GetUInt32( "ItemDropVictory", true );
        kfsetting->_fail_item_drop_id = xmlnode.GetUInt32( "ItemDropFail", true );
        kfsetting->_victory_exp_drop_id = xmlnode.GetUInt32( "HeroExpVictory", true );
        kfsetting->_fail_exp_drop_id = xmlnode.GetUInt32( "HeroExpFail", true );
    }
}