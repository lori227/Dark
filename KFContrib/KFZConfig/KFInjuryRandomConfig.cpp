#include "KFInjuryRandomConfig.hpp"

namespace KFrame
{
    const KFInjuryRandomSetting* KFInjuryRandomConfig::FindInjureRandomSetting( uint32 id )
    {
        auto enditer = _injure_map.rbegin();
        if ( enditer == _injure_map.rend() )
        {
            return nullptr;
        }

        if ( id > enditer->first )
        {
            return enditer->second;
        }

        auto iter = _injure_map.lower_bound( id );
        if ( iter == _injure_map.end() )
        {
            return enditer->second;
        }

        return iter->second;
    }

    void KFInjuryRandomConfig::ClearSetting()
    {
        _injure_map.clear();
        KFConfigT< KFInjuryRandomSetting >::ClearSetting();
    }

    void KFInjuryRandomConfig::ReadSetting( KFNode& xmlnode, KFInjuryRandomSetting* kfsetting )
    {
        kfsetting->_pool = xmlnode.GetUInt32( "Pool" );
        kfsetting->_dead_prob = xmlnode.GetUInt32( "DeadProb" );

        auto id = static_cast<uint32>( kfsetting->_id );
        _injure_map[id] = kfsetting;
    }
    //////////////////////////////////////////////////////////////////////////////////////////////////
}