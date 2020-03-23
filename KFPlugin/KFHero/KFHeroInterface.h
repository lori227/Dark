#ifndef __KF_HERO_INTERFACE_H__
#define __KF_HERO_INTERFACE_H__

#include "KFKernel/KFEntity.h"

namespace KFrame
{
    class KFHeroInterface : public KFModule
    {
    public:
        // 英雄是否达到最大数量
        virtual bool IsFull( KFEntity* player, KFData* kfherorecord ) = 0;

        // 增加经验
        virtual uint32 AddExp( KFEntity* player, KFData* kfhero, uint32 exp ) = 0;

        // 判断英雄是否达到最大等级
        virtual bool IsMaxLevel( KFEntity* player, KFData* kfhero ) = 0;

        // 获取玩家最大等级
        virtual uint32 GetPlayerMaxLevel( KFEntity* player ) = 0;

        // 计算英雄的最大等级
        virtual uint32 CalcMaxLevel( KFEntity* player, KFData* kfhero ) = 0;

        // 改变hp
        virtual uint32 OperateHp( KFEntity* player, KFData* kfhero, uint32 operate, uint32 hp ) = 0;

        // 添加属性
        virtual uint32 AddHeroData( KFEntity* player, KFData* kfhero, const std::string& name, int32 value ) = 0;

        // 查找存活的英雄
        virtual KFData* FindAliveHero( KFData* kfherorecord, uint64 uuid ) = 0;
    };

    __KF_INTERFACE__( _kf_hero, KFHeroInterface );
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

#endif