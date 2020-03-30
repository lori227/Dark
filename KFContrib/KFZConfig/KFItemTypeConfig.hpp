#ifndef __KF_ITEM_TYPE_CONFIG_H__
#define __KF_ITEM_TYPE_CONFIG_H__

#include "KFZConfig/KFConfig.h"

namespace KFrame
{
    class KFItemTypeSeting : public KFIntSetting
    {
    public:
        // 仓库名字
        std::string _store_name;

        // 背包名字
        std::string _bag_name;

        // 额外的背包名
        std::string _extend_name;

        // 移动目标名字列表
        StringSet _move_name_list;

        // 自动处理
        uint32 _is_auto = 0u;

        // 使用限制
        uint32 _use_limit = 0u;

        // 排序索引
        uint32 _sort_index = 0u;

    public:
        // 判断是否能移动
        bool CheckCanMove( const std::string& sourcename, const std::string& targetname ) const;
    };
    ////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////
    class KFItemTypeConfig : public KFConfigT< KFItemTypeSeting >, public KFInstance< KFItemTypeConfig >
    {
    public:
        KFItemTypeConfig()
        {
            _file_name = "itemtype";
        }
    protected:
        // 读取配置
        virtual void ReadSetting( KFNode& xmlnode, KFItemTypeSeting* kfsetting );
    };
}

#endif