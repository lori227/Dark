#ifndef __KF_DIALOGUE_INTERFACE_H__
#define __KF_DIALOGUE_INTERFACE_H__

#include "KFrame.h"

namespace KFrame
{
    class KFDialogueInterface : public KFModule
    {
    };

    __KF_INTERFACE__( _kf_dialogue, KFDialogueInterface );
}

#endif