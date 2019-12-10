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
#include "KFFilter/KFFilterInterface.h"
#include "KFRouteClient/KFRouteClientInterface.h"
#include "KFIconConfig.hpp"
#include "KFFactionConfig.hpp"

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
        // 处理设置性别请求
        __KF_MESSAGE_FUNCTION__( HandleSetSexReq );

        // 处理设置名字请求
        __KF_MESSAGE_FUNCTION__( HandleSetNameReq );

        // 处理设置名字回馈
        __KF_MESSAGE_FUNCTION__( HandleSetPlayerNameToGameAck );

        // 处理设置玩家头像请求
        __KF_MESSAGE_FUNCTION__( HandleSetPlayerHeadIconReq );

        // 处理设置玩家势力请求
        __KF_MESSAGE_FUNCTION__( HandleSetPlayerFactionReq );

    protected:
        // 金币更新
        __KF_UPDATE_DATA_FUNCTION__( OnUpdateMoneyCallBack );

    protected:
        // 检查名字的有效性
        uint32 CheckNameValid( const std::string& name, uint32 maxlength );
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    private:
        // 玩家组件上下文
        KFComponent* _kf_component = nullptr;
    };
}



#endif