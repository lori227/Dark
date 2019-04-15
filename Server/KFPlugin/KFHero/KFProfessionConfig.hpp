#ifndef __KF_PROFESSION_CONFIG_H__
#define __KF_PROFESSION_CONFIG_H__

#include "KFConfig/KFConfigInterface.h"

namespace KFrame
{
    class KFProfessionWeightData
    {
    public:
        // id
        uint32 _id = 0u;

        // 权重
        uint32 _weight = 0u;
    };

    class KFProfessionSetting
    {
    public:
        // 职业
        uint32 _profession = 0u;

        // 成长属性的id
        uint32 _attr_id = 0u;

        // 主动技能个数
        uint32 _active_count = 2u;
        KFWeightList< KFWeight > _active_skills;

        // 被动技能
        uint32 _passivity_count = 1u;
        KFWeightList< KFWeight > _passivity_skills;

        // 武器列表
        KFWeightList< KFWeight > _weapons;
        ////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////
    };
    ////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////
    class KFProfessionConfig : public KFConfig, public KFSingleton< KFProfessionConfig >
    {
    public:
        KFProfessionConfig()
        {
            _file = "profession.config";
        }

        // 读取配置
        bool LoadConfig();

        // 查找职业设定
        const KFProfessionSetting* FindProfessionSetting( uint32 profession ) const;

    private:
        // 职业列表
        KFHashMap< uint32, uint32, KFProfessionSetting > _settings;
    };

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static auto _kf_profession_config = KFProfessionConfig::Instance();
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

#endif