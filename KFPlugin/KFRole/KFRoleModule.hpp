#ifndef __KF_ROLE_MODULE_H__
#define __KF_ROLE_MODULE_H__

/************************************************************************
//    @Module			:    角色逻辑
//    @Author           :    __凌_痕__
//    @QQ				:    7969936
//    @Mail			    :    lori227@qq.com
//    @Date             :    2018-12-27
************************************************************************/
#include "KFrameEx.h"
#include "KFRoleInterface.h"
#include "KFProtocol/KFProtocol.h"
#include "KFKernel/KFKernelInterface.h"
#include "KFMessage/KFMessageInterface.h"
#include "KFPlayer/KFPlayerInterface.h"
#include "KFDisplay/KFDisplayInterface.h"
#include "KFIconConfig.hpp"
#include "KFFactionConfig.hpp"
#include "KFTask/KFTaskInterface.h"
#include "KFStory/KFStoryInterface.h"
#include "KFZConfig/KFInitialProcessConfig.hpp"

namespace KFrame
{
    class KFRoleModule : public KFRoleInterface
    {
    public:
        KFRoleModule() = default;
        ~KFRoleModule() = default;

        // 逻辑
        virtual void BeforeRun();

        // 关闭
        virtual void BeforeShut();
        ////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////
    protected:
        // 处理设置玩家头像请求
        __KF_MESSAGE_FUNCTION__( HandleSetPlayerHeadIconReq );

        // 处理设置玩家势力请求
        __KF_MESSAGE_FUNCTION__( HandleSetPlayerFactionReq );

        // 更新主线流程
        __KF_MESSAGE_FUNCTION__( HandleUpdateMainStageReq );

    protected:
        // 金币更新
        __KF_UPDATE_DATA_FUNCTION__( OnUpdateMoneyCallBack );

        // 主线流程更新
        __KF_UPDATE_DATA_FUNCTION__( OnMainStageUpdate );

        // 删除剧情
        __KF_REMOVE_DATA_FUNCTION__( OnRemoveStoryCallBack );

        // 更新剧情
        __KF_UPDATE_DATA_FUNCTION__( OnUpdateStoryCallBack );

        // PVE结算更新
        __KF_UPDATE_DATA_FUNCTION__( OnUpdatePVECallBack );

        // 探索结算更新
        __KF_UPDATE_DATA_FUNCTION__( OnUpdateRealmCallBack );

        // 进入游戏检查
        __KF_ENTER_PLAYER_FUNCTION__( OnEnterRoleModule );

    protected:
        // 执行初始化流程
        void OnExecuteInitialProcess( KFEntity* player, uint32 id );

        // 增加序列
        void AddSequence( KFEntity* player, KFData* kfdata, uint32 type );
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    private:
        // 玩家组件上下文
        KFComponent* _kf_component = nullptr;
    };
}



#endif