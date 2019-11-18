#include "KFChapterPlugin.hpp"
#include "KFChapterModule.hpp"
#include "KFConfig/KFConfigInterface.h"
//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
    void KFChapterPlugin::Install()
    {
        __REGISTER_MODULE__( KFChapter );
    }

    void KFChapterPlugin::UnInstall()
    {
        __UN_MODULE__( KFChapter );
    }

    void KFChapterPlugin::LoadModule()
    {
        __FIND_MODULE__( _kf_kernel, KFKernelInterface );
        __FIND_MODULE__( _kf_player, KFPlayerInterface );
        __FIND_MODULE__( _kf_config, KFConfigInterface );
        __FIND_MODULE__( _kf_message, KFMessageInterface );
        __FIND_MODULE__( _kf_display, KFDisplayInterface );
        __FIND_MODULE__( _kf_execute, KFExecuteInterface );
        __FIND_MODULE__( _kf_task_chain, KFTaskChainInterface );
    }

    void KFChapterPlugin::AddConfig()
    {
        __KF_ADD_CONFIG__( KFChapterConfig );
        __KF_ADD_CONFIG__( KFStatusConfig );
    }
}