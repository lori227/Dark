#ifndef __KF_RECRUIT_DIVISOR_CONFIG_H__
#define __KF_RECRUIT_DIVISOR_CONFIG_H__

#include "KFZConfig/KFConfig.h"

namespace KFrame
{
    class KFDivisorSetting : public KFIntSetting
    {
    public:
        // 因子类型
        uint32 _type = 0u;

        // 因子数值
        uint32 _value = 0u;

        // 成功率
        uint32 _probability = 0u;

        // 同一类型最大个数
        uint32 _max_count = 0u;
    };
    ////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////
    class KFDivisorConfig : public KFIntConfigT< KFDivisorSetting >, public KFInstance< KFDivisorConfig >
    {
    public:
        KFDivisorConfig()
        {
            _file_name = "divisor";
        }

    protected:
        // 读取配置
        virtual void ReadSetting( KFNode& xmlnode, KFDivisorSetting* kfsetting );
    };
}

#endif