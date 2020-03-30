#include "KFRealmTimeConfig.hpp"
#include "KFProtocol/KFProtocol.h"

namespace KFrame
{
    void KFRealmTimeConfig::ReadSetting( KFNode& xmlnode, KFRealmTimeSetting* kfsetting )
    {
        KFRealmTimeData timedata;
        timedata._time_type = xmlnode.GetUInt32( "Time" );
        timedata._rand_weight = xmlnode.GetUInt32( "InitWeight" );

        auto strexploreduration = xmlnode.GetString( "ExploreDurationn" );
        timedata._min_realm_duration = KFUtility::SplitValue<uint32>( strexploreduration, __RANGE_STRING__ );
        timedata._max_realm_duration = KFUtility::SplitValue<uint32>( strexploreduration, __RANGE_STRING__ );
        if ( timedata._max_realm_duration == 0u )
        {
            timedata._max_realm_duration = timedata._min_realm_duration;
        }

        auto strpveduration = xmlnode.GetString( "PveDurationn" );
        timedata._min_pve_duration = KFUtility::SplitValue<uint32>( strpveduration, __RANGE_STRING__ );
        timedata._max_pve_duration = KFUtility::SplitValue<uint32>( strpveduration, __RANGE_STRING__ );
        if ( timedata._max_pve_duration == 0u )
        {
            timedata._max_pve_duration = timedata._min_pve_duration;
        }

        kfsetting->_total_rand_weight += timedata._rand_weight;
        kfsetting->_time_data_list.emplace_back( timedata );
    }

    //////////////////////////////////////////////////////////////////////////////////////////////////
    const KFRealmTimeData* KFRealmTimeSetting::RandRealmTimeData() const
    {
        if ( _total_rand_weight != 0u )
        {
            auto value = 0u;
            auto rand = KFGlobal::Instance()->RandRatio( _total_rand_weight );
            for ( auto& weatherdata : _time_data_list )
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

    const KFRealmTimeData* KFRealmTimeSetting::GetCurrentRealmTimeData( uint32 timetype ) const
    {
        for ( auto& weatherdata : _time_data_list )
        {
            if ( weatherdata._time_type == timetype )
            {
                return &weatherdata;
            }
        }

        return nullptr;
    }

    const KFRealmTimeData* KFRealmTimeSetting::GetNextRealmTimeData( uint32 timetype ) const
    {
        ++timetype;
        if ( timetype > KFMsg::Night )
        {
            timetype = KFMsg::Morning;
        }

        for ( auto& weatherdata : _time_data_list )
        {
            if ( weatherdata._time_type == timetype )
            {
                return &weatherdata;
            }
        }

        return &_time_data_list.front();
    }
}