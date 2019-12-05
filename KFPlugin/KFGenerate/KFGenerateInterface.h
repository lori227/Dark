﻿#ifndef __KF_GENERATE_INTERFACE_H__
#define __KF_GENERATE_INTERFACE_H__

#include "KFrame.h"

namespace KFrame
{
    class KFDivisorSetting;
    typedef std::list< const KFDivisorSetting* > DivisorList;
    class KFGenerateInterface : public KFModule
    {
    public:
        // 生成玩家英雄
        virtual KFData* GeneratePlayerHero( KFEntity* player, KFData* kfhero, uint32 generateid ) = 0;
        virtual KFData* GeneratePlayerHero( KFEntity* player, KFData* kfhero, uint32 generateid,
                                            const DivisorList& divisorlist, const SetUInt32& professionlist,
                                            uint32 generatelevel, uint32 mingrowth, uint32 maxgrowth ) = 0;

        // 生成npc英雄
        virtual KFData* GenerateNpcHero( KFEntity* player, uint32 generateid, uint32 level ) = 0;
    };

    ///////////////////////////////////////////////////////////////////////////////////////////
    __KF_INTERFACE__( _kf_generate, KFGenerateInterface );
    ///////////////////////////////////////////////////////////////////////////////////////////
}



#endif