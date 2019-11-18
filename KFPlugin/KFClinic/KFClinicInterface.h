#ifndef __KF_CLINIC_INTERFACE_H__
#define __KF_CLINIC_INTERFACE_H__

#include "KFrame.h"

namespace KFrame
{
    class KFClinicInterface : public KFModule
    {
    };

    __KF_INTERFACE__( _kf_clinic, KFClinicInterface );
}

#endif