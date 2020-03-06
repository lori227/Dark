#ifndef __KF_NPC_GROUP_CONFIG_H__
#define __KF_NPC_GROUP_CONFIG_H__

#include "KFZConfig/KFConfig.h"

namespace KFrame
{
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    class KFNpcGroupSetting : public KFIntSetting
    {
    public:
        // npc生成列表
        UInt32Vector _npc_generate_list;

        // 显示模型
        std::string _show_model;
    };

    ////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////
    class KFNpcGroupConfig : public KFConfigT< KFNpcGroupSetting >, public KFInstance< KFNpcGroupConfig >
    {
    public:
        KFNpcGroupConfig()
        {
            _file_name = "npcgroup";
        }

    protected:
        // 读取配置
        virtual void ReadSetting( KFNode& xmlnode, KFNpcGroupSetting* kfsetting );
    };
}

#endif