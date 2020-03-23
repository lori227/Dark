#include "KFWeatherConfig.hpp"
#include "KFProtocol/KFProtocol.h"

namespace KFrame
{
    void KFWeatherConfig::ReadSetting( KFNode& xmlnode, KFWeatherSetting* kfsetting )
    {
        KFWeatherData weatherdata;
        weatherdata._weahter_id = xmlnode.GetUInt32( "Weather" );
        weatherdata._rand_weight = xmlnode.GetUInt32( "InitWeight" );

        auto strexploreduration = xmlnode.GetString( "ExploreDurationn" );
        weatherdata._min_realm_duration = KFUtility::SplitValue<uint32>( strexploreduration, __RANGE_STRING__ );
        weatherdata._max_realm_duration = KFUtility::SplitValue<uint32>( strexploreduration, __RANGE_STRING__ );
        if ( weatherdata._max_realm_duration == 0u )
        {
            weatherdata._max_realm_duration = weatherdata._min_realm_duration;
        }

        auto strpveduration = xmlnode.GetString( "PveDurationn" );
        weatherdata._min_pve_duration = KFUtility::SplitValue<uint32>( strpveduration, __RANGE_STRING__ );
        weatherdata._max_pve_duration = KFUtility::SplitValue<uint32>( strpveduration, __RANGE_STRING__ );
        if ( weatherdata._max_pve_duration == 0u )
        {
            weatherdata._min_pve_duration = weatherdata._max_pve_duration;
        }

        kfsetting->_total_rand_weight += weatherdata._rand_weight;
        kfsetting->_weather_data_list.emplace_back( weatherdata );
    }

    //////////////////////////////////////////////////////////////////////////////////////////////////
    const KFWeatherData* KFWeatherSetting::RandWeatherData() const
    {
        if ( _total_rand_weight != 0u )
        {
            auto value = 0u;
            auto rand = KFGlobal::Instance()->RandRatio( _total_rand_weight );
            for ( auto& weatherdata : _weather_data_list )
            {
                value += weatherdata._rand_weight;
                if ( rand < value )
                {
                    return &weatherdata;
                }
            }
        }

        return nullptr;
    }

    const KFWeatherData* KFWeatherSetting::GetCurrentWeatherData( uint32 weatherid ) const
    {
        for ( auto& weatherdata : _weather_data_list )
        {
            if ( weatherdata._weahter_id == weatherid )
            {
                return &weatherdata;
            }
        }

        return nullptr;
    }
}