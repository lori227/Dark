#ifndef __KF_COMPOUND_RUNE_CONFIG_H__
#define __KF_COMPOUND_RUNE_CONFIG_H__

#include "KFCore/KFElement.h"
#include "KFZConfig/KFConfig.h"

namespace KFrame
{
    /////////////////////////////////////////////////////////////////////////////////
    class KFCompoundRuneSetting : public KFIntSetting
    {
    public:
        // 根据物品id得到合成等级
        uint32 GetLevelById( uint32 compoundid ) const;

        // 根据合成等级获得对应的物品id
        uint32 GetIdByLevel( uint32 level ) const;

    public:
        // 不同合成等级列表
        std::vector<uint32> _compound_ids;
    };

    ////////////////////////////////////////////////////////////////////////////////////
    class KFCompoundRuneConfig : public KFConfigT< KFCompoundRuneSetting >, public KFInstance< KFCompoundRuneConfig >
    {
    public:
        KFCompoundRuneConfig()
        {
            _file_name = "compoundrune";
        }

    protected:
        // 读取配置
        virtual void ReadSetting( KFNode& xmlnode, KFCompoundRuneSetting* kfsetting );
    };
}

#endif