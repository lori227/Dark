#ifndef __KF_FACTION_CONFIG_H__
#define __KF_FACTION_CONFIG_H__

#include "KFZConfig/KFConfig.h"
#include "KFZConfig/KFSetting.h"

namespace KFrame
{
    class KFFactionSetting : public KFIntSetting
    {
    public:

    };

    class KFFactionConfig : public KFConfigT< KFFactionSetting >, public KFInstance< KFFactionConfig >
    {
    public:
        KFFactionConfig()
        {
            _file_name = "faction";
        }

        KFFactionSetting* GetDedaultFactionSetting()
        {
            return _dedaultfaction;
        }

    protected:
        // 读取配置
        virtual void ReadSetting( KFNode& xmlnode, KFFactionSetting* kfsetting )
        {
            if ( _dedaultfaction == nullptr )
            {
                _dedaultfaction = kfsetting;
            }
        }

    protected:
        // 默认势力id
        KFFactionSetting* _dedaultfaction;
    };
}

#endif