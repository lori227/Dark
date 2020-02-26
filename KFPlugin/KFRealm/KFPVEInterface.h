#ifndef __KF_PVE_INTERFACE_H__
#define __KF_PVE_INTERFACE_H__

#include "KFrame.h"

namespace KFrame
{
    class KFPVEInterface : public KFModule
    {
    public:

        // 操作(里世界)信仰值
        virtual void OperateFaith( KFEntity* player, uint32 operate, uint32 value ) = 0;
    };

    //////////////////////////////////////////////////////////////////////////
    __KF_INTERFACE__( _kf_pve, KFPVEInterface );
    //////////////////////////////////////////////////////////////////////////
}



#endif