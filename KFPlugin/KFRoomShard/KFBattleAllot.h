#ifndef __KF_BATTLE_ALLOT_H__
#define __KF_BATTLE_ALLOT_H__

#include "KFrameEx.h"
#include "KFRedis/KFRedisInterface.h"

namespace KFrame
{
    // 战场分配器
    class KFBattleAllot : public KFSingleton< KFBattleAllot >
    {
    public:
        // 初始化
        void Initialize( KFRedisDriver* redisdriver );

        // 注册战场
        bool AddBattle( uint64 serverid, const std::string& ip, uint32 port, const std::string& version );

        // 分配战场
        std::tuple< uint64, std::string, uint32, bool > AllotBattle( uint64 serverid, const std::string& version );

        // 删除战场
        void RemoveBattle( uint64 serverid, const std::string& version );

        // 更新战场时间
        void UpdateBattle( uint64 serverid );
    protected:
        // 分配指定战场
        std::tuple< uint64, std::string, uint32, bool > AllotSpecificBattle( uint64 serverid );

        // 分配随机战场
        std::tuple< uint64, std::string, uint32, bool > AllotVersionBattle( const std::string& version );

    private:
        // 保存战场的数据库
        KFRedisDriver* _room_redis = nullptr;
    };

    //////////////////////////////////////////////////////////////////////////////
    static auto _battle_allot = KFBattleAllot::Instance();
    //////////////////////////////////////////////////////////////////////////////
}

#endif