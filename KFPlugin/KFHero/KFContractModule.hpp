#ifndef __KF_CONTRACT_MOUDLE_H__
#define __KF_CONTRACT_MOUDLE_H__

/************************************************************************
//    @Moudle			:    契约系统
//    @Author           :    erlking
//    @QQ				:    1326273445
//    @Mail			    :    1326273445@qq.com
//    @Date             :    2020-03-06
************************************************************************/

#include "KFrameEx.h"
#include "KFOption/KFOptionInterface.h"
#include "KFKernel/KFKernelInterface.h"
#include "KFPlayer/KFPlayerInterface.h"
#include "KFDisplay/KFDisplayInterface.h"
#include "KFMessage/KFMessageInterface.h"
#include "KFZConfig/KFFormulaConfig.h"
#include "KFZConfig/KFElementConfig.h"

namespace KFrame
{
    class KFContractInterface : public KFModule
    {
    };
    //////////////////////////////////////////////////////////////////////////////
    class KFContractModule : public KFContractInterface
    {
    public:
        KFContractModule() = default;
        ~KFContractModule() = default;

        // 逻辑
        virtual void BeforeRun();

        // 关闭
        virtual void BeforeShut();
        //////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////
    protected:
        // 签约英雄数据请求
        __KF_MESSAGE_FUNCTION__( HandleContractDataReq );

        // 签约英雄抉择
        __KF_MESSAGE_FUNCTION__( HandleContractHeroReq );

    public:
        // 删除英雄天赋
        // bool RemoveInnateId( KFEntity* player, KFData* kfhero, uint32 id );

        // 获取续签数据
        bool GetContractData( KFData* kfhero, uint32& durability, std::string& cost, uint32& errnum );

    protected:
        KFComponent* _kf_component = nullptr;
    };
}



#endif