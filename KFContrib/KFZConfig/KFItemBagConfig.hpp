#ifndef __KF_ITEM_BAG_CONFIG_H__
#define __KF_ITEM_BAG_CONFIG_H__

#include "KFZConfig/KFConfig.h"

namespace KFrame
{
    enum KFItemBagEnum
    {
        RealmClean = 1,		// 探索清空
        RealmBalance = 2,	// 探索结算
    };

    class KFItemBagSetting : public KFStrSetting
    {
    public:
        // 添加是否需要显示飘字
        bool _is_add_show = false;

        // 移动需要显示飘字的背包
        StringSet _move_show_bag;

        // 进入探索的处理类型
        uint32 _enter_realm_type = 0u;
        // 结算探索的处理类型
        uint32 _balance_realm_type = 0u;
        // 回城探索的处理类型
        uint32 _town_realm_type = 0u;

        // 是否支持直接移动
        bool _is_can_move = false;

        // 是否支持一键拾取
        bool _is_can_move_all = false;

        // 是否支持一键清空
        bool _is_can_clean = false;

        // 来自源背包的移动道具需要更新处理
        StringSet _move_add_update_bag;

        // 移动到目标背包的道具是否需要删除更新
        StringSet _move_dec_update_bag;

        // 探索中是否需要纪录
        bool _is_realm_record = false;

        // 子页签列表
        StringSet _tab_list;

        // 是否自动使用礼包
        bool _is_auto_use_gift = false;

        // 额外的背包名字
        std::string _extend_bag_name;

        // 使用的子背包
        std::string _use_child_bag_name;

    public:
        // 判断是否移动显示更新
        bool IsMoveShow( const std::string& name ) const
        {
            return _move_show_bag.find( name ) != _move_show_bag.end();
        }

        // 判断是否需要添加更新
        bool IsMoveAddUpdate( const std::string& name ) const
        {
            return _move_add_update_bag.find( name ) != _move_add_update_bag.end();
        }

        // 判断是否需要删除更新
        bool IsMoveDecUpdate( const std::string& name ) const
        {
            return _move_dec_update_bag.find( name ) != _move_dec_update_bag.end();
        }

        // 是否有子页签
        bool IsHaveTab( const std::string& name ) const
        {
            return _tab_list.find( name ) != _tab_list.end();
        }
    };
    ////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////
    class KFItemBagConfig : public KFConfigT< KFItemBagSetting >, public KFInstance< KFItemBagConfig >
    {
    public:
        KFItemBagConfig()
        {
            _file_name = "itembag";
        }
    protected:
        // 读取配置
        virtual void ReadSetting( KFNode& xmlnode, KFItemBagSetting* kfsetting );

    public:
        // 默认的背包名字
        std::string _default_bag_name;
    };
}

#endif