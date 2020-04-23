#ifndef __KF_ITEM_TYPE_CONFIG_H__
#define __KF_ITEM_TYPE_CONFIG_H__

#include "KFZConfig/KFConfig.h"

namespace KFrame
{
    class KFItemTypeSetting : public KFIntSetting
    {
    public:
        // 移动目标名字列表
        StringSet _move_name_list;

        // 自动处理
        uint32 _is_auto = 0u;

        // 使用限制
        uint32 _use_limit = 0u;

        // 排序索引
        uint32 _sort_index = 0u;

        // 可以放入的页签列表
        StringSet _tab_name_list;

        // 状态对应的背包名
        UInt64String _status_bag_name;

    public:
        // 判断是否能移动
        bool CheckCanMove( const std::string& sourcename, const std::string& targetname ) const;

        // 是否在页签中
        bool IsHaveTab( const std::string& name ) const;

        // 查找背包名
        const std::string& GetBagName( uint32 status ) const;
    };
    ////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////
    class KFItemTypeConfig : public KFConfigT< KFItemTypeSetting >, public KFInstance< KFItemTypeConfig >
    {
    public:
        KFItemTypeConfig()
        {
            _file_name = "itemtype";
        }
    protected:
        // 读取配置
        virtual void ReadSetting( KFNode& xmlnode, KFItemTypeSetting* kfsetting );
    };
}

#endif