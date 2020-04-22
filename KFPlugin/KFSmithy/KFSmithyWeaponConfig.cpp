#include "KFSmithyWeaponConfig.hpp"

namespace KFrame
{
    void KFSmithyWeaponConfig::ReadSetting( KFNode& xmlnode, KFSmithyWeaponSetting* kfsetting )
    {
        kfsetting->_need_num = xmlnode.GetUInt64( "NeedNum", true );
        kfsetting->_consume._str_parse = xmlnode.GetString( "Consume", true );
    }

    void KFSmithyWeaponConfig::LoadAllComplete()
    {
        for ( auto& iter : _settings._objects )
        {
            auto kfsetting = iter.second;
            KFElementConfig::Instance()->ParseElement( kfsetting->_consume, __FILE__, kfsetting->_id );
            KFElementConfig::Instance()->FormatElement( kfsetting->_item, __STRING__( item ), 1u, iter.first );
        }
    }
}
