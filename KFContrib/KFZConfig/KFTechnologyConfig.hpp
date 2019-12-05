﻿#ifndef __KF_TECHNOLOGY_CONFIG_H__
#define __KF_TECHNOLOGY_CONFIG_H__

#include "KFZConfig/KFConfig.h"
#include "KFZConfig/KFSetting.h"
#include "KFCore/KFElement.h"
#include "KFCore/KFCondition.h"
#include "KFExecuteData.h"
#include "KFZConfig/KFReadSetting.h"
#include "KFZConfig/KFRewardConfig.h"

namespace KFrame
{
    class KFTechnologySetting : public KFIntSetting
    {
    public:
        // 默认状态
        uint32 _status = 0u;

        // 建筑id
        uint32 _build_id = 0u;

        // 前置科技条件
        VectorUInt32 _pre_technology;

        // 后置开放的科技
        VectorUInt32 _unlock_technology;

        // 升级消耗
        std::string _str_consume;
        KFElements _consume;

        // 执行功能
        KFExecuteData _execute_data;
    };

    class KFTechnologyConfig : public KFConfigT< KFTechnologySetting >, public KFInstance< KFTechnologyConfig >
    {
    public:
        KFTechnologyConfig()
        {
            _file_name = "technology";
        }

        virtual void LoadAllComplete();

    protected:
        // 读取配置
        virtual void ReadSetting( KFNode& xmlnode, KFTechnologySetting* kfsetting );
    };
}

#endif