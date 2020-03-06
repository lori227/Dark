#ifndef __KF_ITEM_RUNE_INTERFACE_H__
#define __KF_ITEM_RUNE_INTERFACE_H__

#include "KFrame.h"

namespace KFrame
{

    //////////////////////////////////////////////////////////////////////////
    class KFItemRuneInterface : public KFModule
    {
    public:
        // 清空符石槽数据
        virtual void ClearRuneSlotData( KFEntity* player ) = 0;

    };
    __KF_INTERFACE__( _kf_rune, KFItemRuneInterface );
}

#endif