#ifndef __KF_NPC_SKILL_CONFIG_H__
#define __KF_NPC_SKILL_CONFIG_H__

#include "KFZConfig/KFConfig.h"

namespace KFrame
{
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    class KFNpcSkillSetting : public KFIntSetting
    {
    public:
        // 主动技能权重池列表
        VectorUInt32 _active_pool_list;

        // 天赋技能权重池列表
        VectorUInt32 _innate_pool_list;
    };

    ////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////
    class KFNpcSkillConfig : public KFConfigT< KFNpcSkillSetting >, public KFInstance< KFNpcSkillConfig >
    {
    public:
        KFNpcSkillConfig()
        {
            _file_name = "npcskill";
        }

    protected:
        // 读取配置
        virtual void ReadSetting( KFNode& xmlnode, KFNpcSkillSetting* kfsetting );
    };
}

#endif