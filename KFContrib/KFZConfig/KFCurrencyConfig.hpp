#ifndef __KF_CURRENCY_CONFIG_H__
#define __KF_CURRENCY_CONFIG_H__

#include "KFZConfig/KFConfig.h"

namespace KFrame
{
    class KFCurrencySetting : public KFIntSetting
    {
    public:
        // 货币属性名
        std::string _name;
    };

    class KFCurrencyConfig : public KFConfigT< KFCurrencySetting >, public KFInstance< KFCurrencyConfig >
    {
    public:
        KFCurrencyConfig()
        {
            _file_name = "currency";
        }

    protected:
        // 读取配置
        virtual void ReadSetting( KFNode& xmlnode, KFCurrencySetting* kfsetting );
    };
}

#endif