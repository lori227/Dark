#ifndef __KF_RECRUIT_INTERFACE_H__
#define __KF_RECRUIT_INTERFACE_H__

#include "KFrame.h"

namespace KFrame
{
    class KFRecruitInterface : public KFModule
    {
    public:
    };

    /////////////////////////////////////////////////////////////////////
    __KF_INTERFACE__( _kf_recruit, KFRecruitInterface );
    /////////////////////////////////////////////////////////////////////
}

#endif