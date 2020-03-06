#include "KFNpcRandConfig.hpp"

namespace KFrame
{
    void KFNpcRandConfig::ReadSetting( KFNode& xmlnode, KFNpcRandSetting* kfsetting )
    {
        auto npcgroupid = xmlnode.GetUInt32( "NpcGroup" );
        auto weight = xmlnode.GetUInt32( "Weight" );

        auto kfweight = kfsetting->_npc_weight_list.Create( npcgroupid, weight );
        kfweight->_spawn_rule = xmlnode.GetUInt32( "SpawnRule" );
    }
    //////////////////////////////////////////////////////////////////////////////////////////////////


}