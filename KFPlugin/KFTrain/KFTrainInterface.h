#ifndef __KF_TRAIN_INTERFACE_H__
#define __KF_TRAIN_INTERFACE_H__

#include "KFrame.h"

namespace KFrame
{
    class KFTrainInterface : public KFModule
    {
    };

    __KF_INTERFACE__( _kf_train, KFTrainInterface );
}

#endif