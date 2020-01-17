#ifndef __KF_STORY_INTERFACE_H__
#define __KF_STORY_INTERFACE_H__

#include "KFrame.h"

namespace KFrame
{
    class KFStoryInterface : public KFModule
    {
    };

    __KF_INTERFACE__( _kf_story, KFStoryInterface );
}

#endif