#include "KFNpcGroupConfig.hpp"

namespace KFrame
{
    void KFNpcGroupConfig::ReadSetting( KFNode& xmlnode, KFNpcGroupSetting* kfsetting )
    {
        // 生成
        auto strgenerate = xmlnode.GetString( "Npc" );
        KFUtility::SplitList( kfsetting->_npc_generate_list, strgenerate, __SPLIT_STRING__ );
    }
    //////////////////////////////////////////////////////////////////////////////////////////////////


}