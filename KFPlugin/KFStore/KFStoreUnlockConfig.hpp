#ifndef __KF_STORE_UNLOCK_CONFIG_H__
#define __KF_STORE_UNLOCK_CONFIG_H__

#include "KFZConfig/KFConfig.h"
#include "KFZConfig/KFReadSetting.h"

namespace KFrame
{
    ///////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////
    // 商店解锁配置
    class KFStoreUnlockSetting : public KFIntSetting
    {
    public:
        // 索引位置
        uint32 _index = 0u;

        // 解锁序列( 高级覆盖低级 )
        uint32 _sort = 0u;

        // 商店id
        uint32 _store_id = 0u;

        // 解锁条件
        uint32 _condition_type = 0u;
        UInt32Vector _unlock_condition;
    };

    class KFStoreUnlockConfig : public KFConfigT< KFStoreUnlockSetting >, public KFInstance< KFStoreUnlockConfig >
    {
    public:
        KFStoreUnlockConfig()
        {
            _file_name = "storeunlock";
        }

    protected:
        // 读取配置
        virtual void ReadSetting( KFNode& xmlnode, KFStoreUnlockSetting* kfsetting );
    };
}

#endif