#ifndef __KF_CLINIC_CONFIG_H__
#define __KF_CLINIC_CONFIG_H__

#include "KFZConfig/KFConfig.h"
#include "KFCore/KFElement.h"

namespace KFrame
{
    class KFClinicSetting : public KFIntSetting
    {
    public:
        // 物品id
        uint32 _item_id = 0u;

        // 物品上限
        uint32 _max_num = 0u;

        // 间隔时间
        uint64 _cd_time = 0u;

        // 增加数量
        uint32 _add_num = 0u;

        // HP消耗物品数量
        uint32 _consume_num = 0u;

        // 回复hp数量
        uint32 _add_hp = 0u;

        // 栏位数量
        uint32 _count = 0u;

        // 金钱消耗
        std::string _strmoney;
        KFElements _money;

        // 公式id
        uint32 _formual_id = 0u;
    };

    class KFClinicConfig : public KFConfigT< KFClinicSetting >, public KFInstance< KFClinicConfig >
    {
    public:
        KFClinicConfig()
        {
            _file_name = "clinic";
        }

    protected:
        // 读取配置
        void ReadSetting( KFNode& xmlnode, KFClinicSetting* kfsetting );
    };
}

#endif