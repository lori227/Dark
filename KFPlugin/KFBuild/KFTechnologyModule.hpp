#ifndef __KF_TECHNOLOGY_MOUDLE_H__
#define __KF_TECHNOLOGY_MOUDLE_H__

/************************************************************************
//    @Moudle			:    科技系统
//    @Author           :    erlking
//    @QQ				:    1326273445
//    @Mail			    :    1326273445@qq.com
//    @Date             :    2019-11-25
************************************************************************/

#include "KFrameEx.h"
#include "KFProtocol/KFProtocol.h"
#include "KFTechnologyInterface.h"
#include "KFKernel/KFKernelInterface.h"
#include "KFPlayer/KFPlayerInterface.h"
#include "KFDisplay/KFDisplayInterface.h"
#include "KFMessage/KFMessageInterface.h"
#include "KFExecute/KFExecuteInterface.h"
#include "KFZConfig/KFTechnologyConfig.hpp"


namespace KFrame
{
    //////////////////////////////////////////////////////////////////////////////
    class KFTechnologyModule : public KFTechnologyInterface
    {
    public:
        KFTechnologyModule() = default;
        ~KFTechnologyModule() = default;

        // 逻辑
        virtual void BeforeRun();

        // 关闭
        virtual void BeforeShut();
        ////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////
    protected:
        // 进入游戏
        __KF_ENTER_PLAYER_FUNCTION__( OnEnterTechnologyModule );

        // 添加科技
        __KF_ADD_ELEMENT_FUNCTION__( AddTechnologyElement );

        // 科技解锁
        __KF_ADD_DATA_FUNCTION__( OnAddUnlockTechnology );

        // 科技更新
        __KF_UPDATE_DATA_FUNCTION__( OnUpdateTechnologyStatus );

        // 升级科技请求
        __KF_MESSAGE_FUNCTION__( HandleUpgradeTechnologyReq );

        // 升级科技
        void OnUpgradeTechnology( KFEntity* player, KFData* kftechnology );

        // 更新科技数据
        void UpdateTechnologyData( KFEntity* player, KFData* kftechnologyrecord, uint32 technologyid, uint32 status );
        void UpdateTechnologyData( KFEntity* player, KFData* kftechnologyrecord, const KFTechnologySetting* kfsetting, uint32 status );

    protected:
        // 玩家组件上下文
        KFComponent* _kf_component = nullptr;
    };
}



#endif