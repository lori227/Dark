#ifndef __KF_HERO_MOUDLE_H__
#define __KF_HERO_MOUDLE_H__

/************************************************************************
//    @Moudle			:    英雄包裹系统
//    @Author           :    __凌_痕__
//    @QQ				:    7969936
//    @Mail			    :    lori227@qq.com
//    @Date             :    2017-4-14
************************************************************************/
#include "KFrameEx.h"
#include "KFHeroInterface.h"
#include "KFProtocol/KFProtocol.h"
#include "KFOption/KFOptionInterface.h"
#include "KFConfig/KFConfigInterface.h"
#include "KFKernel/KFKernelInterface.h"
#include "KFPlayer/KFPlayerInterface.h"
#include "KFMessage/KFMessageInterface.h"

namespace KFrame
{
    class KFComponent;
    class KFHeroModule : public KFHeroInterface
    {
    public:
        KFHeroModule() = default;
        ~KFHeroModule() = default;

        // 初始化
        virtual void InitModule();

        // 逻辑
        virtual void BeforeRun();

        // 关闭
        virtual void BeforeShut();
        ////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////
    protected:
        // 添加物品
        __KF_ADD_ELEMENT_FUNCTION__( AddHeroElement );
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
    protected:
        // 随机名字
        std::string& RandHeroName( uint32 firstid, uint32 secondid );

    protected:
        KFComponent* _kf_component = nullptr;
    };
}



#endif