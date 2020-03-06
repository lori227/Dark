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
#include "KFGenerate/KFGenerateInterface.h"
#include "KFHero/KFHeroInterface.h"
#include "KFDrop/KFDropInterface.h"

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

        // 更新队伍死亡的英雄
        virtual void UpdateDeadHero( KFEntity* player );

        // 扣除队伍每个英雄的耐久度
        virtual void DecHeroDurability( KFEntity* player, const UInt64Set& fightheros );

        // 移除队伍中耐久度不足的英雄
        virtual void RemoveDurabilityHero( KFEntity* player );

        // 清空队伍英雄ep
        virtual void ClearHeroEp( KFEntity* player );

        // 操作队伍英雄血量
        virtual void OperateHpValue( KFEntity* player, uint32 operate, uint32 value, const KeyValue* conditions = nullptr );

        // 操作队伍英雄血量百分比
        virtual void OperateHpPercent( KFEntity* player, uint32 operate, uint32 value, const KeyValue* conditions = nullptr );
        ////////////////////////////////////////////////////////////////////////////////
    protected:
        // 进入游戏
        __KF_ENTER_PLAYER_FUNCTION__( OnEnterHeroTeamModule );

        // 删除英雄
        __KF_REMOVE_DATA_FUNCTION__( OnRemoveHero );


        // 掉落增加英雄血量
        __KF_DROP_LOGIC_FUNCTION__( OnDropHeroAddHp );

        // 掉落减少英雄血量
        __KF_DROP_LOGIC_FUNCTION__( OnDropHeroDecHp );

    private:
        // 队伍改变请求(英雄不在队伍中)
        __KF_MESSAGE_FUNCTION__( HandleHeroTeamChangeReq );

        // 队伍角色互换请求(英雄在队伍中)
        __KF_MESSAGE_FUNCTION__( HandleHeroTeamExchangeReq );

        // 添加队伍hp
        __KF_MESSAGE_FUNCTION__( HandleAddTeamHpReq );

        // 减少队伍hp
        __KF_MESSAGE_FUNCTION__( HandleDecTeamHpReq );
    protected:
        // 检查队伍英雄
        void CheckHeroInTeam( KFEntity* player );

        // 判断英雄限制条件
        bool CheckHeroConditions( KFData* kfhero, const KeyValue* conditions );
    protected:
        // 玩家上下文组件
        KFComponent* _kf_component = nullptr;
    };
}



#endif