#ifndef __KF_STORE_UNLOCK_MODULE_H__
#define __KF_STORE_UNLOCK_MODULE_H__

/************************************************************************
//    @Module			:    商城解锁系统
//    @Author           :    __凌_痕__
//    @QQ				:    7969936
//    @Mail			    :    lori227@qq.com
//    @Date             :    2020-4-16
************************************************************************/

#include "KFStoreUnlockInterface.h"
#include "KFPlayer/KFPlayerInterface.h"
#include "KFKernel/KFKernelInterface.h"
#include "KFCondition/KFConditionInterface.h"
#include "KFStoreUnlockConfig.hpp"

namespace KFrame
{
    class KFStoreUnlockModule : public KFStoreUnlockInterface
    {
    public:
        KFStoreUnlockModule() = default;
        ~KFStoreUnlockModule() = default;

        // 初始化
        virtual void BeforeRun();

        // 关闭
        virtual void BeforeShut();
        //////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////
        // 判断商店是否激活
        virtual bool IsStoreUnlock( KFEntity* player, uint32 storeid );

    protected:
        // 属性添加回调
        __KF_ADD_DATA_FUNCTION__( OnAddDataUnlockStore );
        __KF_REMOVE_DATA_FUNCTION__( OnRemoveDataUnlockStore );
        __KF_UPDATE_DATA_FUNCTION__( OnUpdateDataUnlockStore );

        // 进入游戏
        __KF_ENTER_PLAYER_FUNCTION__( OnEnterUnlockStore );

        // 解锁商店
        void UnlockStoreData( KFEntity* player, const KFStoreUnlockSetting* kfsetting, KFData* kfindexrecord );

    protected:
        // 玩家上下文组件
        KFComponent* _kf_component = nullptr;
    };

}

#endif