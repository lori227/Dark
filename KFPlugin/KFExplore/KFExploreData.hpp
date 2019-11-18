#ifndef __KF_EXPLORE_DATA_H__
#define __KF_EXPLORE_DATA_H__

#include "KFrameEx.h"
#include "KFCore/KFData.h"
#include "KFKernel/KFEntity.h"
#include "KFProtocol/KFProtocol.h"
#include "KFZConfig/KFCurrencyConfig.hpp"

namespace KFrame
{
    ////////////////////////////////////////////////////////////////////////////////
    class KFExploreRecord
    {
    public:
        KFExploreRecord() = default;
        ~KFExploreRecord();

        // 重置
        KFExploreRecord* Reset();

        // 查找道具
        KFMsg::PBBalanceItemServer* FindItem( const std::string& name, uint32 key );

        // 查找英雄
        KFMsg::PBBalanceHeroServer* FindHero( uint64 uuid );

        // 初始化英雄数据
        void BalanceHeroBeginData( KFEntity* player );

        // 英雄最终数据
        void BalanceHeroEndData( KFEntity* player );

        // 初始化道具数据
        void BalanceItemBeginData( KFEntity* player );

        // 道具最终数据
        void BalanceItemEndData( KFEntity* player );

        // 结算
        void BalanceRecord( KFEntity* player, KFMsg::PBBalanceData* pbdata );

        // 查找npc数据
        KFMsg::PBExploreNpcData* FindNpcData( const std::string& key );

    public:
        // 结算数据
        KFMsg::PBBalanceDataServer _data;

        // npc 列表
        KFHashMap< uint64, uint64, KFData > _npcs;
    };
}

#endif