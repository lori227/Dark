#include "KFRewardConfig.hpp"
#include "KFProtocol/KFProtocol.h"

namespace KFrame
{
    void KFRewardConfig::ClearSetting()
    {
        _id_name_list.clear();
        KFConfigT< KFRewardSetting >::ClearSetting();
    }

    // 读取配置
    void KFRewardConfig::ReadSetting( KFNode& xmlnode, KFRewardSetting* kfsetting )
    {
        kfsetting->_type = xmlnode.GetUInt32( "Type" );
        kfsetting->_code = xmlnode.GetUInt32( "Code" );
        kfsetting->_element_template = xmlnode.GetString( "Template" );

        auto idvalue = CalcTypeCodeValue( kfsetting->_type, kfsetting->_code );
        _id_name_list[ idvalue ] = kfsetting->_id;
    }

    uint64 KFRewardConfig::CalcTypeCodeValue( uint64 type, uint64 code )
    {
#define __TYPE_VALUE__ 10000000000

        uint64 result = 0u;
        switch ( type )
        {
        case KFMsg::CurrenyType:
            result = type * __TYPE_VALUE__ + code;
            break;
        default:
            result = type * __TYPE_VALUE__;
            break;
        }

        return result;
    }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    const std::string& KFRewardConfig::StringElement( const std::string& dataname, uint32 datavalue, uint32 dataid )
    {
        static std::string _str_element;
        _str_element.clear();

        auto kfsetting = FindSetting( dataname );
        if ( kfsetting != nullptr )
        {
            auto strdata = __FORMAT__( kfsetting->_element_template, datavalue, dataid );
            _str_element = __FORMAT__( "[{}]", strdata );
        }

        return _str_element;
    }

    bool KFRewardConfig::FormatElement( KFElements& kfelements, const std::string& dataname, const std::string& datavalue, uint32 dataid )
    {
        auto kfsetting = FindSetting( dataname );
        if ( kfsetting == nullptr )
        {
            return false;
        }

        auto strelement = __FORMAT__( kfsetting->_element_template, datavalue, dataid );
        auto strdata = __FORMAT__( "[{}]", strelement );
        return kfelements.Parse( strdata, __FUNC_LINE__ );
    }

    bool KFRewardConfig::ParseElement( KFElements& kfelements, const std::string& strelement )
    {
        if ( strelement.empty() )
        {
            return true;
        }

        // 将字符串解析成数组
        __JSON_PARSE_STRING__( kfjson, strelement );
        if ( !kfjson.IsArray() )
        {
            return false;
        }

        std::string strdata = "[";
        auto size = __JSON_ARRAY_SIZE__( kfjson );
        for ( uint32 i = 0u; i < size; ++i )
        {
            auto& jsonarray = __JSON_ARRAY_INDEX__( kfjson, i );
            if ( !jsonarray.IsArray() )
            {
                return false;
            }

            auto len = __JSON_ARRAY_SIZE__( jsonarray );
            if ( len != 3u )
            {
                return false;
            }

            auto type = jsonarray[ 0 ].GetUint();
            auto code = jsonarray[ 1 ].GetUint();
            auto num = jsonarray[ 2 ].GetUint();

            auto& strreward = FormatElement( type, code, num );
            if ( strreward.empty() )
            {
                return false;
            }

            if ( i > 0u )
            {
                strdata += ",";
            }

            strdata += strreward;
        }
        strdata += "]";

        return kfelements.Parse( strdata, __FUNC_LINE__ );
    }

    const std::string& KFRewardConfig::FormatElement( uint32 type, uint32 code, uint32 num )
    {
        auto idvalue = CalcTypeCodeValue( type, code );
        auto iter = _id_name_list.find( idvalue );
        if ( iter == _id_name_list.end() )
        {
            return _invalid_string;
        }

        static std::string _str_element;
        _str_element.clear();

        auto kfsetting = FindSetting( iter->second );
        if ( kfsetting != nullptr )
        {
            _str_element = __FORMAT__( kfsetting->_element_template, num, code );
        }

        return _str_element;
    }

}