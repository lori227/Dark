#ifndef __KF_RACE_CONFIG_H__
#define __KF_RACE_CONFIG_H__

#include "KFZConfig/KFConfig.h"

namespace KFrame
{
    class KFRaceSetting : public KFIntSetting
    {
    public:
        // 性别
        uint32 _sex = 0u;

        // 英雄属性偏好率
        StringUInt32 _attribute;

    public:
        // 获得属性
        uint32 GetAttributeValue( const std::string& name ) const;
    };
    ////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////
    class KFRaceConfig : public KFConfigT< KFRaceSetting >, public KFInstance< KFRaceConfig >
    {
    public:
        KFRaceConfig()
        {
            _file_name = "race";
        }

    protected:
        // 读取配置
        virtual void ReadSetting( KFNode& xmlnode, KFRaceSetting* kfsetting );
    };
}

#endif