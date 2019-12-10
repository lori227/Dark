#include "KFPVEConfig.hpp"
#include "KFZConfig/KFReadSetting.h"

namespace KFrame
{
    void KFPVEConfig::ReadSetting( KFNode& xmlnode, KFPVESetting* kfsetting )
    {
        // 生成里诶表
        auto strgenerate = xmlnode.GetString( "Npc" );
        KFUtility::SplitList( kfsetting->_npc_generate_list, strgenerate, __SPLIT_STRING__ );

        auto strdropvictory = xmlnode.GetString( "DropVictory", true );
        KFReadSetting::ParseConditionList( strdropvictory, kfsetting->_victory_drop_list );

        auto strdropfail = xmlnode.GetString( "DropFail", true );
        KFReadSetting::ParseConditionList( strdropfail, kfsetting->_fail_drop_list );

        kfsetting->_consume_str = xmlnode.GetString( "Consume", true );
    }

    void KFPVEConfig::LoadAllComplete()
    {
        for ( auto& iter : _settings._objects )
        {
            auto kfsetting = iter.second;

            if ( !kfsetting->_consume_str.empty() )
            {
                if ( !KFRewardConfig::Instance()->ParseRewards( kfsetting->_consume_str, kfsetting->_consume ) )
                {
                    __LOG_ERROR__( "id=[{}] pve config consume is error", kfsetting->_id );
                }
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////////////////////////////


}