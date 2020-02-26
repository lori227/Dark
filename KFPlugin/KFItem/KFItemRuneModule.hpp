#ifndef __KF_ITEM_RUNE_MOUDLE_H__
#define __KF_ITEM_RUNE_MOUDLE_H__

/************************************************************************
//    @Module			:    符石系统
//    @Author           :    zux
//    @QQ				:    415906519
//    @Mail			    :    415906519@qq.com
//    @Date             :    2020-2-13
************************************************************************/

#include "KFrameEx.h"
#include "KFItemInterface.h"
#include "KFKernel/KFKernelInterface.h"
#include "KFPlayer/KFPlayerInterface.h"
#include "KFMessage/KFMessageInterface.h"
#include "KFOption/KFOptionInterface.h"
#include "KFDisplay/KFDisplayInterface.h"
#include "KFZConfig/KFItemConfig.hpp"
#include "KFZConfig/KFCompoundRuneConfig.hpp"
#include "KFZConfig/KFElementConfig.h"

namespace KFrame
{
    class KFItemRuneModule : public KFItemRuneInterface
    {
    public:
        KFItemRuneModule() = default;
        ~KFItemRuneModule() = default;

        // 逻辑
        virtual void BeforeRun();

        // 关闭
        virtual void BeforeShut();
        ////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////
    protected:
        // 新增符石道具时 回调
        __KF_ADD_DATA_FUNCTION__( OnAddRuneItemCallBack );

    protected:
        // 装备符石
        __KF_MESSAGE_FUNCTION__( HandleRuneSlotMoveReq );

        // 脱下符石
        __KF_MESSAGE_FUNCTION__( HandleRuneTakeOffReq );

    public:
        // 清空符石槽数据
        bool ClearRuneSlotData( KFEntity* player );

    protected:
        // 是否已装备某合成id的符石道具
        KFData* IsHasEquipCompoundId( KFEntity* player, uint32 compoundid );

        // 符石道具转移
        uint32 MoveRuneItem( KFEntity* player, KFData* kfsourcerecord, KFData*  kfsourceitem, KFData* kftargetrecord, uint32 targetindex = 0u );

        // 全部符石道具合成
        bool CompoundAllRune( KFEntity* player );
        bool CompoundRuneByLevel( KFEntity* player, const KFCompoundRuneSetting* setting, uint32 startlevel = 1u );
        bool CompoundRune( KFEntity* player, uint32 inputitemid, uint32 outputitemid );

    private:
        // 玩家上下文组件
        KFComponent* _kf_component = nullptr;
    };
}

#endif