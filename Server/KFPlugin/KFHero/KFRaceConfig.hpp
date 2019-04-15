#ifndef __KF_RACE_CONFIG_H__
#define __KF_RACE_CONFIG_H__

#include "KFConfig/KFConfigInterface.h"

namespace KFrame
{
    class KFRaceWeightData : public KFWeight
    {
    public:
        // 名字库id
        uint32 _name_id = 0u;
    };

    class KFRaceSetting
    {
    public:
        // 种族
        uint32 _race = 0u;

        // 种族权重
        uint32 _weight = 0u;

        // 身份列表
        KFWeightList< KFRaceWeightData > _identity_list;

        // 性别列表
        KFWeightList< KFRaceWeightData > _sex_list;

        // 年龄
        uint32 _min_age = 0u;
        uint32 _max_age = 0u;

        // 职业列表
        KFWeightList< KFWeight > _profession_list;

        // 种族属性点
        uint32 _attr_id = 0u;

        // 天赋技能数量
        uint32 _innate_skill_count;
        KFWeightList< KFWeight > _innate_list;
    };
    ////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////
    class KFRaceConfig : public KFConfig, public KFSingleton< KFRaceConfig >
    {
    public:
        KFRaceConfig()
        {
            _file = "race.config";
        }

        // 读取配置
        bool LoadConfig();

        // 随机名字
        const KFRaceSetting* RandRace() const;
        const KFRaceSetting* FindRaceSetting( uint32 race ) const;

    private:
        // 总权重
        uint32 _total_weight = 0u;

        // 品质列表
        KFHashMap< uint32, uint32, KFRaceSetting > _settings;
    };

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static auto _kf_race_config = KFRaceConfig::Instance();
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

#endif