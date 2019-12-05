#ifndef __KF_TECHNOLOGY_INTERFACE_H__
#define __KF_TECHNOLOGY_INTERFACE_H__

#include "KFrame.h"

namespace KFrame
{
    class KFTechnologyInterface : public KFModule
    {
    };

    __KF_INTERFACE__( _kf_technology, KFTechnologyInterface );
}

#endif