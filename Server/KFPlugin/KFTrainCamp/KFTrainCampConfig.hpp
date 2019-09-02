#ifndef __KF_TRAIN_CAMP_CONFIG_H__
#define __KF_TRAIN_CAMP_CONFIG_H__

#include "KFZConfig/KFConfig.h"

namespace KFrame
{
    class KFTrainCampSetting : public KFIntSetting
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
    };

    class KFTrainCampConfig : public KFIntConfigT< KFTrainCampSetting >, public KFInstance< KFTrainCampConfig >
    {
    public:
        KFTrainCampConfig()
        {
            _file_name = "traincamp";
        }

    protected:
        // 读取配置
        virtual void ReadSetting( KFNode& xmlnode, KFTrainCampSetting* kfsetting );
    };
}

#endif