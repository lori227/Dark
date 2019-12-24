#ifndef __KF_INNATE_CONFIG_H__
#define __KF_INNATE_CONFIG_H__

#include "KFZConfig/KFConfig.h"

namespace KFrame
{
    class KFSkillSetting : public KFIntSetting
    {
    public:
        // 种族限制
        UInt32Set _race_list;

        // 职业限定
        UInt32Set _profession_list;

        // 武器限定
        UInt32Set _weapon_type_list;

        // 背景限制
        UInt32Set _background_list;

        // 是否有效( 被限制 )
        bool IsValid( uint32 race, uint32 profession, uint32 background, uint32 weapontype ) const;
    };
    ////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////
    class KFSkillConfig : public KFConfigT< KFSkillSetting >, public KFInstance< KFSkillConfig >
    {
    public:
        KFSkillConfig()
        {
            _file_name = "skill";
        }

    protected:
        // 读取配置
        virtual void ReadSetting( KFNode& xmlnode, KFSkillSetting* kfsetting );
    };
}

#endif