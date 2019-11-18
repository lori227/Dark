#include "KFPVEConfig.hpp"
#include "KFZConfig/KFReadSetting.h"

namespace KFrame
{
    void KFPVEConfig::ReadSetting( KFNode& xmlnode, KFPVESetting* kfsetting )
    {
        // 生成里诶表
        auto strgenerate = xmlnode.GetString( "Npc" );
        kfsetting->_npc_generate_list = KFUtility::SplitList< VectorUInt32 >( strgenerate, __SPLIT_STRING__ );

        auto strdropvictory = xmlnode.GetString( "DropVictory", true );
        KFReadSetting::ParseConditionList( strdropvictory, kfsetting->_victory_drop_list );

        auto strdropfail = xmlnode.GetString( "DropFail", true );
        KFReadSetting::ParseConditionList( strdropfail, kfsetting->_fail_drop_list );
    }
    //////////////////////////////////////////////////////////////////////////////////////////////////


}