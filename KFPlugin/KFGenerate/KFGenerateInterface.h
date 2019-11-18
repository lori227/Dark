#ifndef __KF_GENERATE_INTERFACE_H__
#define __KF_GENERATE_INTERFACE_H__

#include "KFrame.h"

namespace KFrame
{
    class KFGenerateInterface : public KFModule
    {
    public:
        // 生成玩家英雄
        virtual KFData* GeneratePlayerHero( KFEntity* player, KFData* kfhero, uint32 generateid, bool usedivisor, const char* function, uint32 line ) = 0;

        // 生成npc英雄
        virtual KFData* GenerateNpcHero( KFEntity* player, uint32 generateid, uint32 level, const char* function, uint32 line ) = 0;
    };

    ///////////////////////////////////////////////////////////////////////////////////////////
    __KF_INTERFACE__( _kf_generate, KFGenerateInterface );
    ///////////////////////////////////////////////////////////////////////////////////////////
}



#endif