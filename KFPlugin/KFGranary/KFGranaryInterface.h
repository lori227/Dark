#ifndef __KF_GRANARY_INTERFACE_H__
#define __KF_GRANARY_INTERFACE_H__

#include "KFrame.h"

namespace KFrame
{
    class KFGranaryInterface : public KFModule
    {
    };

    __KF_INTERFACE__( _kf_granary, KFGranaryInterface );
}

#endif