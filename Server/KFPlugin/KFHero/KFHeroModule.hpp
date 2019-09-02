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
#include "KFGenerate/KFGenerateInterface.h"
#include "KFZConfig/KFLevelConfig.hpp"
#include "KFZConfig/KFProfessionConfig.hpp"


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

        // 增加经验
        virtual uint32 AddExp( KFEntity* player, KFData* kfhero, uint32 exp );

        // 判断英雄是否达到最大等级
        virtual bool IsMaxLevel( KFData* kfhero );

        // 添加hp
        virtual uint32 AddHp( KFEntity* player, KFData* kfhero, uint32 hp );

        // 添加属性
        virtual uint32 AddHeroData( KFEntity* player, KFData* kfhero, const std::string& name, int32 value );
        ////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////
    protected:
        // 判断能否添加英雄
        __KF_CHECK_ADD_ELEMENT_FUNCTION__( CheckAddHeroElement );

        // 添加英雄
        __KF_ADD_ELEMENT_FUNCTION__( AddHeroElement );

        // 英雄经验更新
        __KF_UPDATE_DATA_FUNCTION__( OnHeroExpUpdate );

        // 英雄最大hp更新
        __KF_UPDATE_DATA_FUNCTION__( OnHeroMaxhpUpdate );
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
        // 玩家上下文组件
        KFComponent* _kf_component = nullptr;
    };
}



#endif