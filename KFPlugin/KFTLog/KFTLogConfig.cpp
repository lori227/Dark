#include "KFTLogConfig.hpp"

namespace KFrame
{
    void KFTLogConfig::ReadSetting( KFNode& xmlnode, KFTLogSetting* kfsetting )
    {
        kfsetting->_str_value = xmlnode.GetString( "Value" );
        kfsetting->_uint64_value = __TO_UINT64__( kfsetting->_str_value );
    }
    ////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////
    uint64 KFTLogConfig::GetUInt64( const std::string& name )
    {
        auto kfsetting = FindSetting( name );
        if ( kfsetting == nullptr )
        {
            return _invalid_int;
        }

        return kfsetting->_uint64_value;
    }

    const std::string& KFTLogConfig::GetString( const std::string& name )
    {
        auto kfsetting = FindSetting( name );
        if ( kfsetting == nullptr )
        {
            return _invalid_string;
        }

        return kfsetting->_str_value;
    }
}