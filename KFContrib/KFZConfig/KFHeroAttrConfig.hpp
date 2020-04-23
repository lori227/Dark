#ifndef __KF_HERO_ATTR_CONFIG_H__
#define __KF_HERO_ATTR_CONFIG_H__

#include "KFZConfig/KFConfig.h"

namespace KFrame
{
    class KFAttributeData
    {
    public:
        KFRange<uint32> _range;

    public:
        uint32 GetValue() const;
    };

    class KFAttributeSetting : public KFIntSetting
    {
    public:
        KFHashMap< std::string, const std::string&, KFAttributeData > _datas;
    };
    ////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////
    class KFHeroAttributeConfig : public KFConfigT< KFAttributeSetting >, public KFInstance< KFHeroAttributeConfig >
    {
    public:
        KFHeroAttributeConfig()
        {
            _file_name = "heroattribute";
        }

    private:
        // 读取配置
        virtual void ReadSetting( KFNode& xmlnode, KFAttributeSetting* kfsetting );
    };
}

#endif