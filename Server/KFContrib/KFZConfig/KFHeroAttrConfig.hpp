#ifndef __KF_HERO_ATTR_CONFIG_H__
#define __KF_HERO_ATTR_CONFIG_H__

#include "KFZConfig/KFConfig.h"

namespace KFrame
{
    class KFAttributeData
    {
    public:
        uint32 _min_value = 0u;
        uint32 _max_value = 0u;

        uint32 GetValue() const;
    };

    class KFAttributeSetting : public KFIntSetting
    {
    public:
        KFHashMap< std::string, const std::string&, KFAttributeData > _datas;
    };
    ////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////
    class KFHeroAttributeConfig : public KFIntConfigT< KFAttributeSetting >, public KFInstance< KFHeroAttributeConfig >
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