#include "KFAttrProbabilityConfig.hpp"

namespace KFrame
{
    /////////////////////////////////////////////////////////////////////////////////
    bool KFAttrProbabilityConfig::LoadConfig()
    {
        _probability_setting.Clear();
        //////////////////////////////////////////////////////////////////
        KFXml kfxml( _file );
        auto config = kfxml.RootNode();
        auto xmlnode = config.FindNode( "Setting" );
        while ( xmlnode.IsValid() )
        {
            auto id = xmlnode.GetUInt32( "Id" );
            auto kfsetting = _probability_setting.Create( id );

            std::list< std::string > keylist;
            xmlnode.GetKeyList( keylist );

            for ( auto& key : keylist )
            {
                auto attrname = key;
                KFUtility::SplitString( attrname, ":" );
                if ( !attrname.empty() )
                {
                    auto weight = xmlnode.GetUInt32( key.c_str() );

                    auto data = kfsetting->_probability_list.Create( 0, weight );
                    data->_data_name = attrname;
                }
            }

            xmlnode.NextNode();
        }
        //////////////////////////////////////////////////////////////////

        return true;
    }

    /////////////////////////////////////////////////////////////////////////////////
    const KFAttrProbabilityData* KFAttrProbabilityConfig::RandAttribute( uint32 id ) const
    {
        auto kfsetting = _probability_setting.Find( id );
        if ( kfsetting == nullptr )
        {
            return nullptr;
        }

        return kfsetting->_probability_list.Rand();
    }

    const KFAttrProbabilitySetting* KFAttrProbabilityConfig::FindAttrProbabilitySetting( uint32 id ) const
    {
        return _probability_setting.Find( id );
    }
}