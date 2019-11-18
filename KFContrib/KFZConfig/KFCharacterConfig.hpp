#ifndef __KF_CHARACTER_CONFIG_H__
#define __KF_CHARACTER_CONFIG_H__

#include "KFZConfig/KFConfig.h"

namespace KFrame
{
    class KFCharacterSetting : public KFIntSetting
    {
    public:
        // 种族限制
        SetUInt32 _race_list;

        // 背景限制
        SetUInt32 _background_list;

        // 职业限制
        SetUInt32 _profession_list;

        // 是否有效( 被限制 )
        bool IsValid( uint32 race, uint32 background, uint32 profession ) const;
    };
    ////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////
    class KFCharacterConfig : public KFConfigT< KFCharacterSetting >, public KFInstance< KFCharacterConfig >
    {
    public:
        KFCharacterConfig()
        {
            _file_name = "character";
        }

    protected:
        // 读取配置
        virtual void ReadSetting( KFNode& xmlnode, KFCharacterSetting* kfsetting );
    };
}

#endif