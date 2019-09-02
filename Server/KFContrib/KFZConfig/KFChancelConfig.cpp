#include "KFChancelConfig.hpp"

namespace KFrame
{
    void KFChancelConfig::ClearSetting()
    {
        _type_list.clear();
        KFIntConfigT< KFChancelSetting >::ClearSetting();
    }

    void KFChancelConfig::ReadSetting( KFNode& xmlnode, KFChancelSetting* kfsetting )
    {
        kfsetting->_type = xmlnode.GetUInt32( "Type", true );
        kfsetting->_level = xmlnode.GetUInt32( "Stage", true );

        auto id = GetChancelId( kfsetting->_type, kfsetting->_level );
        if ( id != kfsetting->_id )
        {
            __LOG_ERROR__( "herochancel config id error, id = [{}]!", kfsetting->_id );
        }

        _type_list.insert( kfsetting->_type );

        std::string conditionstr = xmlnode.GetString( "Condition", true );

        __JSON_PARSE_STRING__( kfjson, conditionstr );

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

            auto type = jsonarray[0].GetUint();
            auto num = jsonarray[1].GetUint();

            kfsetting->_condition_map[type] = num;
        }
    }

    uint32 KFChancelConfig::GetChancelId( uint32 type, uint32 level )
    {
        return type * 100u + level;
    }

    bool KFChancelConfig::IsValid( uint32 type )
    {
        return _type_list.find( type ) != _type_list.end();
    }
}