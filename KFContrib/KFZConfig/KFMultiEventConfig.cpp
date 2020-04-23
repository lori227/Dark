#include "KFMultiEventConfig.hpp"
#include "KFZConfig/KFElementConfig.h"

namespace KFrame
{
    void KFMultiEventConfig::ReadSetting( KFNode& xmlnode, KFMultiEventSetting* kfsetting )
    {
        auto index = xmlnode.GetUInt32( "Index" );
        auto kfdata = kfsetting->_multi_event_data_list.Create( index );
        kfdata->_cost_elements._str_parse = xmlnode.GetString( "Cost" );
        kfdata->_event_list = xmlnode.GetUInt32Vector( "Event" );
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
                KFElementConfig::Instance()->ParseElement( kfdata->_cost_elements, __FILE__, kfsetting->_id );
            }
        }
    }
}