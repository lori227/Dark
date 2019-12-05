﻿#ifndef __KF_EXPLORE_DATA_H__
#define __KF_EXPLORE_DATA_H__

#include "KFrameEx.h"
#include "KFCore/KFData.h"
#include "KFKernel/KFEntity.h"
#include "KFProtocol/KFProtocol.h"
#include "KFZConfig/KFItemConfig.hpp"
#include "KFZConfig/KFCurrencyConfig.hpp"

namespace KFrame
{
    ////////////////////////////////////////////////////////////////////////////////
    class KFExploreRecord
    {
    public:
        enum BalanceType
        {
            // 统计类型
            StartType = 0,		// 开始
            EndType = 1,		// 结束
        };

        KFExploreRecord() = default;
        ~KFExploreRecord();

        // 重置
        KFExploreRecord* Reset();

        // 查找英雄
        KFMsg::PBBalanceHeroServer* FindHero( uint64 uuid );

        // 查找道具
        KFMsg::PBBalanceItemServer* FindItem( uint32 id, uint64 uuid );

        // 查找货币
        KFMsg::PBBalanceCurrency* FindCurrency( const std::string& name );
        ////////////////////////////////////////////////////////////////////////////////////////////////
        // 初始化英雄数据
        void BalanceHeroBeginData( KFEntity* player );

        // 英雄最终数据
        void BalanceHeroEndData( KFEntity* player );

        // 初始化道具数据
        void BalanceItemBeginData( KFEntity* player );

        // 道具最终数据
        void BalanceItemEndData( KFEntity* player );

        // 初始化货币数据
        void BalanceCurrencyBeginData( KFEntity* player );

        // 最终货币数据
        void BalanceCurrencyEndData( KFEntity* player );

        // 结算
        void BalanceRecord( KFMsg::PBBalanceData* pbdata );
        ////////////////////////////////////////////////////////////////////////////////////////////////
        // 查找npc数据
        KFMsg::PBExploreNpcData* FindNpcData( const std::string& key );
    protected:
        // 计算道具数据
        void BalanceItemRecordData( KFData* kfitemrecord, uint32 balancetype );
        void BalanceItemData( KFData* kfitem, uint32 balancetype );

        // 结算各项数据
        void BalanceHeroRecord( KFMsg::PBBalanceData* pbdata );
        void BalanceItemRecord( KFMsg::PBBalanceData* pbdata );
        void BalanceCurrencyRecord( KFMsg::PBBalanceData* pbdata );
    public:
        // 结算数据
        KFMsg::PBBalanceDataServer _data;

        // npc 列表
        KFHashMap< uint64, uint64, KFData > _npcs;
    };
}

#endif