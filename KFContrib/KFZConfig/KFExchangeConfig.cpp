#include "KFExchangeConfig.hpp"

namespace KFrame
{
    void KFExchangeConfig::ReadSetting( KFNode& xmlnode, KFExchangeSetting* kfsetting )
    {
        auto level = xmlnode.GetUInt32( "Level" );
        auto exchange = kfsetting->_exchange_list.Create( level );
        exchange->_drop_type = xmlnode.GetUInt32( "DropType", true );
        exchange->_normal_drop_id = xmlnode.GetUInt32( "NormalDrop" );
        exchange->_inner_drop_id = xmlnode.GetUInt32( "InnerDrop" );
    }

    //////////////////////////////////////////////////////////////////////////////////////////////////
}