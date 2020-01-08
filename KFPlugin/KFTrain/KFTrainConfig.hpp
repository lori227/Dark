#ifndef __KF_TRAIN_CONFIG_H__
#define __KF_TRAIN_CONFIG_H__

#include "KFZConfig/KFConfig.h"
#include "KFZConfig/KFSetting.h"
#include "KFZConfig/KFElementConfig.h"

namespace KFrame
{
    class KFTrainSetting : public KFIntSetting
    {
    public:
        // 训练所等级
        uint32 _level = 0u;

        // 栏位数量
        uint32 _count = 0u;

        // 总时间(s)
        uint64 _total_time = 0u;

        // 间隔时间(s)
        uint64 _cd_time = 0u;

        // 增加经验
        uint32 _add_exp = 0u;

        // 金钱消耗
        std::string _str_consume;
        KFElements _consume;

        // 一键完成消耗
        std::string _str_onekey_consume;
        KFElements _onekey_consume;

        // 一键完成单位时间
        uint32 _unit_time;
    };

    class KFTrainConfig : public KFConfigT< KFTrainSetting >, public KFInstance< KFTrainConfig >
    {
    public:
        KFTrainConfig()
        {
            _file_name = "train";
        }

        virtual void LoadAllComplete();

    protected:
        // 读取配置
        virtual void ReadSetting( KFNode& xmlnode, KFTrainSetting* kfsetting );
    };
}

#endif