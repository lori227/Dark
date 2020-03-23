#ifndef __KF_QUALITY_CONFIG_H__
#define __KF_QUALITY_CONFIG_H__

#include "KFZConfig/KFConfig.h"

namespace KFrame
{
    class KFQualitySetting : public KFIntSetting
    {
    public:
        // 成长率上限
        uint32 _max_growth = 0u;
    };

    class KFQualityConfig : public KFConfigT< KFQualitySetting >, public KFInstance< KFQualityConfig >
    {
    public:
        KFQualityConfig()
        {
            _file_name = "heroquality";
        }

        // 通过成长率获取品质
        uint32 GetQualityByGrowth( uint32 growth );

    protected:
        // 清空配置
        virtual void ClearSetting();

        // 读取配置
        virtual void ReadSetting( KFNode& xmlnode, KFQualitySetting* kfsetting );

    private:
        // 最大成长率
        uint32 _max_growth = 0u;

        // 品质表(成长率 品质)
        std::map<uint32, uint32> _quality_map;
    };
}

#endif