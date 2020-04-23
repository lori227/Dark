#include "KFQualityConfig.hpp"

namespace KFrame
{
    /////////////////////////////////////////////////////////////////////////////////
    void KFQualityConfig::ReadSetting( KFNode& xmlnode, KFQualitySetting* kfsetting )
    {
        kfsetting->_innate_num = xmlnode.GetUInt32( "InnateNum", true );

        kfsetting->_innate_quality.SetValue( xmlnode.GetString( "InnateQuality", true ) );
        kfsetting->_active_quality.SetValue( xmlnode.GetString( "SkillQuality", true ) );
        kfsetting->_role_durability.SetValue( xmlnode.GetString( "RoleDurability" ) );

        std::list< std::string > keylist;
        xmlnode.GetKeyList( keylist );

        for ( auto& key : keylist )
        {
            auto attrname = key;
            KFUtility::SplitString( attrname, "Attr_" );
            if ( !attrname.empty() )
            {
                auto data = kfsetting->_growth.Create( attrname );
                data->SetValue( xmlnode.GetString( key.c_str() ) );
            }
        }
    }

}