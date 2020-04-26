#ifndef __KF_PROFESSION_CONFIG_H__
#define __KF_PROFESSION_CONFIG_H__

#include "KFCore/KFElement.h"
#include "KFZConfig/KFConfig.h"

namespace KFrame
{
    class KFProfessionSetting : public KFIntSetting
    {
    public:
        // 种族限制
        UInt32Set _race_list;

        // 性别限制
        uint32 _sex_limit = 0u;

        // 移动方式
        uint32 _move_type = 0u;

        // 武器类型
        UInt32Set _weapon_type_list;

        // 英雄属性偏好率
        StringUInt32 _attribute;

        // 最大等级
        uint32 _max_level = 0u;

        // 职业等级
        uint32 _class_lv = 0u;

        // 耐久度
        KFRange<uint32> _durability_range;

        // 音色类型
        UInt32Vector _voice_list;

    public:
        // 是否有效( 被限制 )
        bool IsValid( uint32 race, uint32 sex, uint32 movetype, uint32 weapontype ) const;

        // 获得属性
        uint32 GetAttributeValue( const std::string& name ) const;

        // 武器类型是否有效
        bool IsWeaponTypeValid( uint32 weapontype ) const;

        // 随机武器类型
        uint32 RandWeapontype() const;

        // 随机耐久度
        uint32 RandRoleDurability() const;
    };
    ////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////
    class KFProfessionConfig : public KFConfigT< KFProfessionSetting >, public KFInstance< KFProfessionConfig >
    {
    public:
        KFProfessionConfig()
        {
            _file_name = "profession";
        }

    protected:
        // 读取配置
        virtual void ReadSetting( KFNode& xmlnode, KFProfessionSetting* kfsetting );
    };
}

#endif