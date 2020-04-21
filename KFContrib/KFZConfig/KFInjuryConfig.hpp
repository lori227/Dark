#ifndef __KF_INJURY_CONFIG_H__
#define __KF_INJURY_CONFIG_H__

#include "KFZConfig/KFConfig.h"

namespace KFrame
{
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    class KFInjurySetting : public KFIntSetting
    {
    public:
        bool IsValid() const
        {
            return true;
        }

        // 最大等级
        uint32 _max_level = 0u;
    };

    ////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////
    class KFInjuryConfig : public KFConfigT< KFInjurySetting >, public KFInstance< KFInjuryConfig >
    {
    public:
        KFInjuryConfig()
        {
            _file_name = "injury";
            _key_name = "Type";
        }

    protected:
        // 读取配置
        virtual void ReadSetting( KFNode& xmlnode, KFInjurySetting* kfsetting );
    };
}

#endif