﻿#include "KFGambleConfig.hpp"

namespace KFrame
{
    void KFGambleConfig::ReadSetting( KFNode& xmlnode, KFGambleSetting* kfsetting )
    {
        kfsetting->_cost_item_id = xmlnode.GetUInt32( "CostItemId" );
        kfsetting->_init_count = xmlnode.GetUInt32( "Price" );
        kfsetting->_max_count = xmlnode.GetUInt32( "MaxPrice" );
        if ( kfsetting->_max_count == 0u )
        {
            kfsetting->_max_count = __MAX_UINT32__;
        }

        kfsetting->_params = xmlnode.GetUInt32Vector( "Params" );
        kfsetting->_show_count = xmlnode.GetUInt32( "ShowCount" );
        kfsetting->_drop_type = xmlnode.GetUInt32( "DropType", true );
        kfsetting->_normal_drop_id = xmlnode.GetUInt32( "NormalDrop" );
        kfsetting->_inner_drop_id = xmlnode.GetUInt32( "InnerDrop" );
    }

    //////////////////////////////////////////////////////////////////////////////////////////////////
    void KFGambleConfig::LoadAllComplete()
    {
        for ( auto& iter : _settings._objects )
        {
            auto kfsetting = iter.second;
            KFElementConfig::Instance()->FormatElement( kfsetting->_cost_item, __STRING__( item ), 1u, kfsetting->_cost_item_id );
        }
    }

}