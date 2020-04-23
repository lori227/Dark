#include "KFHeroAttrConfig.hpp"

namespace KFrame
{
    /////////////////////////////////////////////////////////////////////////////////
    void KFHeroAttributeConfig::ReadSetting( KFNode& xmlnode, KFAttributeSetting* kfsetting )
    {
        std::list< std::string > keylist;
        xmlnode.GetKeyList( keylist );

        for ( auto& key : keylist )
        {
            auto attrname = key;
            KFUtility::SplitString( attrname, "_" );
            if ( !attrname.empty() )
            {
                auto data = kfsetting->_datas.Create( attrname );

                auto strline = xmlnode.GetString( key.c_str() );
                data->_range.SetValue( strline );
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////
    uint32 KFAttributeData::GetValue() const
    {
        return _range.CalcValue();
    }
}