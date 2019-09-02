#ifndef __KF_WEIGHT_CONFIG_H__
#define __KF_WEIGHT_CONFIG_H__

#include "KFZConfig/KFConfig.h"

namespace KFrame
{
    class KFWeightSetting : public KFIntSetting
    {
    public:
        // 权重列表
        KFWeightList< KFWeight > _weight_list;
    };
    /////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////
    class KFWeightConfig : public KFIntConfigT< KFWeightSetting >, public KFInstance< KFWeightConfig >
    {
    public:
        KFWeightConfig()
        {
            _file_name = "weight";
        }

        // 查找权重池
        const KFWeightList< KFWeight >* FindWeightPool( uint32 id );
    protected:
        // 读取配置
        virtual void ReadSetting( KFNode& xmlnode, KFWeightSetting* kfsetting );
    };
}

#endif