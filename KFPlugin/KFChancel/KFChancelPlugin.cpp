#include "KFChancelPlugin.hpp"
#include "KFChancelModule.hpp"
#include "KFConfig/KFConfigInterface.h"

namespace KFrame
{
    void KFChancelPlugin::Install()
    {
        __REGISTER_MODULE__( KFChancel );
    }

    void KFChancelPlugin::UnInstall()
    {
        __UN_MODULE__( KFChancel );
    }

    void KFChancelPlugin::LoadModule()
    {
        __FIND_MODULE__( _kf_config, KFConfigInterface );
        __FIND_MODULE__( _kf_player, KFPlayerInterface );
        __FIND_MODULE__( _kf_kernel, KFKernelInterface );
        __FIND_MODULE__( _kf_display, KFDisplayInterface );
        __FIND_MODULE__( _kf_record_client, KFRecordClientInterface );
    }

    void KFChancelPlugin::AddConfig()
    {
        __KF_ADD_CONFIG__( KFChancelConfig );
        __KF_ADD_CONFIG__( KFProfessionConfig );
    }
}