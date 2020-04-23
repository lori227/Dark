#include "KFItemTypeConfig.hpp"

namespace KFrame
{
    void KFItemTypeConfig::ReadSetting( KFNode& xmlnode, KFItemTypeSetting* kfsetting )
    {
        kfsetting->_is_auto = xmlnode.GetUInt32( "Auto", true );
        kfsetting->_sort_index = xmlnode.GetUInt32( "Sort", true );
        kfsetting->_status_bag_name = xmlnode.GetUInt64String( "Storage", true );
        kfsetting->_move_name_list = xmlnode.GetStringSet( "Move", true );
        kfsetting->_tab_name_list = xmlnode.GetStringSet( "Tab", true );

        auto struselimit = xmlnode.GetString( "UseLimit", true );
        while ( !struselimit.empty() )
        {
            auto usemask = KFUtility::SplitValue< uint32 >( struselimit, __SPLIT_STRING__ );
            KFUtility::AddBitMask( kfsetting->_use_limit, usemask );
        }
    }

    bool KFItemTypeSetting::CheckCanMove( const std::string& sourcename, const std::string& targetname ) const
    {
        return _move_name_list.find( targetname ) != _move_name_list.end();
    }

    bool KFItemTypeSetting::IsHaveTab( const std::string& name ) const
    {
        return _tab_name_list.find( name ) != _tab_name_list.end();
    }

    const std::string& KFItemTypeSetting::GetBagName( uint32 status ) const
    {
        auto iter = _status_bag_name.find( status );
        if ( iter == _status_bag_name.end() )
        {
            iter = _status_bag_name.find( 0 );
            if ( iter == _status_bag_name.end() )
            {
                return _invalid_string;
            }
        }

        return iter->second;
    }
}