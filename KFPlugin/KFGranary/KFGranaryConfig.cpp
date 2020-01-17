#include "KFGranaryConfig.hpp"

namespace KFrame
{
    void KFGranaryConfig::ReadSetting( KFNode& xmlnode, KFGranarySetting* kfsetting )
    {
        kfsetting->_str_consume = xmlnode.GetString( "Consume", true );
    }

    void KFGranaryConfig::LoadAllComplete()
    {
        for ( auto& iter : _settings._objects )
        {
            auto kfsetting = iter.second;
            KFElementConfig::Instance()->ParseElement( kfsetting->_consume, kfsetting->_str_consume, __FILE__, kfsetting->_id );
        }
    }
}