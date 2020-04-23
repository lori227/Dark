#include "KFPunishConfig.hpp"

namespace KFrame
{
    void KFPunishConfig::ReadSetting( KFNode& xmlnode, KFPunishSetting* kfsetting )
    {
        auto mincount = xmlnode.GetUInt32( "MinCount" );
        auto maxcount = xmlnode.GetUInt32( "MaxCount" );
        kfsetting->_range_count.SetValue( mincount, maxcount );

        KFPunishData data;
        data._range_weight.SetValue( xmlnode.GetUInt32( "MinWeight" ), xmlnode.GetUInt32( "MaxWeight" ) );

        data._name = xmlnode.GetString( "Name" );
        data._key = xmlnode.GetUInt32( "Key" );
        data._range_value.SetValue( xmlnode.GetUInt32( "MinValue" ), xmlnode.GetUInt32( "MaxValue" ) );
        kfsetting->_punish_list.emplace_back( data );
    }

    //////////////////////////////////////////////////////////////////////////////////////////////////


}