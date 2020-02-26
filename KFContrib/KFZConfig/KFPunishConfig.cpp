#include "KFPunishConfig.hpp"

namespace KFrame
{
    void KFPunishConfig::ReadSetting( KFNode& xmlnode, KFPunishSetting* kfsetting )
    {
        if ( kfsetting->_min_count == 0u )
        {
            kfsetting->_min_count = xmlnode.GetUInt32( "MinCount" );
        }

        if ( kfsetting->_max_count == 0u )
        {
            kfsetting->_max_count = xmlnode.GetUInt32( "MaxCount" );
        }

        KFPunishData data;
        data._min_weight = xmlnode.GetUInt32( "MinWeight" );
        data._max_weight = xmlnode.GetUInt32( "MaxWeight" );
        data._name = xmlnode.GetString( "Name" );
        data._key = xmlnode.GetUInt32( "Key" );
        data._min_value = xmlnode.GetUInt32( "MinValue" );
        data._max_value = xmlnode.GetUInt32( "MaxValue" );
        kfsetting->_punish_list.emplace_back( data );
    }

    //////////////////////////////////////////////////////////////////////////////////////////////////


}