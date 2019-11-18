#include "KFCurrencyConfig.hpp"

namespace KFrame
{
    void KFCurrencyConfig::ReadSetting( KFNode& xmlnode, KFCurrencySetting* kfsetting )
    {
        kfsetting->_name = xmlnode.GetString( "Name", true );
    }
}