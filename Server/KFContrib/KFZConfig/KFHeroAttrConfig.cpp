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
                data->_min_value = KFUtility::SplitValue< uint32 >( strline, FUNCTION_RANGE_STRING );
                data->_max_value = KFUtility::SplitValue< uint32 >( strline, FUNCTION_RANGE_STRING );
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////
    uint32 KFAttributeData::GetValue() const
    {
        if ( _min_value >= _max_value )
        {
            return _min_value;
        }

        return KFGlobal::Instance()->RandRange( _min_value, _max_value, 1 );
    }

}