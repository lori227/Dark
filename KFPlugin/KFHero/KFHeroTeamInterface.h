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

        // 删除队伍死亡的英雄
        virtual void RemoveTeamDeadHero( KFEntity* player ) = 0;
    };

    //////////////////////////////////////////////////////////////////////////////////
    __KF_INTERFACE__( _kf_hero_team, KFHeroTeamInterface );
    //////////////////////////////////////////////////////////////////////////////////
}



#endif