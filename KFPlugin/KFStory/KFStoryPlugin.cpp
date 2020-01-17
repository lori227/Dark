#include "KFStoryPlugin.hpp"
#include "KFStoryModule.hpp"
#include "KFConfig/KFConfigInterface.h"
//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
    void KFStoryPlugin::Install()
    {
        __REGISTER_MODULE__( KFStory );
    }

    void KFStoryPlugin::UnInstall()
    {
        __UN_MODULE__( KFStory );
    }

    void KFStoryPlugin::LoadModule()
    {
        __FIND_MODULE__( _kf_kernel, KFKernelInterface );
        __FIND_MODULE__( _kf_drop, KFDropInterface );
        __FIND_MODULE__( _kf_message, KFMessageInterface );
        __FIND_MODULE__( _kf_display, KFDisplayInterface );
        __FIND_MODULE__( _kf_player, KFPlayerInterface );
        __FIND_MODULE__( _kf_execute, KFExecuteInterface );
        __FIND_MODULE__( _kf_task, KFTaskInterface );
        __FIND_MODULE__( _kf_dialogue, KFDialogueInterface );
        __FIND_MODULE__( _kf_config, KFConfigInterface );
    }

    void KFStoryPlugin::AddConfig()
    {
        __KF_ADD_CONFIG__( KFStoryConfig );
        __KF_ADD_CONFIG__( KFDialogueConfig );
    }

}