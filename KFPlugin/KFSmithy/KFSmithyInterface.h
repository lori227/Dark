#ifndef __KF_SMITHY_INTERFACE_H__
#define __KF_SMITHY_INTERFACE_H__

#include "KFrame.h"

namespace KFrame
{
    class KFSmithyInterface : public KFModule
    {
    };

    __KF_INTERFACE__( _kf_smithy, KFSmithyInterface );
}

#endif