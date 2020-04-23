#include "KFLevelConfig.hpp"

namespace KFrame
{
    uint32 KFLevelConfig::GetLevelByExp( uint32 exp, uint32 level )
    {
        if ( exp >= _total_exp )
        {
            return static_cast<uint32>( _exp_map.size() );
        }

        auto iter = _exp_map.upper_bound( exp );
        if ( iter == _exp_map.end() )
        {
            return level;
        }

        return iter->second - 1;
    }

    void KFLevelConfig::ClearSetting()
    {
        _total_exp = 0u;
        _exp_map.clear();
        KFConfigT< KFLevelSetting >::ClearSetting();
    }

    void KFLevelConfig::ReadSetting( KFNode& xmlnode, KFLevelSetting* kfsetting )
    {
        auto exp = xmlnode.GetUInt32( "Exp", true );
        _total_exp += exp;
        kfsetting->_exp = _total_exp;
        _exp_map[_total_exp] = static_cast<uint32>( kfsetting->_id );

        kfsetting->_floor_attribute = xmlnode.GetUInt32( "Floor", true );
        kfsetting->_upper_attribute = xmlnode.GetUInt32( "Upper", true );
        kfsetting->_active_pool_list = xmlnode.GetUInt32Vector( "ActivePool", true );
        kfsetting->_innate_pool_list = xmlnode.GetUInt32Vector( "InnatePool", true );
    }
}