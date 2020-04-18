#include "KFTransferConfig.hpp"

namespace KFrame
{
    void KFTransferConfig::ClearSetting()
    {
        _parent_child_map.clear();
        _child_parent_map.clear();
        _transfer_level_map.clear();
        KFConfigT< KFTransferSetting >::ClearSetting();
    }

    void KFTransferConfig::ReadSetting( KFNode& xmlnode, KFTransferSetting* kfsetting )
    {
        kfsetting->_child_id = xmlnode.GetUInt32( "ChildId", true );
        kfsetting->_parent_id = xmlnode.GetUInt32( "ParentId", true );
        auto transferid = GetTransferId( kfsetting->_child_id, kfsetting->_parent_id );
        if ( transferid != kfsetting->_id )
        {
            __LOG_ERROR__( "id=[{}] transfer config id is error", kfsetting->_id );
        }

        kfsetting->_level = xmlnode.GetUInt32( "Level", true );

        auto stractivepool = xmlnode.GetString( "ActivePool", true );
        KFUtility::SplitList( kfsetting->_active_pool_list, stractivepool, __SPLIT_STRING__ );

        auto strcharacterpool = xmlnode.GetString( "CharacterPool", true );
        KFUtility::SplitList( kfsetting->_character_pool_list, strcharacterpool, __SPLIT_STRING__ );

        auto strinnatepool = xmlnode.GetString( "InnatePool", true );
        KFUtility::SplitList( kfsetting->_innate_pool_list, strinnatepool, __SPLIT_STRING__ );

        kfsetting->_cost._str_element = xmlnode.GetString( "Cost", true );

        _parent_child_map[kfsetting->_parent_id].push_back( kfsetting->_child_id );
        _child_parent_map[kfsetting->_child_id].push_back( kfsetting->_parent_id );
        _transfer_level_map[kfsetting->_parent_id] = kfsetting->_level;
    }

    void KFTransferConfig::LoadAllComplete()
    {
        for ( auto& iter : _settings._objects )
        {
            auto kfsetting = iter.second;
            KFElementConfig::Instance()->ParseElement( kfsetting->_cost, kfsetting->_cost._str_element, __FILE__, kfsetting->_id );
        }
    }

    const KFTransferSetting* KFTransferConfig::FindTransferSetting( uint32 childid, uint32 parentid )
    {
        auto id = GetTransferId( childid, parentid );
        return FindSetting( id );
    }

    uint32 KFTransferConfig::GetTransferId( uint32 childid, uint32 parentid )
    {
        return childid * 10000u + parentid;
    }

    uint32 KFTransferConfig::GetRandChildId( uint32 parentid )
    {
        auto iter = _parent_child_map.find( parentid );
        if ( iter == _parent_child_map.end() )
        {
            return _invalid_int;
        }

        auto count = static_cast<uint32>( iter->second.size() );
        if ( count == 0u )
        {
            return _invalid_int;
        }

        auto randcount = KFGlobal::Instance()->RandRatio( count );
        return iter->second[randcount];
    }

    uint32 KFTransferConfig::GetRandParentId( uint32 childid )
    {
        auto iter = _child_parent_map.find( childid );
        if ( iter == _child_parent_map.end() )
        {
            return _invalid_int;
        }

        auto count = static_cast<uint32>( iter->second.size() );
        if ( count == 0u )
        {
            return _invalid_int;
        }

        auto randcount = KFGlobal::Instance()->RandRatio( count );
        return iter->second[randcount];
    }

    uint32 KFTransferConfig::GetProTransferLevel( uint32 profession )
    {
        auto iter = _transfer_level_map.find( profession );
        if ( iter != _transfer_level_map.end() )
        {
            return iter->second;
        }

        return _invalid_int;
    }
}