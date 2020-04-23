#ifndef __KF_PUNISH_CONFIG_H__
#define __KF_PUNISH_CONFIG_H__

#include "KFZConfig/KFConfig.h"

namespace KFrame
{
    class KFPunishData
    {
    public:
        // 权重值
        KFRange<uint32> _range_weight;

        // 类型
        std::string _name;
        uint32 _key = 0;

        // 数值
        KFRange<uint32> _range_value;
    };

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    class KFPunishSetting : public KFIntSetting
    {
    public:
        // 随机数量
        KFRange<uint32> _range_count;

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