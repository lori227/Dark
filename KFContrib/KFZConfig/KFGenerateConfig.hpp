#ifndef __KF_GENERATE_CONFIG_H__
#define __KF_GENERATE_CONFIG_H__

#include "KFZConfig/KFConfig.h"

namespace KFrame
{
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    class KFGenerateSetting : public KFIntSetting
    {
    public:
        // 种族
        uint32 _race_pool_id = 0u;

        // 性别
        uint32 _sex_pool_id = 0u;

        // 职业
        uint32 _profession_pool_id = 0u;

        // 背景
        uint32 _background_pool_id = 0u;

        // 性格
        UInt32Vector _character_pool_list;

        // 天赋
        UInt32Vector _innate_pool_list;

        // 主动技能
        UInt32Vector _active_pool_list;

        // 初始属性偏转率id
        uint32 _attr_id = 0u;

        // 品质池id
        uint32 _quality_pool_id = 0u;

        // 招募消耗花费公式
        UInt32Vector _formula_list;

        // 耐久度
        uint32 _durability = 0u;

        // 生成等级
        uint32 _level = 0u;

        // 订制英雄
        uint32 _custom = 0u;

        // 经验倍率(万分比)
        uint32 _exp_rate = 0u;
    };

    ////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////
    class KFGenerateConfig : public KFConfigT< KFGenerateSetting >, public KFInstance< KFGenerateConfig >
    {
    public:
        KFGenerateConfig()
        {
            _file_name = "generate";
        }

    protected:
        // 读取配置
        virtual void ReadSetting( KFNode& xmlnode, KFGenerateSetting* kfsetting );
    };
}

#endif