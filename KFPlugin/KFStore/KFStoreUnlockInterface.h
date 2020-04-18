#ifndef __KF_STORE_UNLOCK_INTERFACE_H__
#define __KF_STORE_UNLOCK_INTERFACE_H__

#include "KFrame.h"

namespace KFrame
{
    class KFStoreUnlockInterface : public KFModule
    {
    public:
        // 判断商店是否激活
        virtual bool IsStoreUnlock( KFEntity* player, uint32 storeid ) = 0;
    };
    ///////////////////////////////////////////////////////////////////////////////////////
    __KF_INTERFACE__( _kf_store_unlock, KFStoreUnlockInterface );
    ///////////////////////////////////////////////////////////////////////////////////////
}

#endif