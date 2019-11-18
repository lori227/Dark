#include "KFSmithyConfig.hpp"

namespace KFrame
{
    void KFSmithyConfig::ReadSetting( KFNode& xmlnode, KFSmithySetting* kfsetting )
    {
        kfsetting->_item_id = xmlnode.GetUInt32( "ItemId", true );
        kfsetting->_cd_time = xmlnode.GetUInt64( "CdTime", true );
        kfsetting->_add_num = xmlnode.GetUInt32( "AddNum", true );
        kfsetting->_collect_max = xmlnode.GetUInt32( "CollectMax", true );
        kfsetting->_store_max = xmlnode.GetUInt32( "StoreMax", true );
        kfsetting->_make_max = xmlnode.GetUInt32( "MakeMax", true );
    }
}