#include "KFQualityConfig.hpp"

namespace KFrame
{
    /////////////////////////////////////////////////////////////////////////////////
    bool KFQualityConfig::LoadConfig()
    {
        _total_weight = 0u;
        _quality_setting.Clear();
        //////////////////////////////////////////////////////////////////
        KFXml kfxml( _file );
        auto config = kfxml.RootNode();
        auto xmlnode = config.FindNode( "Setting" );
        while ( xmlnode.IsValid() )
        {
            auto quality = xmlnode.GetUInt32( "Quality" );
            auto kfsetting = _quality_setting.Create( quality );

            kfsetting->_quality = quality;
            kfsetting->_weight = xmlnode.GetUInt32( "Weight" );
            kfsetting->_basic_point = xmlnode.GetUInt32( "Basic" );
            kfsetting->_growup_point = xmlnode.GetUInt32( "Growup" );

            _total_weight += kfsetting->_weight;

            xmlnode.NextNode();
        }
        //////////////////////////////////////////////////////////////////

        return true;
    }

    /////////////////////////////////////////////////////////////////////////////////
    const KFQualitySetting* KFQualityConfig::RandQuality() const
    {
        auto rand = KFGlobal::Instance()->RandRatio( _total_weight );

        auto value = 0u;
        for ( auto& iter : _quality_setting._objects )
        {
            auto kfsetting = iter.second;
            value += kfsetting->_weight;

            if ( rand < value )
            {
                return kfsetting;
            }
        }

        return nullptr;
    }

    const KFQualitySetting* KFQualityConfig::FindQuality( uint32 quality ) const
    {
        return _quality_setting.Find( quality );
    }
}