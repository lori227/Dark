#ifndef __KF_MARSHAL_MOUDLE_H__
#define __KF_MARSHAL_MOUDLE_H__

/************************************************************************
//    @Moudle			:    英雄编队系统
//    @Author           :    __凌_痕__
//    @QQ				:    7969936
//    @Mail			    :    lori227@qq.com
//    @Date             :    2019-7-15
************************************************************************/
#include "KFrameEx.h"
#include "KFKernel/KFKernelInterface.h"
#include "KFPlayer/KFPlayerInterface.h"
#include "KFDisplay/KFDisplayInterface.h"
#include "KFMessage/KFMessageInterface.h"

namespace KFrame
{
    class KFMarshalInterface : public KFModule
    {
    public:
    };
    ////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////

    class KFMarshalModule : public KFMarshalInterface
    {
    public:
        KFMarshalModule() = default;
        ~KFMarshalModule() = default;

        // 逻辑
        virtual void BeforeRun();

        // 关闭
        virtual void BeforeShut();
        ////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////

        // 通过uuid查找英雄在队伍位置
        uint32 GetIndexById( KFEntity* player, uint64 uuid );
    protected:
        // 删除英雄
        __KF_REMOVE_DATA_FUNCTION__( OnRemoveHero );

        // 队伍改变请求(增加删除)
        __KF_MESSAGE_FUNCTION__( HandleHeroTeamChangeReq );

        // 队伍角色互换请求
        __KF_MESSAGE_FUNCTION__( HandleHeroTeamExchangeReq );

    protected:
        // 玩家上下文组件
        KFComponent* _kf_component = nullptr;
    };
}



#endif