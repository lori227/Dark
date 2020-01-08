#include "KFTransferConfig.hpp"

namespace KFrame
{
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

        kfsetting->_str_cost = xmlnode.GetString( "Cost", true );
    }

    void KFTransferConfig::LoadAllComplete()
    {
        for ( auto& iter : _settings._objects )
        {
            auto kfsetting = iter.second;
            KFElementConfig::Instance()->ParseElement( kfsetting->_cost, kfsetting->_str_cost, __FILE__, kfsetting->_id );
        }
    }

    uint32 KFTransferConfig::GetTransferId( uint32 childid, uint32 parentid )
    {
        return childid * 10000u + parentid;
    }

    uint32 KFTransferConfig::GetParentPro( uint32 childpro )
    {
        for ( auto& iter : _settings._objects )
        {
            auto kfsetting = iter.second;
            if ( kfsetting->_child_id == childpro )
            {
                return kfsetting->_parent_id;
            }
        }

        return _invalid_int;
    }
}