#ifndef __KF_EXPLORE_INTERFACE_H__
#define __KF_EXPLORE_INTERFACE_H__

#include "KFrame.h"
#include "KFExploreData.hpp"

namespace KFrame
{
    class KFExploreInterface : public KFModule
    {
    public:
        virtual KFExploreRecord* GetExploreRecord( uint64 keyid ) = 0;
    };

    //////////////////////////////////////////////////////////////////////////
    __KF_INTERFACE__( _kf_explore, KFExploreInterface );
    //////////////////////////////////////////////////////////////////////////
}



#endif