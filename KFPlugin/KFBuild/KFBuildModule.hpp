#ifndef __KF_BUILD_MOUDLE_H__
#define __KF_BUILD_MOUDLE_H__

/************************************************************************
//    @Moudle			:    建筑系统
//    @Author           :    erlking
//    @QQ				:    1326273445
//    @Mail			    :    1326273445@qq.com
//    @Date             :    2019-11-21
************************************************************************/

#include "KFrameEx.h"
#include "KFBuildInterface.h"
#include "KFProtocol/KFProtocol.h"
#include "KFKernel/KFKernelInterface.h"
#include "KFPlayer/KFPlayerInterface.h"
#include "KFDisplay/KFDisplayInterface.h"
#include "KFMessage/KFMessageInterface.h"
#include "KFCondition/KFConditionInterface.h"
#include "KFZConfig/KFBuildConfig.hpp"
#include "KFZConfig/KFTechnologyConfig.hpp"

namespace KFrame
{
    class KFBuildModule : public KFBuildInterface
    {
    public:
        KFBuildModule() = default;
        ~KFBuildModule() = default;

        // 逻辑
        virtual void BeforeRun();

        // 关闭
        virtual void BeforeShut();
        ////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////
    protected:
        // 属性添加回调
        __KF_ADD_DATA_FUNCTION__( OnAddDataBuildModule );
        __KF_REMOVE_DATA_FUNCTION__( OnRemoveDataBuildModule );
        __KF_UPDATE_DATA_FUNCTION__( OnUpdateDataBuildModule );

        // 建筑解锁
        __KF_ADD_DATA_FUNCTION__( OnAddUnlockBuild );

        // 建筑升级
        __KF_UPDATE_DATA_FUNCTION__( OnUpdateBuildLevel );

        // 开始升级建筑
        __KF_MESSAGE_FUNCTION__( HandleStartUpgradeBuildReq );

        // 一键升级建筑
        __KF_MESSAGE_FUNCTION__( HandleOnekeyUpgradeBuildReq );

        // 升级建筑请求
        __KF_MESSAGE_FUNCTION__( HandleUpgradeBuildReq );

    protected:

        // 获取建筑正在升级数量
        uint32 GetUpgradeBuildNum( KFEntity* player );

        // 解锁科技等级
        void UnlockTechnologyLevel( KFEntity* player, KFData* kfdata );

        // 更新建筑升级条件
        void UpdateBuildCondition( KFEntity* player, KFData* kfdata );

    protected:
        // 玩家组件上下文
        KFComponent* _kf_component = nullptr;
    };
}



#endif