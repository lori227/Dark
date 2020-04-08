#ifndef __KF_HERO_MOUDLE_H__
#define __KF_HERO_MOUDLE_H__

/************************************************************************
//    @Moudle			:    英雄逻辑系统
//    @Author           :    __凌_痕__
//    @QQ				:    7969936
//    @Mail			    :    lori227@qq.com
//    @Date             :    2017-4-14
************************************************************************/
#include "KFrameEx.h"
#include "KFHeroInterface.h"
#include "KFKernel/KFKernelInterface.h"
#include "KFPlayer/KFPlayerInterface.h"
#include "KFMessage/KFMessageInterface.h"
#include "KFDisplay/KFDisplayInterface.h"
#include "KFFilter/KFFilterInterface.h"
#include "KFExecute/KFExecuteInterface.h"
#include "KFGenerate/KFGenerateInterface.h"
#include "KFZConfig/KFLevelConfig.hpp"
#include "KFZConfig/KFProfessionConfig.hpp"
#include "KFRealm/KFRealmInterface.h"

namespace KFrame
{
    class KFHeroModule : public KFHeroInterface
    {
    public:
        KFHeroModule() = default;
        ~KFHeroModule() = default;

        // 逻辑
        virtual void BeforeRun();

        // 关闭
        virtual void BeforeShut();
        ////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////
        // 英雄是否达到最大数量
        virtual bool IsFull( KFEntity* player, KFData* kfherorecord );

        // 增加经验
        virtual uint32 AddExp( KFEntity* player, KFData* kfhero, uint32 exp );

        // 判断英雄是否达到最大等级
        virtual bool IsMaxLevel( KFEntity* player, KFData* kfhero );

        // 获取玩家最大等级
        virtual uint32 GetPlayerMaxLevel( KFEntity* player );

        // 计算英雄的最大等级
        virtual uint32 CalcMaxLevel( KFEntity* player, KFData* kfhero );

        // 添加hp
        virtual uint32 OperateHp( KFEntity* player, KFData* kfhero, uint32 operate, uint32 hp );

        // 添加属性
        virtual uint32 AddHeroData( KFEntity* player, KFData* kfhero, const std::string& name, int32 value );

        // 查找存活的英雄
        virtual KFData* FindAliveHero( KFData* kfherorecord, uint64 uuid );
        ////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////
    protected:
        // 锁定英雄
        __KF_MESSAGE_FUNCTION__( HandleLockHeroReq );

        // 删除英雄
        __KF_MESSAGE_FUNCTION__( HandleRemoveHeroReq );

        // 设置英雄名字
        __KF_MESSAGE_FUNCTION__( HandleSetHeroNameReq );

        // 选择英雄主动技能
        __KF_MESSAGE_FUNCTION__( HandleSetHeroActiveSkillReq );
        ///////////////////////////////////////////////////////////////////////
        ///////////////////////////////////////////////////////////////////////
    protected:
        // 进入游戏
        __KF_ENTER_PLAYER_FUNCTION__( OnEnterHeroModule );

        // 判断能否添加英雄
        __KF_CHECK_ADD_ELEMENT_FUNCTION__( CheckAddHeroElement );

        // 添加英雄
        __KF_ADD_ELEMENT_FUNCTION__( AddHeroElement );

        // 判断能否添加英雄
        __KF_CHECK_REMOVE_ELEMENT_FUNCTION__( CheckRemoveHeroElement );

        // 删除英雄
        __KF_REMOVE_ELEMENT_FUNCTION__( RemoveHeroElement );

        // 英雄经验更新
        __KF_UPDATE_DATA_FUNCTION__( OnHeroExpUpdate );

        // 英雄主动技能更新
        __KF_ADD_DATA_FUNCTION__( OnHeroActiveUpdate );

        // 添加hp
        __KF_EXECUTE_FUNCTION__( OnExecuteAddHp );

        // 减少hp
        __KF_EXECUTE_FUNCTION__( OnExecuteDecHp );

        // 添加dip
        __KF_EXECUTE_FUNCTION__( OnExecuteAddDip );

        // 减少dip
        __KF_EXECUTE_FUNCTION__( OnExecuteDecDip );

        // 添加英雄等级
        __KF_EXECUTE_FUNCTION__( OnExecuteTechnologyHeroLevel );

        // 最大英雄数量
        __KF_EXECUTE_FUNCTION__( OnExecuteTechnologyMaxHeroCount );

    protected:
        // 更新所有英雄的最大等级
        void UpdateAllHeroMaxLevel( KFEntity* player );

        // 玩家上下文组件
        KFComponent* _kf_component = nullptr;
    };
}



#endif