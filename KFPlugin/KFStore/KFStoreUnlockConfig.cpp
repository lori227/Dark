#include "KFStoreUnlockConfig.hpp"

namespace KFrame
{
    void KFStoreUnlockConfig::ReadSetting( KFNode& xmlnode, KFStoreUnlockSetting* kfsetting )
    {
        kfsetting->_index = xmlnode.GetUInt32( "Index" );
        kfsetting->_sort = xmlnode.GetUInt32( "Sort" );
        kfsetting->_store_id = xmlnode.GetUInt32( "StoreId" );

        auto strunlockcondition = xmlnode.GetString( "UnLockCondition" );
        kfsetting->_condition_type = KFReadSetting::ParseConditionList( strunlockcondition, kfsetting->_unlock_condition );
    }
}