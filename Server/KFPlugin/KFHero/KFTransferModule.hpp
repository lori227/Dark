#ifndef __KF_TRANSFER_MOUDLE_H__
#define __KF_TRANSFER_MOUDLE_H__

/************************************************************************
//    @Moudle			:    转职系统
//    @Author           :    erlking
//    @QQ				:    1326273445
//    @Mail			    :    1326273445@qq.com
//    @Date             :    2019-08-15
************************************************************************/

#include "KFrameEx.h"
#include "KFOption/KFOptionInterface.h"
#include "KFKernel/KFKernelInterface.h"
#include "KFPlayer/KFPlayerInterface.h"
#include "KFDisplay/KFDisplayInterface.h"
#include "KFMessage/KFMessageInterface.h"
#include "KFZConfig/KFTransferConfig.hpp"
#include "KFZConfig/KFProfessionConfig.hpp"
#include "KFZConfig/KFEquipTypeConfig.hpp"
#include "KFZConfig/KFWeightConfig.hpp"
#include "KFZConfig/KFSkillConfig.hpp"
#include "KFZConfig/KFCharacterConfig.hpp"

namespace KFrame
{
    class KFTransferInterface : public KFModule
    {
    };
    //////////////////////////////////////////////////////////////////////////////
    class KFTransferModule : public KFTransferInterface
    {
    public:
        KFTransferModule() = default;
        ~KFTransferModule() = default;

        // 逻辑
        virtual void BeforeRun();

        // 关闭
        virtual void BeforeShut();
        //////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////
    protected:
        // 英雄转职请求
        __KF_MESSAGE_FUNCTION__( HandleTransferProReq );

        // 选择天赋请求
        __KF_MESSAGE_FUNCTION__( HandleSelectInnateReq );

    public:
        // 随机权重数据
        void RandWeightData( KFEntity* player, uint64 uuid, const std::string& str, const std::list<uint32>& slist );

        // 删除英雄天赋
        void RemoveInnateId( KFEntity* player, uint64 uuid, uint32 id );

    protected:
        KFComponent* _kf_component = nullptr;
    };
}



#endif