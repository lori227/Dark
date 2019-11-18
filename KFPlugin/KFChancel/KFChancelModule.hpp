#ifndef __KF_CHANCEL_MODULE_H__
#define __KF_CHANCEL_MODULE_H__

/************************************************************************
//    @Module			:    英雄圣坛
//    @Author           :    erlking
//    @QQ				:    1326273445
//    @Mail			    :    1326273445@qq.com
//    @Date             :    2019-08-21
************************************************************************/

#include "KFrameEx.h"
#include "KFChancelInterface.h"
#include "KFPlayer/KFPlayerInterface.h"
#include "KFProtocol/KFProtocol.h"
#include "KFKernel/KFKernelInterface.h"
#include "KFDisplay/KFDisplayInterface.h"
#include "KFZConfig/KFChancelConfig.hpp"
#include "KFZConfig/KFProfessionConfig.hpp"
#include "KFRecordClient/KFRecordClientInterface.h"

namespace KFrame
{
    class KFChancelModule : public KFChancelInterface
    {
    public:
        KFChancelModule() = default;
        ~KFChancelModule() = default;

        // 刷新
        virtual void BeforeRun();

        // 关闭
        virtual void BeforeShut();

    protected:
        // 添加英雄
        __KF_ADD_DATA_FUNCTION__( OnAddHero );

        // 删除英雄
        __KF_REMOVE_DATA_FUNCTION__( OnRemoveHero );

    protected:
        //////////////////////////////////////////////////////////////////////////////////////

        // 更新英雄死亡数据
        void UpdateDeadData( KFEntity* player, KFData* kfhero );

        // 更新死亡数量
        void UpdateDeadNum( KFEntity* player, uint32 type );

        // 更新死亡等级
        void UpdateDeadLevel( KFEntity* player, uint32 type );


    protected:
        KFComponent* _kf_component = nullptr;
    };
}

#endif