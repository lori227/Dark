#ifndef __KF_LEVEL_CONFIG_H__
#define __KF_LEVEL_CONFIG_H__

#include "KFZConfig/KFConfig.h"

namespace KFrame
{
    class KFLevelSetting : public KFIntSetting
    {
    public:
        // 总经验
        uint32 _exp = 0u;

        // 属性下限偏移
        uint32 _floor_attribute = 0u;

        // 属性上限偏移
        uint32 _upper_attribute = 0u;
    };

    class KFLevelConfig : public KFConfigT< KFLevelSetting >, public KFInstance< KFLevelConfig >
    {
    public:
        KFLevelConfig()
        {
            _file_name = "herolevel";
        }

        uint32 GetTotalExp()
        {
            return _total_exp;
        }

        uint32 GetLevelByExp( uint32 exp, uint32 level );

    protected:
        // 清空配置
        virtual void ClearSetting();

        // 读取配置
        virtual void ReadSetting( KFNode& xmlnode, KFLevelSetting* kfsetting );

    private:
        // 总经验
        uint32 _total_exp = 0u;

        // 经验表(经验 等级)
        std::map<uint32, uint32> _exp_map;
    };
}

#endif