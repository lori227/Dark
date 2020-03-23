#ifndef __KF_PVE_CONFIG_H__
#define __KF_PVE_CONFIG_H__

#include "KFZConfig/KFConfig.h"
#include "KFZConfig/KFElementConfig.h"
#include "KFZConfig/KFNpcGroupConfig.hpp"
#include "KFZConfig/KFNpcRandConfig.hpp"

namespace KFrame
{
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    class KFPVESetting : public KFIntSetting
    {
    public:
        // npc随机配置
        uint32 _npc_rand_id = 0u;
        const KFNpcRandSetting* _npc_rand_setting = nullptr;

        // 地形随机
        UInt32Vector _dungeon_id_list;

        // 成功道具掉落
        UInt32Vector _victory_drop_list;
        UInt32Vector _inner_victory_drop_list;

        // 失败道具掉落
        UInt32Vector _fail_drop_list;
        UInt32Vector _inner_fail_drop_list;

        // 进入消耗
        std::string _str_consume;
        KFElements _consume;

        // 逃跑成功率
        uint32 _normal_flee_rate = 0u;
        uint32 _inner_flee_rate = 0u;

        // 等级成功率修正
        uint32 _level_coefficient;

        // 逃跑失败惩罚id
        uint32 _punish_id = 0u;

        // 消耗耐久度
        uint32 _durability = 0u;

        // 探索时间配置id
        uint32 _time_id = 0u;

        // 探索天气配置id
        uint32 _weather_id = 0u;

    public:
        // 随机地形id
        uint32 RandDungeonId() const
        {
            auto count = _dungeon_id_list.size();
            if ( count == 0u )
            {
                return 0u;
            }

            auto index = KFGlobal::Instance()->RandRatio( count );
            return _dungeon_id_list.at( index );
        }
    };

    ////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////
    class KFPVEConfig : public KFConfigT< KFPVESetting >, public KFInstance< KFPVEConfig >
    {
    public:
        KFPVEConfig()
        {
            _file_name = "pve";
        }

        virtual void LoadAllComplete();

    protected:
        // 读取配置
        virtual void ReadSetting( KFNode& xmlnode, KFPVESetting* kfsetting );
    };
}

#endif