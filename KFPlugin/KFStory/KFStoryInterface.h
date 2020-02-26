#ifndef __KF_STORY_INTERFACE_H__
#define __KF_STORY_INTERFACE_H__

#include "KFrame.h"

namespace KFrame
{
    class KFStoryInterface : public KFModule
    {
    public:
        // 添加剧情
        virtual void AddStory( KFEntity* player, uint32 storyid ) = 0;
    };

    __KF_INTERFACE__( _kf_story, KFStoryInterface );
}

#endif