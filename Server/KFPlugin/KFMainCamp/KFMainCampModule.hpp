#ifndef __KF_MAIN_CAMP_MODULE_H__
#define __KF_MAIN_CAMP_MODULE_H__

/************************************************************************
//    @Module			:    军中帐功能模块
//    @Author           :    erlking
//    @QQ				:    1326273445
//    @Mail			    :    1326273445@qq.com
//    @Date             :    2019-07-26
************************************************************************/

#include "KFrameEx.h"
#include "KFMainCampInterface.h"
#include "KFTimer/KFTimerInterface.h"
#include "KFPlayer/KFPlayerInterface.h"
#include "KFKernel/KFKernelInterface.h"
#include "KFMessage/KFMessageInterface.h"
#include "KFDisplay/KFDisplayInterface.h"
#include "KFOption/KFOptionInterface.h"
#include "KFRouteClient/KFRouteClientInterface.h"
#include "KFZConfig/KFBuildLevelConfig.hpp"
#include "KFZConfig/KFBuildSkinConfig.hpp"

namespace KFrame
{
    class KFMainCampModule : public KFMainCampInterface
    {
    public:
        KFMainCampModule() = default;
        ~KFMainCampModule() = default;

        // 刷新
        virtual void BeforeRun();

        // 关闭
        virtual void BeforeShut();

        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    protected:
        // 建筑等级更新
        __KF_UPDATE_DATA_FUNCTION__( OnBuildLevelUpdate );

        // 进入游戏
        __KF_ENTER_PLAYER_FUNCTION__( OnEnterMainCampModule );

        // 离开游戏
        __KF_LEAVE_PLAYER_FUNCTION__( OnLeaveMainCampModule );
    protected:
        // 选择建筑外观
        __KF_MESSAGE_FUNCTION__( HandleSetBuildSkinReq );
    protected:
        //////////////////////////////////////////////////////////////////////////////////////
        // 获取军中帐等级
        uint32 GetMainCampLevel( KFEntity* player );

        // 更新建筑等级
        void UpdateBuildLevel( KFEntity* player );

        // 更新建筑皮肤
        void UpdateBuildSkin( KFEntity* player );
    protected:
        // 玩家上下文组件
        KFComponent* _kf_component = nullptr;
    };
}

#endif