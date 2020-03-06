#ifndef __KF_NPC_RAND_CONFIG_H__
#define __KF_NPC_RAND_CONFIG_H__

#include "KFZConfig/KFConfig.h"

namespace KFrame
{
    class KFNpcWeightData : public KFWeight
    {
    public:
        uint32 _spawn_rule = 0u;
    };
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    class KFNpcRandSetting : public KFIntSetting
    {
    public:
        // 权重列表
        KFWeightList< KFNpcWeightData > _npc_weight_list;
    };

    ////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////
    class KFNpcRandConfig : public KFConfigT< KFNpcRandSetting >, public KFInstance< KFNpcRandConfig >
    {
    public:
        KFNpcRandConfig()
        {
            _file_name = "npcrand";
        }

    protected:
        // 读取配置
        virtual void ReadSetting( KFNode& xmlnode, KFNpcRandSetting* kfsetting );
    };
}

#endif