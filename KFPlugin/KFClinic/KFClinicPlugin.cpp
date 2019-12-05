#include "KFClinicPlugin.hpp"
#include "KFClinicModule.hpp"
#include "KFConfig/KFConfigInterface.h"
//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
    void KFClinicPlugin::Install()
    {
        __REGISTER_MODULE__( KFClinic );
    }

    void KFClinicPlugin::UnInstall()
    {
        __UN_MODULE__( KFClinic );
    }

    void KFClinicPlugin::LoadModule()
    {
        __FIND_MODULE__( _kf_timer, KFTimerInterface );
        __FIND_MODULE__( _kf_config, KFConfigInterface );
        __FIND_MODULE__( _kf_kernel, KFKernelInterface );
        __FIND_MODULE__( _kf_player, KFPlayerInterface );
        __FIND_MODULE__( _kf_message, KFMessageInterface );
        __FIND_MODULE__( _kf_display, KFDisplayInterface );
        __FIND_MODULE__( _kf_record_client, KFRecordClientInterface );
        __FIND_MODULE__( _kf_execute, KFExecuteInterface );
    }

    void KFClinicPlugin::AddConfig()
    {
        __KF_ADD_CONFIG__( KFClinicConfig );
        __KF_ADD_CONFIG__( KFFormulaConfig );
        __KF_ADD_CONFIG__( KFElementConfig );
    }
}