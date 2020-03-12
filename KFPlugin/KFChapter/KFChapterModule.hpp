#ifndef __KF_CHAPTER_MOUDLE_H__
#define __KF_CHAPTER_MOUDLE_H__

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
#include "KFExecute/KFExecuteInterface.h"
#include "KFTask/KFTaskChainInterface.h"
#include "KFZConfig/KFChapterConfig.hpp"
#include "KFZConfig/KFStatusConfig.hpp"

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
        // 完成任务链章节点逻辑
        __KF_FINISH_TASK_CHAIN_FUNCTION__( OnFinishTaskChainChapterLogic );

        // 章节状态
        __KF_ADD_ELEMENT_FUNCTION__( AddChapterElement );
    protected:
        // 请求章节事件
        __KF_MESSAGE_FUNCTION__( HandleChapterExecuteStatusReq );

    protected:
        // 玩家组件上下文
        KFComponent* _kf_component = nullptr;
    };
}

#endif