#ifndef __KF_REALM_TIME_CONFIG_H__
#define __KF_REALM_TIME_CONFIG_H__

#include "KFZConfig/KFConfig.h"

namespace KFrame
{
    class KFRealmTimeData
    {
    public:
        // 时间类型
        uint32 _time_type = 0u;

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

    class KFRealmTimeSetting : public KFIntSetting
    {
    public:
        // 时间数据
        std::vector< KFRealmTimeData > _time_data_list;

        // 总共权重
        uint32 _total_rand_weight = 0u;

    public:
        // 随机时间
        const KFRealmTimeData* RandRealmTimeData() const;

        // 获得本次时间
        const KFRealmTimeData* GetCurrentRealmTimeData( uint32 timetype ) const;

        // 获得下次时间
        const KFRealmTimeData* GetNextRealmTimeData( uint32 timetype ) const;
    };

    ////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////
    class KFRealmTimeConfig : public KFConfigT< KFRealmTimeSetting >, public KFInstance< KFRealmTimeConfig >
    {
    public:
        KFRealmTimeConfig()
        {
            _file_name = "realmtime";
        }

    protected:
        // 读取配置
        virtual void ReadSetting( KFNode& xmlnode, KFRealmTimeSetting* kfsetting );
    };
}

#endif