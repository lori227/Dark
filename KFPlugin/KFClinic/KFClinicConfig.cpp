#include "KFClinicConfig.hpp"

namespace KFrame
{
    void KFClinicConfig::ReadSetting( KFNode& xmlnode, KFClinicSetting* kfsetting )
    {
        kfsetting->_item_id = xmlnode.GetUInt32( "Item", true );
        kfsetting->_max_num = xmlnode.GetUInt32( "MaxNum", true );
        kfsetting->_cd_time = xmlnode.GetUInt64( "CdTime", true );
        kfsetting->_add_num = xmlnode.GetUInt32( "AddNum", true );
        kfsetting->_consume_num = xmlnode.GetUInt32( "ConsumeNum", true );
        kfsetting->_add_hp = xmlnode.GetUInt32( "AddHp", true );
        kfsetting->_count = xmlnode.GetUInt32( "Count", true );

        std::string strelement = xmlnode.GetString( "Money", true );
        kfsetting->_money.Parse( strelement, __FUNC_LINE__ );
    }
}