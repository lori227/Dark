#include "KFSmithyWeaponConfig.hpp"

namespace KFrame
{
    void KFSmithyWeaponConfig::ReadSetting( KFNode& xmlnode, KFSmithyWeaponSetting* kfsetting )
    {
        kfsetting->_need_num = xmlnode.GetUInt64( "NeedNum", true );

        kfsetting->_consume_str = xmlnode.GetString( "Consume", true );
    }

    void KFSmithyWeaponConfig::LoadAllComplete()
    {
        std::string item_str = "item";
        for ( auto& iter : _settings._objects )
        {
            auto kfsetting = iter.second;
            auto kfelementsetting = KFElementConfig::Instance()->FindElementSetting( item_str );
            auto strelement = __FORMAT__( kfelementsetting->_element_template, item_str, 1u, iter.first );

            kfsetting->_item.Parse( strelement, __FUNC_LINE__ );

            if ( !kfsetting->_consume_str.empty() )
            {
                if ( !KFRewardConfig::Instance()->ParseRewards( kfsetting->_consume_str, kfsetting->_consume ) )
                {
                    __LOG_ERROR__( "id=[{}] smithyweapon config consume is error", kfsetting->_id );
                }
            }
        }
    }
}
