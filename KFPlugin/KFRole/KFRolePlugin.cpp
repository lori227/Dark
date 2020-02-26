#include "KFRolePlugin.hpp"
#include "KFRoleModule.hpp"
#include "KFConfig/KFConfigInterface.h"
//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
    void KFRolePlugin::Install()
    {
        __REGISTER_MODULE__( KFRole );
    }

    void KFRolePlugin::UnInstall()
    {
        __UN_MODULE__( KFRole );
    }

    void KFRolePlugin::LoadModule()
    {
        __FIND_MODULE__( _kf_kernel, KFKernelInterface );
        __FIND_MODULE__( _kf_player, KFPlayerInterface );
        __FIND_MODULE__( _kf_message, KFMessageInterface );
        __FIND_MODULE__( _kf_display, KFDisplayInterface );
        __FIND_MODULE__( _kf_config, KFConfigInterface );
        __FIND_MODULE__( _kf_task, KFTaskInterface );
        __FIND_MODULE__( _kf_story, KFStoryInterface );
    }

    void KFRolePlugin::AddConfig()
    {
        __KF_ADD_CONFIG__( KFIconConfig );
        __KF_ADD_CONFIG__( KFFactionConfig );
        __KF_ADD_CONFIG__( KFInitialProcessConfig );
        __KF_ADD_CONFIG__( KFChapterConfig );
        __KF_ADD_CONFIG__( KFStatusConfig );
    }
}