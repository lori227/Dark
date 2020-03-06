#ifndef __KF_DIALOGUE_INTERFACE_H__
#define __KF_DIALOGUE_INTERFACE_H__

#include "KFrame.h"
#include "KFProtocol/KFProtocol.h"

namespace KFrame
{
    class KFDialogueInterface : public KFModule
    {
    public:
        // 添加对话
        virtual void AddDialogue( KFEntity* player, uint32 dialogid, uint32 type = KFMsg::UIDialogue, uint32 storyid = 0u ) = 0;
    };

    __KF_INTERFACE__( _kf_dialogue, KFDialogueInterface );
}

#endif