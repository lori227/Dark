#ifndef __KF_BUILD_INTERFACE_H__
#define __KF_BUILD_INTERFACE_H__

#include "KFrame.h"

namespace KFrame
{
    class KFBuildInterface : public KFModule
    {
    };

    __KF_INTERFACE__( _kf_build, KFBuildInterface );
}

#endif