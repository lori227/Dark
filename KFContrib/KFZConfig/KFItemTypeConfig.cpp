﻿#include "KFItemTypeConfig.hpp"

namespace KFrame
{
    void KFItemTypeConfig::ReadSetting( KFNode& xmlnode, KFItemTypeSeting* kfsetting )
    {
        kfsetting->_store_name = xmlnode.GetString( "Store", true );
        kfsetting->_bag_name = xmlnode.GetString( "Explore", true );
        kfsetting->_extend_name = xmlnode.GetString( "Extend", true );
        kfsetting->_is_auto = xmlnode.GetUInt32( "Auto", true );
        kfsetting->_sort_index = xmlnode.GetUInt32( "Sort", true );

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
    }

    bool KFItemTypeSeting::CheckCanMove( const std::string& sourcename, const std::string& targetname ) const
    {
        // 额外的背包, 并且是探索背包
        if ( sourcename == _extend_name && targetname == _bag_name )
        {
            return true;
        }

        return _move_name_list.find( targetname ) != _move_name_list.end();
    }
}