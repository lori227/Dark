#ifndef __KF_EXPLORE_MOUDLE_H__
#define __KF_EXPLORE_MOUDLE_H__

/************************************************************************
//    @Module			:    世界地图章节系统
//    @Author           :    __凌_痕__
//    @QQ				:    7969936
//    @Mail			    :    lori227@qq.com
//    @Date             :    2019-8-27
************************************************************************/

#include "KFrameEx.h"
#include "KFChapterInterface.h"
#include "KFKernel/KFKernelInterface.h"
#include "KFPlayer/KFPlayerInterface.h"
#include "KFMessage/KFMessageInterface.h"
#include "KFDisplay/KFDisplayInterface.h"

namespace KFrame
{
    class KFChapterModule : public KFChapterInterface
    {
    public:
        KFChapterModule() = default;
        ~KFChapterModule() = default;

        // 逻辑
        virtual void BeforeRun();

        // 关闭
        virtual void BeforeShut();
        ////////////////////////////////////////////////////////////////////////////////
    protected:
        // 玩家组件上下文
        KFComponent* _kf_component = nullptr;
    };
}

#endif