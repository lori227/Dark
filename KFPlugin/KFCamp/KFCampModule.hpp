#ifndef __KF_CAMP_MODULE_H__
#define __KF_CAMP_MODULE_H__

/************************************************************************
//    @Module			:    军中帐功能模块
//    @Author           :    erlking
//    @QQ				:    1326273445
//    @Mail			    :    1326273445@qq.com
//    @Date             :    2019-07-26
************************************************************************/

#include "KFrameEx.h"
#include "KFCampInterface.h"
#include "KFPlayer/KFPlayerInterface.h"
#include "KFKernel/KFKernelInterface.h"
#include "KFMessage/KFMessageInterface.h"
#include "KFDisplay/KFDisplayInterface.h"
#include "KFBuildSkinConfig.hpp"

namespace KFrame
{
    class KFCampModule : public KFCampInterface
    {
    public:
        KFCampModule() = default;
        ~KFCampModule() = default;

        // 刷新
        virtual void BeforeRun();

        // 关闭
        virtual void BeforeShut();

        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    protected:
        // 选择建筑外观
        __KF_MESSAGE_FUNCTION__( HandleSetBuildSkinReq );
    };
}

#endif