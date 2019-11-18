#ifndef __KF_PVE_CONFIG_H__
#define __KF_PVE_CONFIG_H__

#include "KFZConfig/KFConfig.h"

namespace KFrame
{
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    class KFPVESetting : public KFIntSetting
    {
    public:
        // npc生成列表
        VectorUInt32 _npc_generate_list;

        // 成功道具掉落
        VectorUInt32 _victory_drop_list;

        // 失败道具掉落
        VectorUInt32 _fail_drop_list;
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

    protected:
        // 读取配置
        virtual void ReadSetting( KFNode& xmlnode, KFPVESetting* kfsetting );
    };
}

#endif