#ifndef __KF_BUILD_SKIN_CONFIG_H__
#define __KF_BUILD_SKIN_CONFIG_H__

#include "KFZConfig/KFConfig.h"

namespace KFrame
{
    class KFBuildSkinSetting : public KFIntSetting
    {
    };

    class KFBuildSkinConfig : public KFConfigT< KFBuildSkinSetting >, public KFInstance< KFBuildSkinConfig >
    {
    public:
        KFBuildSkinConfig()
        {
            _file_name = "buildskin";
        }

    protected:

        // 读取配置
        virtual void ReadSetting( KFNode& xmlnode, KFBuildSkinSetting* kfsetting );
    };
}

#endif