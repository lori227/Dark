#ifndef __KF_WEATHER_CONFIG_H__
#define __KF_WEATHER_CONFIG_H__

#include "KFZConfig/KFConfig.h"

namespace KFrame
{
    class KFWeatherData
    {
    public:
        // 天气数据
        uint32 _weahter_id = 0u;

        // 探索最小持续时间
        uint32 _min_realm_duration = 0u;

        // 探索最大持续时间
        uint32 _max_realm_duration = 0u;

        // 战斗最小持续时间
        uint32 _min_pve_duration = 0u;

        // 战斗最大持续时间
        uint32 _max_pve_duration = 0u;

        // 权重值
        uint32 _rand_weight = 0u;
    };

    class KFWeatherSetting : public KFIntSetting
    {
    public:
        // 天气数据
        std::vector< KFWeatherData > _weather_data_list;

        // 总共权重
        uint32 _total_rand_weight = 0u;

    public:
        // 随机天气
        const KFWeatherData* RandWeatherData() const;

        // 获得当前天气的数据
        const KFWeatherData* GetCurrentWeatherData( uint32 weatherid ) const;
    };

    ////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////
    class KFWeatherConfig : public KFConfigT< KFWeatherSetting >, public KFInstance< KFWeatherConfig >
    {
    public:
        KFWeatherConfig()
        {
            _file_name = "weather";
        }

    protected:
        // 读取配置
        virtual void ReadSetting( KFNode& xmlnode, KFWeatherSetting* kfsetting );
    };
}

#endif