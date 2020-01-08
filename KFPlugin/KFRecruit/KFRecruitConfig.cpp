#include "KFRecruitConfig.hpp"

namespace KFrame
{
    /////////////////////////////////////////////////////////////////////////////////
    void KFRecruitConfig::ReadSetting( KFNode& xmlnode, KFRecruitSetting* kfsetting )
    {
        kfsetting->_is_clear = xmlnode.GetBoolen( "Clear", true );
        kfsetting->_generate_technology_id = xmlnode.GetUInt32( "GenerateId" );
        kfsetting->_str_cost = xmlnode.GetString( "Cost", true );
    }

    void KFRecruitConfig::LoadAllComplete()
    {
        for ( auto& iter : _settings._objects )
        {
            auto kfsetting = iter.second;
            KFElementConfig::Instance()->ParseElement( kfsetting->_cost_elements, kfsetting->_str_cost, __FILE__, kfsetting->_id );
        }
    }
}