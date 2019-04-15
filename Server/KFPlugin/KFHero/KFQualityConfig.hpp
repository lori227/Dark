#ifndef __KF_QUALITY_CONFIG_H__
#define __KF_QUALITY_CONFIG_H__

#include "KFConfig/KFConfigInterface.h"

namespace KFrame
{
    class KFQualitySetting
    {
    public:
        // 品质
        uint32 _quality = 0u;

        // 权重
        uint32 _weight = 0u;

        // 基础属性点数
        uint32 _basic_point = 0u;

        // 成长属性点数
        uint32 _growup_point = 0u;
    };
    ////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////
    class KFQualityConfig : public KFConfig, public KFSingleton< KFQualityConfig >
    {
    public:
        KFQualityConfig()
        {
            _file = "quality.config";
        }

        // 读取配置
        bool LoadConfig();

        // 随机名字
        const KFQualitySetting* RandQuality() const;
        const KFQualitySetting* FindQuality( uint32 quality ) const;

    private:
        // 总权重
        uint32 _total_weight = 0u;

        // 品质列表
        KFHashMap< uint32, uint32, KFQualitySetting > _quality_setting;
    };

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static auto _kf_quality_config = KFQualityConfig::Instance();
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

#endif