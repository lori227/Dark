#ifndef __KF_PVE_CONFIG_H__
#define __KF_PVE_CONFIG_H__

#include "KFZConfig/KFConfig.h"
#include "KFCore/KFElement.h"
#include "KFZConfig/KFRewardConfig.h"

namespace KFrame
{
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    class KFPVESetting : public KFIntSetting
    {
    public:
        // npc生成列表
        UInt32Vector _npc_generate_list;

        // 成功道具掉落
        UInt32Vector _victory_drop_list;

        // 失败道具掉落
        UInt32Vector _fail_drop_list;

        // 进入消耗
        std::string _consume_str;
        KFElements _consume;
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