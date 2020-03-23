#include "KFQualityConfig.hpp"

namespace KFrame
{
    uint32 KFQualityConfig::GetQualityByGrowth( uint32 growth )
    {
        if ( growth > _max_growth )
        {
            return static_cast<uint32>( _quality_map.size() );
        }

        auto iter = _quality_map.lower_bound( growth );
        if ( iter == _quality_map.end() )
        {
            return 1u;
        }

        return iter->second;
    }

    void KFQualityConfig::ClearSetting()
    {
        _max_growth = 0u;
        _quality_map.clear();
        KFConfigT< KFQualitySetting >::ClearSetting();
    }

    void KFQualityConfig::ReadSetting( KFNode& xmlnode, KFQualitySetting* kfsetting )
    {
        kfsetting->_max_growth = xmlnode.GetUInt32( "GrowthMax", true );

        if ( kfsetting->_max_growth > _max_growth )
        {
            _max_growth = kfsetting->_max_growth;
        }

        _quality_map[kfsetting->_max_growth] = static_cast<uint32>( kfsetting->_id );
    }
}