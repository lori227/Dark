#include "KFRaceConfig.hpp"

namespace KFrame
{
    /////////////////////////////////////////////////////////////////////////////////
    void KFRaceConfig::ReadSetting( KFNode& xmlnode, KFRaceSetting* kfsetting )
    {
        kfsetting->_surname_id = xmlnode.GetUInt32( "SurName" );

        auto strname = xmlnode.GetString( "FirstName" );
        kfsetting->_male_firstname_id = KFUtility::SplitValue< uint32 >( strname, DEFAULT_SPLIT_STRING );
        kfsetting->_female_firstname_id = KFUtility::SplitValue< uint32 >( strname, DEFAULT_SPLIT_STRING );

        // 属性偏好率
        std::list< std::string > keylist;
        xmlnode.GetKeyList( keylist );
        for ( auto& key : keylist )
        {
            auto attrname = key;
            KFUtility::SplitString( attrname, "_" );
            if ( !attrname.empty() )
            {
                uint32 value = xmlnode.GetUInt32( key.c_str() );
                kfsetting->_attribute[ attrname ] = value;
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////
    uint32 KFRaceSetting::GetAttributeValue( const std::string& name ) const
    {
        auto iter = _attribute.find( name );
        if ( iter == _attribute.end() )
        {
            return 0;
        }

        return iter->second;
    }
}