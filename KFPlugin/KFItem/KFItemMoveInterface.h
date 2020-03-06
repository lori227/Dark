#ifndef __KF_ITEM_MOVE_INTERFACE_H__
#define __KF_ITEM_MOVE_INTERFACE_H__

#include "KFrame.h"

namespace KFrame
{
    //////////////////////////////////////////////////////////////////////////
    class KFItemMoveInterface : public KFModule
    {
    public:
        // 清空移动包裹
        virtual void CleanItem( KFEntity* player, const std::string& name, bool isauto = false ) = 0;
    };
    __KF_INTERFACE__( _kf_item_move, KFItemMoveInterface );
}

#endif