#include "KFSmithyWeaponConfig.hpp"

namespace KFrame
{
    void KFSmithyWeaponConfig::ReadSetting( KFNode& xmlnode, KFSmithyWeaponSetting* kfsetting )
    {
        kfsetting->_need_num = xmlnode.GetUInt64( "NeedNum", true );
        kfsetting->_str_consume = xmlnode.GetString( "Consume", true );
    }

    void KFSmithyWeaponConfig::LoadAllComplete()
    {
        for ( auto& iter : _settings._objects )
        {
            auto kfsetting = iter.second;
            KFElementConfig::Instance()->FormatElemnt( kfsetting->_item, __STRING__( item ), "1", iter.first );
            KFElementConfig::Instance()->ParseElement( kfsetting->_consume, kfsetting->_str_consume, __FILE__, kfsetting->_id );
        }
    }
}
