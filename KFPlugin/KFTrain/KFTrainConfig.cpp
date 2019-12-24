#include "KFTrainConfig.hpp"

namespace KFrame
{
    void KFTrainConfig::ReadSetting( KFNode& xmlnode, KFTrainSetting* kfsetting )
    {
        kfsetting->_level = xmlnode.GetUInt32( "Id", true );
        kfsetting->_count = xmlnode.GetUInt32( "Count", true );
        kfsetting->_total_time = xmlnode.GetUInt64( "TotalTime", true );
        kfsetting->_cd_time = xmlnode.GetUInt64( "CdTime", true );
        kfsetting->_add_exp = xmlnode.GetUInt32( "AddExp", true );
        kfsetting->_unit_time = xmlnode.GetUInt32( "UnitTime", true );

        kfsetting->_consume_str = xmlnode.GetString( "Consume", true );
        kfsetting->_onekey_consume_str = xmlnode.GetString( "OnekeyConsume", true );
    }

    void KFTrainConfig::LoadAllComplete()
    {
        for ( auto& iter : _settings._objects )
        {
            auto kfsetting = iter.second;
            if ( !kfsetting->_consume_str.empty() )
            {
                if ( !KFRewardConfig::Instance()->ParseRewards( kfsetting->_consume_str, kfsetting->_consume ) )
                {
                    __LOG_ERROR__( "id=[{}] train config consume is error", kfsetting->_id );
                }
            }

            if ( !kfsetting->_onekey_consume_str.empty() )
            {
                if ( !KFRewardConfig::Instance()->ParseRewards( kfsetting->_onekey_consume_str, kfsetting->_onekey_consume ) )
                {
                    __LOG_ERROR__( "id=[{}] train config onekey consume is error", kfsetting->_id );
                }
            }
        }
    }
}