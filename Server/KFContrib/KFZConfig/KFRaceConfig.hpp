#ifndef __KF_RACE_CONFIG_H__
#define __KF_RACE_CONFIG_H__

#include "KFZConfig/KFConfig.h"

namespace KFrame
{
    class KFRaceSetting : public KFIntSetting
    {
    public:
        // 姓( 字库id )
        uint32 _surname_id = 0u;

        // 名( 男性字库id )
        uint32 _male_firstname_id = 0u;

        // 名( 女性字库id )
        uint32 _female_firstname_id = 0u;

        // 英雄属性偏好率
        std::unordered_map< std::string, uint32 > _attribute;

    public:
        // 获得属性
        uint32 GetAttributeValue( const std::string& name ) const;
    };
    ////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////
    class KFRaceConfig : public KFIntConfigT< KFRaceSetting >, public KFInstance< KFRaceConfig >
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