#ifndef __KF_HERO_TEAM_INTERFACE_H__
#define __KF_HERO_TEAM_INTERFACE_H__

#include "KFrameEx.h"

namespace KFrame
{
    class KFHeroTeamInterface : public KFModule
    {
    public:
        // 获取队伍英雄数量
        virtual uint32 GetHeroCount( KFEntity* player ) = 0;

        // 获取队伍活着的英雄数量
        virtual uint32 GetAliveHeroCount( KFEntity* player ) = 0;

        // 获取队伍死亡的英雄数量
        virtual uint32 GetDeadHeroCount( KFEntity* player ) = 0;

        // 更新队伍死亡的英雄
        virtual void UpdateDeadHero( KFEntity* player ) = 0;

        // 队伍英雄的耐久度足够
        virtual bool IsDurabilityEnough( KFEntity* player ) = 0;

        // 战斗扣除队伍英雄耐久度
        virtual void DecHeroPVEDurability( KFEntity* player, const UInt64Set& fightheros, uint32 durability ) = 0;

        // 秘境扣除队伍英雄耐久度
        virtual void DecHeroRealmDurability( KFEntity* player, uint32 durability, const UInt64Set& excludelist ) = 0;

        // 清空队伍英雄的ep
        virtual void ClearHeroEp( KFEntity* player ) = 0;

        // 操作队伍英雄血量数值
        virtual void OperateHpValue( KFEntity* player, uint32 operate, uint32 value, const KeyValue* conditions = nullptr ) = 0;

        // 操作队伍英雄血量百分比
        virtual void OperateHpPercent( KFEntity* player, uint32 operate, uint32 value, const KeyValue* conditions = nullptr ) = 0;
    };

    //////////////////////////////////////////////////////////////////////////////////
    __KF_INTERFACE__( _kf_hero_team, KFHeroTeamInterface );
    //////////////////////////////////////////////////////////////////////////////////
}



#endif