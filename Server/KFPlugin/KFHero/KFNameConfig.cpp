#include "KFNameConfig.hpp"

namespace KFrame
{
    /////////////////////////////////////////////////////////////////////////////////
    bool KFNameConfig::LoadConfig()
    {
        _name_setting.Clear();
        //////////////////////////////////////////////////////////////////
        KFXml kfxml( _file );
        auto config = kfxml.RootNode();
        auto xmlnode = config.FindNode( "Setting" );
        while ( xmlnode.IsValid() )
        {
            auto id = xmlnode.GetUInt32( "Id" );
            auto name = xmlnode.GetString( "Name" );

            auto kfsetting = _name_setting.Create( id );
            kfsetting->_name_list.push_back( name );

            xmlnode.NextNode();
        }
        //////////////////////////////////////////////////////////////////

        return true;
    }

    /////////////////////////////////////////////////////////////////////////////////
    const std::string& KFNameConfig::RandName( uint32 id ) const
    {
        auto kfsetting = _name_setting.Find( id );
        if ( kfsetting == nullptr || kfsetting->_name_list.empty() )
        {
            __LOG_ERROR__( "name=[{}] is empty!", id );
            return _invalid_str;
        }

        auto index = KFGlobal::Instance()->RandRatio( static_cast< uint32 >( kfsetting->_name_list.size() ) );
        return kfsetting->_name_list.at( index );
    }
}