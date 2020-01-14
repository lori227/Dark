#ifndef __KF_REALM_INTERFACE_H__
#define __KF_REALM_INTERFACE_H__

#include "KFrame.h"
#include "KFRealmData.hpp"

namespace KFrame
{
    class KFRealmInterface : public KFModule
    {
    public:
        virtual KFRealmData* GetRealmData( uint64 keyid ) = 0;
    };

    //////////////////////////////////////////////////////////////////////////
    __KF_INTERFACE__( _kf_realm, KFRealmInterface );
    //////////////////////////////////////////////////////////////////////////
}



#endif