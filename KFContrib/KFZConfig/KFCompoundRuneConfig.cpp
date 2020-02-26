#include "KFCompoundRuneConfig.hpp"

namespace KFrame
{
    void KFCompoundRuneConfig::ReadSetting( KFNode& xmlnode, KFCompoundRuneSetting* kfsetting )
    {
        auto index = 1u;
        while ( true )
        {
            auto strkey = __FORMAT__( "Compound{}", index++ );
            auto ok = xmlnode.HaveChild( strkey.c_str() );
            if ( !ok )
            {
                break;
            }

            auto itemid = xmlnode.GetInt32( strkey.c_str() );
            kfsetting->_compound_ids.push_back( itemid );
        }
    }

    uint32 KFCompoundRuneSetting::GetLevelById( uint32 compoundid ) const
    {
        for ( auto level = 0u; level < _compound_ids.size(); level++ )
        {
            if ( _compound_ids.at( level ) == compoundid )
            {
                return level + 1;
            }
        }

        return _invalid_int;
    }

    uint32 KFCompoundRuneSetting::GetIdByLevel( uint32 level ) const
    {
        if ( level > _compound_ids.size() )
        {
            return _invalid_int;
        }

        return _compound_ids.at( level - 1 );
    }

}