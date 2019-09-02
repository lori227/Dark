﻿#include "KFRecordClientPlugin.hpp"
#include "KFRecordClientModule.hpp"
#include "KFConfig/KFConfigInterface.h"

namespace KFrame
{
    void KFRecordClientPlugin::Install()
    {
        __REGISTER_MODULE__( KFRecordClient );
    }

    void KFRecordClientPlugin::UnInstall()
    {
        __UN_MODULE__( KFRecordClient );
    }

    void KFRecordClientPlugin::LoadModule()
    {
        __FIND_MODULE__( _kf_reset, KFResetInterface );
        __FIND_MODULE__( _kf_notice, KFNoticeInterface );
        __FIND_MODULE__( _kf_display, KFDisplayInterface );
        __FIND_MODULE__( _kf_player, KFPlayerInterface );
        __FIND_MODULE__( _kf_kernel, KFKernelInterface );
        __FIND_MODULE__( _kf_message, KFMessageInterface );
        __FIND_MODULE__( _kf_route, KFRouteClientInterface );
        __FIND_MODULE__( _kf_option, KFOptionInterface );
        __FIND_MODULE__( _kf_config, KFConfigInterface );
    }

    void KFRecordClientPlugin::AddConfig()
    {
        __KF_ADD_CONFIG__( KFRecordConfig );
    }

}