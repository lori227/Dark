#include "KFMultiEventConfig.hpp"
#include "KFZConfig/KFElementConfig.h"

namespace KFrame
{
    void KFMultiEventConfig::ReadSetting( KFNode& xmlnode, KFMultiEventSetting* kfsetting )
    {
        auto index = xmlnode.GetUInt32( "Index" );
        auto kfdata = kfsetting->_multi_event_data_list.Create( index );
        kfdata->_str_cost = xmlnode.GetString( "Cost" );
        kfdata->_event_id = xmlnode.GetUInt32( "Event" );
    }
    //////////////////////////////////////////////////////////////////////////////////////////////////

    void KFMultiEventConfig::LoadAllComplete()
    {
        for ( auto& iter : _settings._objects )
        {
            auto kfsetting = iter.second;
            for ( auto& miter : kfsetting->_multi_event_data_list._objects )
            {
                auto kfdata = miter.second;
                KFElementConfig::Instance()->ParseElement( kfdata->_cost_elements, kfdata->_str_cost, __FILE__, kfsetting->_id );
            }
        }
    }
}