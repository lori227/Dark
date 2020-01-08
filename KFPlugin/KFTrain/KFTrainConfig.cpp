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

        kfsetting->_str_consume = xmlnode.GetString( "Consume", true );
        kfsetting->_str_onekey_consume = xmlnode.GetString( "OnekeyConsume", true );
    }

    void KFTrainConfig::LoadAllComplete()
    {
        for ( auto& iter : _settings._objects )
        {
            auto kfsetting = iter.second;
            KFElementConfig::Instance()->ParseElement( kfsetting->_consume, kfsetting->_str_consume, __FILE__, kfsetting->_id );
            KFElementConfig::Instance()->ParseElement( kfsetting->_onekey_consume, kfsetting->_str_onekey_consume, __FILE__, kfsetting->_id );
        }
    }
}