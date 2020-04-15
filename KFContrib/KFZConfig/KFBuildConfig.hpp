#ifndef __KF_BUILD_CONFIG_H__
#define __KF_BUILD_CONFIG_H__

#include "KFZConfig/KFConfig.h"
#include "KFZConfig/KFSetting.h"
#include "KFCore/KFCondition.h"
#include "KFZConfig/KFElementConfig.h"

namespace KFrame
{
    class KFBuildLevelSetting
    {
    public:
        // 建筑等级
        uint32 _level = 0u;

        // 建筑条件
        uint32 _condition_type = 0u;
        UInt32Vector _condition;

        // 解锁科技
        UInt32Vector _technology;

        // 升级时间(s)
        uint64 _upgrade_time = 0u;

        // 单位时间(s)
        uint64 _unit_time = 0u;

        // 升级消耗
        std::string _str_consume;
        KFElements _consume;

        // 快速完成消耗
        std::string _str_onekey_consume;
        KFElements _onekey_consume;
    };

    class KFBuildSetting : public KFIntSetting
    {
    public:
        // 最大等级
        uint32 _max_level = 0u;

        // 等级列表
        KFHashMap< uint32, uint32, KFBuildLevelSetting > _build_level_settings;
    };

    class KFBuildConfig : public KFConfigT< KFBuildSetting >, public KFInstance< KFBuildConfig >
    {
    public:
        KFBuildConfig()
        {
            _key_name = "BuildId";
            _file_name = "buildlevel";
        }

        virtual void LoadAllComplete();

        // 获取建筑配置
        const KFBuildLevelSetting* FindBuildLevelSetting( uint32 id, uint32 level );

    protected:
        // 读取配置
        virtual void ReadSetting( KFNode& xmlnode, KFBuildSetting* kfsetting );
    };
}

#endif