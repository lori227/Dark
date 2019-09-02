﻿#ifndef __KF_GENERATE_CONFIG_H__
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
        std::list< uint32 > _character_pool_list;

        // 天赋
        std::list< uint32 > _innate_pool_list;

        // 主动技能
        std::list< uint32 > _active_pool_list;

        // 初始属性偏转率id
        uint32 _attr_id = 0u;

        // 成长率id
        uint32 _growth_id = 0u;
    };

    ////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////
    class KFGenerateConfig : public KFIntConfigT< KFGenerateSetting >, public KFInstance< KFGenerateConfig >
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