#include "KFDialoguePlugin.hpp"
#include "KFDialogueModule.hpp"
#include "KFConfig/KFConfigInterface.h"
//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
    void KFDialoguePlugin::Install()
    {
        __REGISTER_MODULE__( KFDialogue );
    }

    void KFDialoguePlugin::UnInstall()
    {
        __UN_MODULE__( KFDialogue );
    }

    void KFDialoguePlugin::LoadModule()
    {
        __FIND_MODULE__( _kf_kernel, KFKernelInterface );
        __FIND_MODULE__( _kf_drop, KFDropInterface );
        __FIND_MODULE__( _kf_message, KFMessageInterface );
        __FIND_MODULE__( _kf_display, KFDisplayInterface );
        __FIND_MODULE__( _kf_player, KFPlayerInterface );
        __FIND_MODULE__( _kf_execute, KFExecuteInterface );
    }

    void KFDialoguePlugin::AddConfig()
    {
    }

}