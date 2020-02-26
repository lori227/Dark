#ifndef __KF_EXCHANGE_CONFIG_H__
#define __KF_EXCHANGE_CONFIG_H__

#include "KFZConfig/KFConfig.h"

namespace KFrame
{
    class KFExchangeData
    {
    public:
        // 掉落
        uint32 _normal_drop_id = 0u;
        uint32 _inner_drop_id = 0u;
    };

    class KFExchangeSetting : public KFIntSetting
    {
    public:
        // 交换数据
        KFHashMap<uint32, uint32, KFExchangeData> _exchange_list;
    };

    ////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////
    class KFExchangeConfig : public KFConfigT< KFExchangeSetting >, public KFInstance< KFExchangeConfig >
    {
    public:
        KFExchangeConfig()
        {
            _file_name = "exchange";
        }

    protected:
        // 读取配置
        virtual void ReadSetting( KFNode& xmlnode, KFExchangeSetting* kfsetting );
    };
}

#endif