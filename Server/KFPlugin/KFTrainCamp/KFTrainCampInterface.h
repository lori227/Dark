#ifndef __KF_TRAIN_CAMP_INTERFACE_H__
#define __KF_TRAIN_CAMP_INTERFACE_H__

#include "KFrame.h"

namespace KFrame
{
    class KFTrainCampInterface : public KFModule
    {
    };

    __KF_INTERFACE__( _kf_train_camp, KFTrainCampInterface );
}

#endif