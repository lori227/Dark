#ifndef __KF_BUILD_LEVEL_CONFIG_H__
#define __KF_BUILD_LEVEL_CONFIG_H__

#include "KFZConfig/KFConfig.h"

namespace KFrame
{
    class KFBuildLevelSetting : public KFIntSetting
    {
    public:
        // 解锁等级
        uint32 _unlock_level = 0u;
    };

    class KFBuildLevelConfig : public KFIntConfigT< KFBuildLevelSetting >, public KFInstance< KFBuildLevelConfig >
    {
    public:
        KFBuildLevelConfig()
        {
            _file_name = "buildlevel";
        }

        // 获取建筑等级
        uint32 GetBuildUnlockLevel( uint32 id, uint32 level );

    protected:

        // 读取配置
        virtual void ReadSetting( KFNode& xmlnode, KFBuildLevelSetting* kfsetting );
    };
}

#endif