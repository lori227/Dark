#ifndef __KF_FOOD_CONSUME_CONFIG_H__
#define __KF_FOOD_CONSUME_CONFIG_H__

#include "KFZConfig/KFConfig.h"

namespace KFrame
{
    class KFFoodConsumeSetting : public KFIntSetting
    {
    public:
        // 步数
        uint32 _step_count = 0u;

        // 消耗粮食数量
        uint32 _food_num = 0u;

        // 消耗hp
        uint32 _hp_num = 0u;
    };
    ////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////
    class KFFoodConsumeConfig : public KFConfigT< KFFoodConsumeSetting >, public KFInstance< KFFoodConsumeConfig >
    {
    public:
        KFFoodConsumeConfig()
        {
            _file_name = "foodconsume";
        }
    protected:
        // 读取配置
        virtual void ReadSetting( KFNode& xmlnode, KFFoodConsumeSetting* kfsetting );
    };
}

#endif