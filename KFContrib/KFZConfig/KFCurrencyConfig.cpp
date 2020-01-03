#include "KFCurrencyConfig.hpp"

namespace KFrame
{
    void KFCurrencyConfig::ReadSetting( KFNode& xmlnode, KFCurrencySetting* kfsetting )
    {
        kfsetting->_name = xmlnode.GetString( "Name", true );

        _currency_map[kfsetting->_name] = kfsetting->_id;
    }

    uint64 KFCurrencyConfig::GetIdByName( const std::string& name )
    {
        auto iter = _currency_map.find( name );
        if ( iter != _currency_map.end() )
        {
            return iter->second;
        }

        return _invalid_int;
    }
}