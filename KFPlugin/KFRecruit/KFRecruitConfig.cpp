#include "KFRecruitConfig.hpp"

namespace KFrame
{
    /////////////////////////////////////////////////////////////////////////////////
    void KFRefreshConfig::ReadSetting( KFNode& xmlnode, KFRefreshSetting* kfsetting )
    {
        kfsetting->_count = xmlnode.GetUInt32( "Count" );
        kfsetting->_is_clear = xmlnode.GetBoolen( "Clear", true );
        kfsetting->_display_id = xmlnode.GetBoolen( "Display", true );

        auto strcost = xmlnode.GetString( "Cost", true );
        kfsetting->_cost_elements.Parse( strcost, __FUNC_LINE__ );

        auto strgenerate = xmlnode.GetString( "Generate" );
        kfsetting->_hero_generate.Parse( strgenerate, __FUNC_LINE__ );
    }
}