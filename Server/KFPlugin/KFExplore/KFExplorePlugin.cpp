#include "KFExplorePlugin.hpp"
#include "KFExploreModule.hpp"
#include "KFConfig/KFConfigInterface.h"
//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
    void KFExplorePlugin::Install()
    {
        __REGISTER_MODULE__( KFExplore );
    }

    void KFExplorePlugin::UnInstall()
    {
        __UN_MODULE__( KFExplore );
    }

    void KFExplorePlugin::LoadModule()
    {
        __FIND_MODULE__( _kf_drop, KFDropInterface );
        __FIND_MODULE__( _kf_hero, KFHeroInterface );
        __FIND_MODULE__( _kf_kernel, KFKernelInterface );
        __FIND_MODULE__( _kf_player, KFPlayerInterface );
        __FIND_MODULE__( _kf_message, KFMessageInterface );
        __FIND_MODULE__( _kf_display, KFDisplayInterface );
        __FIND_MODULE__( _kf_generate, KFGenerateInterface );
    }

    void KFExplorePlugin::AddConfig()
    {
        __KF_ADD_CONFIG__( KFExploreConfig );
    }
}