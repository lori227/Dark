﻿#ifndef __KF_ITEM_SETTING_H__
#define __KF_ITEM_SETTING_H__

#include "KFrameEx.h"
#include "KFCore/KFElement.h"
#include "KFZConfig/KFSetting.h"
#include "KFExecuteData.h"

namespace KFrame
{
    namespace KFItemEnum
    {
        enum EConstDefine
        {
            /////////////////////////////////////////////////////////////////////////////////////////////////////////
            // 物品类型
            None = 0,		// 无类型
            Normal = 1,		// 普通道具
            Gift = 2,		// 礼包, 抽奖道具
            Drug = 3,		// 药品道具
            Weapon = 4,		// 武器道具
            Material = 5,	// 材料道具
            Rune = 6,       // 符石道具( 探索内道具 )
            Other = 7,		// 其他道具( 探索内道具 )
            Food = 8,		// 粮食道具
            Script = 101,	// 脚本道具(不是真正的道具)
            //////////////////////////////////////////////////////////////////////////////////////////////////////
            //////////////////////////////////////////////////////////////////////////////////////////////////////
            UseInAll = 0,		// 任何都能使用
            UseInCity = 1,		// 在主城中使用
            UseInExplore = 2,	// 在探索中使用
            UseInFight = 4,		// 在战斗中使用
            //////////////////////////////////////////////////////////////////////////////////////////////////////
            //////////////////////////////////////////////////////////////////////////////////////////////////////
            // 脚本调用类型
            AddFunction = 1,	// 添加物品调用
            UseFunction = 2,	// 使用物品调用
            RemoveFunction = 3,	// 删除物品调用
            MaxFunction = 4,	// 最大值
            /////////////////////////////////////////////////////////////////////////////////////////////////////
            /////////////////////////////////////////////////////////////////////////////////////////////////////
            OverlayByCount = 1,	// 数量叠加
            OverlayByTime = 2,	// 时间叠加
            /////////////////////////////////////////////////////////////////////////////////////////////////////
            /////////////////////////////////////////////////////////////////////////////////////////////////////
        };
    }

    ////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////
    class KFItemSetting : public KFIntSetting
    {
    public:
        virtual ~KFItemSetting()
        {
            for ( auto execute : _execute_list )
            {
                __KF_DELETE__( KFExecuteData, execute );
            }
            _execute_list.clear();
        }

        // 判断是否能使用
        bool CheckCanUse() const;

        // 获得lua函数
        const std::string& GetFunction( uint32 type ) const;

        // 判断是否真正的物品
        bool IsRealItem() const;

        // 是否可以叠加
        bool IsOverlay() const;

        // 获得堆叠数量
        uint32 GetOverlayCount( const std::string& bagname ) const;

        // 是否自动使用
        bool IsAutoUse() const;

    public:

        //////////////////////////////////////////////////////////////////////////////////////////////////////
        /////////// 通用属性
        // 物品类型
        uint32 _type = 0u;

        // 品质
        uint32 _quality = 0u;

        // 可叠加类型
        uint32 _overlay_type = 0u;

        // 默认叠加数量
        uint32 _overlay_count = 0u;
        StringUInt64 _overlay_count_list;

        // 可使用次数( 0 = 不能使用 )
        uint32 _use_count = 0u;

        // 使用限制
        uint32 _use_limit = KFItemEnum::UseInAll;

        // 出售价格
        KFElements _sell_elements;

        // lua文件
        std::string _lua_file;

        // lua函数
        std::string _function[ KFItemEnum::MaxFunction ];
        //////////////////////////////////////////////////////////////////////////////////////////////////////
        /////////////// 礼包, 随机礼品等道具
        // 掉落id
        uint32 _drop_id = 0u;

        // 获取后自动使用
        uint32 _auto_use = 0u;

        // 奖励
        KFElements _reward;
        //////////////////////////////////////////////////////////////////////////////////////////////////////
        /////////////// 装备等道具
        // 武器类型
        uint32 _weapon_type = 0u;

        // 武器等级
        uint32 _weapon_level = 0u;

        // 使用等级限制
        uint32 _level_limit = 0u;

        // 耐久度
        uint32 _durability = 0u;

        // 耐久度消耗系数
        uint32 _durability_cost = 0u;

        // 武器词缀池列表
        UInt32Vector _affix_pool_list;

        // 种族限定
        UInt32Set _race_limit_list;
        //////////////////////////////////////////////////////////////////////////////////////////////////////
        /////////////// 药品等道具
        // 药品类型
        uint32 _drug_type = 0u;

        // 药品执行功能列表
        std::vector<KFExecuteData*> _execute_list;

        // 使用目标
        uint32 _use_target = 0u;
        //////////////////////////////////////////////////////////////////////////////////////////////////////
        /////////////// 符石等道具

        // 符石类型
        uint32 _rune_type = 0u;

        // 符石等级
        uint32 _rune_level = 0u;

    };
    //////////////////////////////////////////////////////////////////////////////////////////////////////////

}


#endif