#ifndef __KF_TEAM_MOUDLE_H__
#define __KF_TEAM_MOUDLE_H__

/************************************************************************
//    @Moudle			:    英雄队伍系统
//    @Author           :    __凌_痕__
//    @QQ				:    7969936
//    @Mail			    :    lori227@qq.com
//    @Date             :    2019-7-15
************************************************************************/

#include "KFHeroTeamInterface.h"
#include "KFOption/KFOptionInterface.h"
#include "KFKernel/KFKernelInterface.h"
#include "KFPlayer/KFPlayerInterface.h"
#include "KFDisplay/KFDisplayInterface.h"
#include "KFMessage/KFMessageInterface.h"

namespace KFrame
{
    class KFHeroTeamModule : public KFHeroTeamInterface
    {
    public:
        KFHeroTeamModule() = default;
        ~KFHeroTeamModule() = default;

        // 逻辑
        virtual void BeforeRun();

        // 关闭
        virtual void BeforeShut();
        ////////////////////////////////////////////////////////////////////////////////
        // 获取队伍英雄数量
        virtual uint32 GetHeroCount( KFEntity* player );

        // 获取队伍活着的英雄数量
        virtual uint32 GetAliveHeroCount( KFEntity* player );

        // 获取队伍死亡的英雄数量
        virtual uint32 GetDeadHeroCount( KFEntity* player );

        // 删除队伍死亡的英雄
        virtual void RemoveTeamDeadHero( KFEntity* player );

        // 扣除队伍英雄的耐久度
        virtual void DecTeamHeroDurability( KFEntity* player );

        ////////////////////////////////////////////////////////////////////////////////
        // 通过uuid查找英雄在队伍位置
        uint32 GetIndexById( KFEntity* player, uint64 uuid );
    protected:
        // 进入游戏
        __KF_ENTER_PLAYER_FUNCTION__( OnEnterHeroTeamModule );

        // 删除英雄
        __KF_REMOVE_DATA_FUNCTION__( OnRemoveHero );

        // 队伍改变请求(增加删除)
        __KF_MESSAGE_FUNCTION__( HandleHeroTeamChangeReq );

        // 队伍角色互换请求
        __KF_MESSAGE_FUNCTION__( HandleHeroTeamExchangeReq );

    protected:
        // 玩家上下文组件
        KFComponent* _kf_component = nullptr;
    };
}



#endif