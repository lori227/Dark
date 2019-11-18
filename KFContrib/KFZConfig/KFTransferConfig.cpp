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
            __LOG_ERROR__( "id=[{}] config is error", kfsetting->_id );
        }

        kfsetting->_level = xmlnode.GetUInt32( "Level", true );

        auto stractivepool = xmlnode.GetString( "ActivePool", true );
        kfsetting->_active_pool_list = KFUtility::SplitList< VectorUInt32 >( stractivepool, __SPLIT_STRING__ );

        auto strcharacterpool = xmlnode.GetString( "CharacterPool", true );
        kfsetting->_character_pool_list = KFUtility::SplitList< VectorUInt32 >( strcharacterpool, __SPLIT_STRING__ );

        auto strinnatepool = xmlnode.GetString( "InnatePool", true );
        kfsetting->_innate_pool_list = KFUtility::SplitList< VectorUInt32 >( strinnatepool, __SPLIT_STRING__ );

        std::string strcost = xmlnode.GetString( "Cost", true );
        kfsetting->_cost.Parse( strcost, __FUNC_LINE__ );

        kfsetting->_consume_item_str = xmlnode.GetString( "ConsumeItem", true );
    }

    void KFTransferConfig::LoadAllComplete()
    {
        std::string item_str = "item";
        for ( auto& iter : _settings._objects )
        {
            if ( iter.second->_consume_item_str.empty() )
            {
                continue;
            }

            __JSON_PARSE_STRING__( kfjson, iter.second->_consume_item_str );

            if ( !kfjson.IsArray() )
            {
                continue;
            }

            std::string consumeitemstr;
            auto size = __JSON_ARRAY_SIZE__( kfjson );
            for ( auto i = 0u; i < size; ++i )
            {
                auto& jsonarray = __JSON_ARRAY_INDEX__( kfjson, i );
                if ( !jsonarray.IsArray() )
                {
                    continue;
                }

                auto len = __JSON_ARRAY_SIZE__( jsonarray );
                if ( len != 2u )
                {
                    continue;
                }

                auto itemid = jsonarray[0].GetUint();
                auto itemnum = jsonarray[1].GetUint();

                auto kfelementsetting = KFElementConfig::Instance()->FindElementSetting( item_str );
                auto strelement = __FORMAT__( kfelementsetting->_element_template, item_str, itemnum, itemid );
                consumeitemstr += strelement;
            }

            KFUtility::ReplaceString( consumeitemstr, "][", "," );
            iter.second->_consume_item.Parse( consumeitemstr, __FUNC_LINE__ );
        }
    }

    uint32 KFTransferConfig::GetTransferId( uint32 childid, uint32 parentid )
    {
        return childid * 10000u + parentid;
    }
}