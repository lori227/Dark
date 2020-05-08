#include "KFItemBagConfig.hpp"

namespace KFrame
{
    void KFItemBagConfig::ReadSetting( KFNode& xmlnode, KFItemBagSetting* kfsetting )
    {
        auto isdefault = xmlnode.GetBoolen( "Default" );
        if ( isdefault )
        {
            _default_bag_name = kfsetting->_id;
        }

        kfsetting->_is_add_show = xmlnode.GetBoolen( "AddShow", true );
        kfsetting->_is_can_move = xmlnode.GetBoolen( "Move", true );
        kfsetting->_is_can_move_all = xmlnode.GetBoolen( "MoveAll", true );
        kfsetting->_is_can_clean = xmlnode.GetBoolen( "Clean", true );
        kfsetting->_is_realm_record = xmlnode.GetBoolen( "Record", true );
        kfsetting->_is_auto_use_gift = xmlnode.GetBoolen( "AutoUseGift", true );
        kfsetting->_extend_bag_name = xmlnode.GetString( "Extend", true );
        kfsetting->_use_child_bag_name = xmlnode.GetString( "UseChild", true );
        kfsetting->_index_name = xmlnode.GetString( "IndexName", true );

        kfsetting->_enter_realm_type = xmlnode.GetUInt32( "EnterType", true );
        kfsetting->_balance_realm_type = xmlnode.GetUInt32( "BalanceType", true );
        kfsetting->_town_realm_type = xmlnode.GetUInt32( "TownType", true );
        kfsetting->_move_show_bag = xmlnode.GetStringSet( "MoveShow" );
        kfsetting->_move_add_update_bag = xmlnode.GetStringSet( "MoveAddUpdate" );
        kfsetting->_move_dec_update_bag = xmlnode.GetStringSet( "MoveDecUpdate" );

        kfsetting->_tab_list = xmlnode.GetStringSet( "TabList" );
        if ( !kfsetting->_index_name.empty() )
        {
            kfsetting->_tab_list.insert( kfsetting->_index_name );
        }
    }
}