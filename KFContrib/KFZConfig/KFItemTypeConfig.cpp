#include "KFItemTypeConfig.hpp"

namespace KFrame
{
    void KFItemTypeConfig::ReadSetting( KFNode& xmlnode, KFItemTypeSetting* kfsetting )
    {
        kfsetting->_is_auto = xmlnode.GetUInt32( "Auto", true );
        kfsetting->_sort_index = xmlnode.GetUInt32( "Sort", true );

        auto strstorage = xmlnode.GetString( "Storage", true );
        while ( !strstorage.empty() )
        {
            auto statusstorage = KFUtility::SplitString( strstorage, __SPLIT_STRING__ );
            if ( !statusstorage.empty() )
            {
                auto status = KFUtility::SplitValue<uint32>( statusstorage, __DOMAIN_STRING__ );
                auto name = KFUtility::SplitString( statusstorage, __DOMAIN_STRING__ );
                if ( !name.empty() )
                {
                    kfsetting->_status_bag_name[ status ] = name;
                }
            }
        }

        auto strmove = xmlnode.GetString( "Move", true );
        while ( !strmove.empty() )
        {
            auto movename = KFUtility::SplitString( strmove, __SPLIT_STRING__ );
            if ( !movename.empty() )
            {
                kfsetting->_move_name_list.insert( movename );
            }
        }

        auto struselimit = xmlnode.GetString( "UseLimit", true );
        while ( !struselimit.empty() )
        {
            auto usemask = KFUtility::SplitValue< uint32 >( struselimit, __SPLIT_STRING__ );
            KFUtility::AddBitMask( kfsetting->_use_limit, usemask );
        }

        auto strtab = xmlnode.GetString( "Tab", true );
        while ( !strtab.empty() )
        {
            auto tabname = KFUtility::SplitString( strtab, __SPLIT_STRING__ );
            if ( !tabname.empty() )
            {
                kfsetting->_tab_name_list.insert( tabname );
            }
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