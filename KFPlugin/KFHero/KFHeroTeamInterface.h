﻿#ifndef __KF_HERO_TEAM_INTERFACE_H__
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

        // 删除队伍死亡的英雄
        virtual void UpdateTeamDeadHero( KFEntity* player ) = 0;

        // 扣除队伍每个英雄的耐久度
        virtual void DecTeamHeroDurability( KFEntity* player ) = 0;

        // 移除队伍中耐久度不足的英雄
        virtual void RemoveTeamHeroDurability( KFEntity* player ) = 0;

        // 通过uuid查找英雄在队伍位置
        virtual uint32 GetTeamIndexById( KFEntity* player, uint64 uuid ) = 0;
    };

    //////////////////////////////////////////////////////////////////////////////////
    __KF_INTERFACE__( _kf_hero_team, KFHeroTeamInterface );
    //////////////////////////////////////////////////////////////////////////////////
}



#endif