#ifndef __KF_PUNISH_CONFIG_H__
#define __KF_PUNISH_CONFIG_H__

#include "KFZConfig/KFConfig.h"

namespace KFrame
{
    class KFPunishData
    {
    public:
        // 权重值
        uint32 _min_weight = 0u;
        uint32 _max_weight = 0u;

        // 类型
        std::string _name;
        uint32 _key = 0;

        // 数值
        uint32 _min_value = 0u;
        uint32 _max_value = 0u;
    };

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    class KFPunishSetting : public KFIntSetting
    {
    public:
        // 随机数量
        uint32 _min_count = 0u;
        uint32 _max_count = 0u;

        // 列表
        std::vector< KFPunishData > _punish_list;
    };

    ////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////
    class KFPunishConfig : public KFConfigT< KFPunishSetting >, public KFInstance< KFPunishConfig >
    {
    public:
        KFPunishConfig()
        {
            _file_name = "punish";
        }

    protected:
        // 读取配置
        virtual void ReadSetting( KFNode& xmlnode, KFPunishSetting* kfsetting );
    };
}

#endif