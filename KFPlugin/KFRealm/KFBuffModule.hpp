#ifndef __KF_BUFF_MOUDLE_H__
#define __KF_BUFF_MOUDLE_H__

/************************************************************************
//    @Module			:    探索buff系统
//    @Author           :    __凌_痕__
//    @QQ				:    7969936
//    @Mail			    :    lori227@qq.com
//    @Date             :    2020-3-10
************************************************************************/

#include "KFrameEx.h"
#include "KFRealmData.hpp"
#include "KFBuffInterface.h"
#include "KFPVEInterface.h"
#include "KFRealmInterface.h"
#include "KFHero/KFHeroInterface.h"
#include "KFHero/KFHeroTeamInterface.h"
#include "KFDrop/KFDropInterface.h"
#include "KFKernel/KFKernelInterface.h"
#include "KFPlayer/KFPlayerInterface.h"
#include "KFMessage/KFMessageInterface.h"

namespace KFrame
{
    ///////////////////////////////////////////////////////////////////////////////////////
    class KFBuffModule : public KFBuffInterface
    {
    public:
        KFBuffModule() = default;
        ~KFBuffModule() = default;

        // 逻辑
        virtual void BeforeRun();

        // 关闭
        virtual void BeforeShut();

    protected:
        //////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////
        // 处理添加buff
        __KF_MESSAGE_FUNCTION__( HandleAddTeamBuffReq );

        // 处理删除buff
        __KF_MESSAGE_FUNCTION__( HandleRemoveTeamBuffReq );
    protected:
        // 掉落增加英雄buff
        __KF_DROP_LOGIC_FUNCTION__( OnDropHeroAddBuff );

        // 掉落减少英雄buff
        __KF_DROP_LOGIC_FUNCTION__( OnDropHeroDecBuff );

        // pve战斗开始结算
        __KF_PVE_START_FUNCTION__( OnPVEStartBuffModule );
    protected:
        //////////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////
        // 添加队伍buff
        void AddTeamBuff( KFRealmData* kfrealmdata,  uint32 buffid, uint32 count );

        // 删除队伍buff
        void RemoveTeamBuff( KFRealmData* kfrealmdata, uint32 buffid, uint32 count );

        // 添加英雄buff
        void AddBuffData( KFRealmData* kfrealmdata, uint64 uuid, uint32 value );

        // 减少英雄buff
        void RemoveBuffData( KFRealmData* kfrealmdata, uint64 uuid, uint32 value );

        // 更新buff到客户端
        void SendBuffMessage( KFEntity* player, KFRealmData* kfrealmdata );

        // 添加显示的buff
        void AddBuffToShow( KFEntity* player, const std::string& logicname, uint32 buffid, uint32 count );
    };
}

#endif