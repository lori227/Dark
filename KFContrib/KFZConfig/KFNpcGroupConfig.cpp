#include "KFNpcGroupConfig.hpp"

namespace KFrame
{
    void KFNpcGroupConfig::ReadSetting( KFNode& xmlnode, KFNpcGroupSetting* kfsetting )
    {
        // 生成
        kfsetting->_npc_generate_list = xmlnode.GetUInt32Vector( "Npc" );
        kfsetting->_show_model = xmlnode.GetString( "SpawnModel" );
    }
    //////////////////////////////////////////////////////////////////////////////////////////////////


}