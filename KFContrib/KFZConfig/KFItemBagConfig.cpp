#include "KFItemBagConfig.hpp"

namespace KFrame
{
    void KFItemBagConfig::ReadSetting( KFNode& xmlnode, KFItemBagSetting* kfsetting )
    {
        kfsetting->_is_add_show = xmlnode.GetBoolen( "AddShow", true );
        kfsetting->_is_can_move = xmlnode.GetBoolen( "Move", true );
        kfsetting->_is_can_move_all = xmlnode.GetBoolen( "MoveAll", true );
        kfsetting->_is_can_clean = xmlnode.GetBoolen( "Clean", true );
        kfsetting->_is_realm_record = xmlnode.GetBoolen( "Record", true );
        kfsetting->_is_auto_use_gift = xmlnode.GetBoolen( "AutoUseGift", true );
        kfsetting->_extend_bag_name = xmlnode.GetString( "Extend", true );
        kfsetting->_use_child_bag_name = xmlnode.GetString( "UseChild", true );

        kfsetting->_enter_realm_type = xmlnode.GetUInt32( "EnterType", true );
        kfsetting->_balance_realm_type = xmlnode.GetUInt32( "BalanceType", true );
        kfsetting->_town_realm_type = xmlnode.GetUInt32( "TownType", true );

        auto strtablist = xmlnode.GetString( "TabList" );
        while ( !strtablist.empty() )
        {
            auto value = KFUtility::SplitString( strtablist, __SPLIT_STRING__ );
            if ( !value.empty() )
            {
                kfsetting->_tab_list.emplace( value );
            }
        }

        auto strmoveshow = xmlnode.GetString( "MoveShow" );
        while ( !strmoveshow.empty() )
        {
            auto value = KFUtility::SplitString( strmoveshow, __SPLIT_STRING__ );
            if ( !value.empty() )
            {
                kfsetting->_move_show_bag.emplace( value );
            }
        }

        auto strmoveaddupdata = xmlnode.GetString( "MoveAddUpdate" );
        while ( !strmoveaddupdata.empty() )
        {
            auto value = KFUtility::SplitString( strmoveaddupdata, __SPLIT_STRING__ );
            if ( !value.empty() )
            {
                kfsetting->_move_add_update_bag.emplace( value );
            }
        }

        auto strmovedecupdata = xmlnode.GetString( "MoveDecUpdate" );
        while ( !strmovedecupdata.empty() )
        {
            auto value = KFUtility::SplitString( strmovedecupdata, __SPLIT_STRING__ );
            if ( !value.empty() )
            {
                kfsetting->_move_dec_update_bag.emplace( value );
            }
        }
    }
}