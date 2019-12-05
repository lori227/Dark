#include "KFTechnologyConfig.hpp"
#include "KFZConfig/KFReadSetting.h"

namespace KFrame
{
    void KFTechnologyConfig::ReadSetting( KFNode& xmlnode, KFTechnologySetting* kfsetting )
    {
        kfsetting->_status = xmlnode.GetUInt32( "Status" );
        kfsetting->_build_id = xmlnode.GetUInt32( "BuildingId" );

        auto conditionstr = xmlnode.GetString( "Condition" );
        KFReadSetting::ParseConditionList( conditionstr, kfsetting->_pre_technology );

        auto strunlocktechnology = xmlnode.GetString( "UnlockId" );
        KFUtility::SplitList( kfsetting->_unlock_technology, strunlocktechnology, __SPLIT_STRING__ );

        kfsetting->_str_consume = xmlnode.GetString( "Consume" );
        KFReadSetting::ReadExecuteData( xmlnode, &kfsetting->_execute_data );
    }

    void KFTechnologyConfig::LoadAllComplete()
    {
        for ( auto& iter : _settings._objects )
        {
            auto kfsetting = iter.second;
            if ( !kfsetting->_str_consume.empty() )
            {
                if ( !KFRewardConfig::Instance()->ParseRewards( kfsetting->_str_consume, kfsetting->_consume ) )
                {
                    __LOG_ERROR__( "id=[{}] technology config consume=[{}] is error", kfsetting->_id, kfsetting->_str_consume );
                }
            }
        }
    }
}