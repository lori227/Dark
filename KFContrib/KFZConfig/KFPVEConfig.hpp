#ifndef __KF_PVE_CONFIG_H__
#define __KF_PVE_CONFIG_H__

#include "KFZConfig/KFConfig.h"
#include "KFZConfig/KFElementConfig.h"
#include "KFZConfig/KFNpcGroupConfig.hpp"

namespace KFrame
{
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    class KFPVESetting : public KFIntSetting
    {
    public:
        // npc生成列表
        uint32 _npc_group_id = 0;
        const KFNpcGroupSetting* _npc_group_setting = nullptr;

        // 成功道具掉落
        UInt32Vector _victory_drop_list;
        UInt32Vector _inner_victory_drop_list;

        // 失败道具掉落
        UInt32Vector _fail_drop_list;
        UInt32Vector _inner_fail_drop_list;

        // 进入消耗
        std::string _str_consume;
        KFElements _consume;

        // 逃跑成功率
        uint32 _normal_flee_rate = 0u;
        uint32 _inner_flee_rate = 0u;

        // 等级成功率修正
        uint32 _level_coefficient;

        // 逃跑失败惩罚id
        uint32 _punish_id = 0u;
    };

    ////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////
    class KFPVEConfig : public KFConfigT< KFPVESetting >, public KFInstance< KFPVEConfig >
    {
    public:
        KFPVEConfig()
        {
            _file_name = "pve";
        }

        virtual void LoadAllComplete();

    protected:
        // 读取配置
        virtual void ReadSetting( KFNode& xmlnode, KFPVESetting* kfsetting );
    };
}

#endif