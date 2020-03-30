#ifndef __KF_GAMBLE_MOUDLE_H__
#define __KF_GAMBLE_MOUDLE_H__

/************************************************************************
//    @Module			:    秘境探索摇奖系统
//    @Author           :    __凌_痕__
//    @QQ				:    7969936
//    @Mail			    :    lori227@qq.com
//    @Date             :    20120-2-19
************************************************************************/

#include "KFrameEx.h"
#include "KFRealmData.hpp"
#include "KFGambleInterface.h"
#include "KFRealmInterface.h"
#include "KFKernel/KFKernelInterface.h"
#include "KFPlayer/KFPlayerInterface.h"
#include "KFDisplay/KFDisplayInterface.h"
#include "KFMessage/KFMessageInterface.h"
#include "KFDrop/KFDropInterface.h"
#include "KFItem/KFItemInterface.h"
#include "KFZConfig/KFGambleConfig.hpp"
#include "KFZConfig/KFExchangeConfig.hpp"
#include "KFZConfig/KFItemConfig.hpp"
#include "KFZConfig/KFSelectConfig.hpp"
#include "KFZConfig/KFMultiEventConfig.hpp"

namespace KFrame
{
    ///////////////////////////////////////////////////////////////////////////////////////
    class KFGambleModule : public KFGambleInterface
    {
    public:
        KFGambleModule() = default;
        ~KFGambleModule() = default;

        // 逻辑
        virtual void BeforeRun();

        // 关闭
        virtual void BeforeShut();
        ///////////////////////////////////////////////////////////////////////////////////////////////////////
        ///////////////////////////////////////////////////////////////////////////////////////////////////////
    protected:
        // 处理请求道具摇奖
        __KF_MESSAGE_FUNCTION__( HandleGambleItemShowReq );

        // 处理开始药具摇奖
        __KF_MESSAGE_FUNCTION__( HandleGambleItemStartReq );

        // 处理请求符石交换
        __KF_MESSAGE_FUNCTION__( HandleRuneExchangeStartReq );

        // 处理掉落选择逻辑
        __KF_MESSAGE_FUNCTION__( HandleDropSelectReq );

        // 处理选择掉落道具
        __KF_MESSAGE_FUNCTION__( HandleSelectItemReq );

        // 处理多事件请求
        __KF_MESSAGE_FUNCTION__( HandleMultiEventReq );

    protected:
        // 掉落抽奖道具
        __KF_DROP_LOGIC_FUNCTION__( OnDropGambleItemLogic );

        // 进入秘境回调
        __KF_REALM_ENTER_FUNCTION__( OnEnterRealm );

    protected:
        // 计算花费
        uint32 CalcGambleCostItemCount( KFGambleData* kfgambledata, const KFGambleSetting* kfsetting );

        // 发送显示掉落选择消息
        void ShowDropSelectMessage( KFEntity* player, KFRealmData* kfrealmdata, uint32 delaytime );

        // 秘境抽奖
        void OnGambleItemStart( KFEntity* player, const KFGambleSetting* kfsetting, uint32 dropid, uint32 gamblecount, uint32 costcount );

        // 符石交换
        void OnRuneExchangeStart( KFEntity* player, uint32 dropid, uint32 exchangeid );

        // 掉落选择
        void OnDropSelect( KFEntity* player, uint32 selectid );
    };
}

#endif