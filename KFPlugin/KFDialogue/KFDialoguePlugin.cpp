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
        __FIND_MODULE__( _kf_condition, KFConditionInterface );
        __FIND_MODULE__( _kf_config, KFConfigInterface );
    }

    void KFDialoguePlugin::AddConfig()
    {
        __KF_ADD_CONFIG__( KFDialogueConfig );
        __KF_ADD_CONFIG__( KFDialogueBranchConfig );
        __KF_ADD_CONFIG__( KFElementConfig );
    }

}