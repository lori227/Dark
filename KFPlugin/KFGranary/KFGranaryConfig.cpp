#include "KFGranaryConfig.hpp"

namespace KFrame
{
    void KFGranaryConfig::ReadSetting( KFNode& xmlnode, KFGranarySetting* kfsetting )
    {
        kfsetting->_consume._str_parse = xmlnode.GetString( "Consume", true );
    }

    void KFGranaryConfig::LoadAllComplete()
    {
        for ( auto& iter : _settings._objects )
        {
            auto kfsetting = iter.second;
            KFElementConfig::Instance()->ParseElement( kfsetting->_consume, __FILE__, kfsetting->_id );
        }
    }
}