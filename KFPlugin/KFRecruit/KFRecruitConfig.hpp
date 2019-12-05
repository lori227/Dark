#ifndef __KF_RECRUIT_CONFIG_H__
#define __KF_RECRUIT_CONFIG_H__

#include "KFCore/KFElement.h"
#include "KFZConfig/KFConfig.h"
#include "KFZConfig/KFRewardConfig.h"

namespace KFrame
{
    class KFRecruitSetting : public KFIntSetting
    {
    public:
        // 是否清空
        bool _is_clear = false;

        // 生成池的科技id
        uint32 _generate_technology_id = 0u;

        // 刷新花费
        std::string _str_cost;
        KFElements _cost_elements;
    };
    ////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////
    class KFRecruitConfig : public KFConfigT< KFRecruitSetting >, public KFInstance< KFRecruitConfig >
    {
    public:
        KFRecruitConfig()
        {
            _file_name = "recruit";
        }

        virtual void LoadAllComplete();

    protected:
        // 读取配置
        virtual void ReadSetting( KFNode& xmlnode, KFRecruitSetting* kfsetting );
    };
}

#endif