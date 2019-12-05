#ifndef __KF_DIALOGUE_MOUDLE_H__
#define __KF_DIALOGUE_MOUDLE_H__

/************************************************************************
//    @Moudle			:    对话系统
//    @Author           :    zux
//    @QQ				:    415906519
//    @Mail			    :    415906519@qq.com
//    @Date             :    2019-11-28
************************************************************************/

#include "KFrameEx.h"
#include "KFDialogueInterface.h"
#include "KFKernel/KFKernelInterface.h"
#include "KFPlayer/KFPlayerInterface.h"
#include "KFDisplay/KFDisplayInterface.h"
#include "KFMessage/KFMessageInterface.h"
#include "KFProtocol/KFProtocol.h"
#include "KFDrop/KFDropInterface.h"
#include "KFExecute/KFExecuteInterface.h"

/*
#include "KFTimer/KFTimerInterface.h"
#include "KFHero/KFHeroInterface.h"
#include "KFRecordClient/KFRecordClientInterface.h"
#include "KFZConfig/KFLevelConfig.hpp"
*/

namespace KFrame
{
    class KFDialogueModule : public KFDialogueInterface
    {
    public:
        KFDialogueModule() = default;
        ~KFDialogueModule() = default;

        // 逻辑
        virtual void BeforeRun();

        // 关闭
        virtual void BeforeShut();
        ////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////
    protected:
        // 添加对话id
        __KF_ADD_ELEMENT_FUNCTION__( AddDialogueElement );

        // 掉落对话段落回调
        __KF_DROP_LOGIC_FUNCTION__( OnDropDialogue );

        // 进入世界地图点
        __KF_EXECUTE_FUNCTION__( OnExecuteDialogue );

        // 告知完成对话
        __KF_MESSAGE_FUNCTION__( HandleReceiveDialogueFinishReq );

    protected:
        // 通知客户端对话消息
        bool SendToClientDialogueStart( KFEntity* player, uint32 dialogid, const char* function, uint32 line );

    protected:
        // 玩家组件上下文
        KFComponent* _kf_component = nullptr;
    };
}



#endif