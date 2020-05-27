#ifndef __KF_ITEM_CONFIG_H__
#define __KF_ITEM_CONFIG_H__

#include "KFItemSetting.hpp"
#include "KFZConfig/KFConfig.h"
#include "KFZConfig/KFElementConfig.h"

namespace KFrame
{
    ////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////
    class KFItemConfig : public KFConfigT< KFItemSetting >, public KFInstance< KFItemConfig >
    {
    public:
        KFItemConfig()
        {
            _file_name = "item";
        }
    protected:
        // 清空配置
        virtual void ClearSetting();

        // 读取配置
        virtual void ReadSetting( KFNode& xmlnode, KFItemSetting* kfsetting );

        // 加载完成
        virtual void LoadComplete();

        // 全部加载完成
        virtual void LoadAllComplete();

        // 读取通用道具
        void ReadCommonSetting( KFNode& xmlnode, KFItemSetting* kfsetting );

        // 读取礼包配置
        void ReadGiftSetting( KFNode& xmlnode, KFItemSetting* kfsetting );

        // 读取药品配置
        void ReadDrugSetting( KFNode& xmlnode, KFItemSetting* kfsetting );

        // 读取武器配置
        void ReadWeaponSetting( KFNode& xmlnode, KFItemSetting* kfsetting );

        // 读取材料配置
        void ReadMaterialSetting( KFNode& xmlnode, KFItemSetting* kfsetting );

        // 读取其他配置
        void ReadOtherSetting( KFNode& xmlnode, KFItemSetting* kfsetting );

        // 读取粮食道具
        void ReadFoodSetting( KFNode& xmlnode, KFItemSetting* kfsetting );

        // 读取符石道具
        void ReadRuneSetting( KFNode& xmlnode, KFItemSetting* kfsetting );

    public:
        // 获取合成符石id
        uint32 GetRuneCompoundId( uint32 id );

    private:
        // 符石类型等级表(type*10000+level, itemid)
        UInt32Map _rune_type_level;

        // 符石合成表(低级 高级)
        UInt32Map _rune_compound;

    };
}

#endif