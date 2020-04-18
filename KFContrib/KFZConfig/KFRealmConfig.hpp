#ifndef __KF_REALM_CONFIG_H__
#define __KF_REALM_CONFIG_H__

#include "KFZConfig/KFConfig.h"
#include "KFZConfig/KFElementConfig.h"

namespace KFrame
{
    class KFRealmLevel
    {
    public:
        // 层级
        uint32 _level = 0u;

        // 探索关卡id
        uint32 _explore_id = 0u;

        // 成功道具掉落
        UInt32Vector _victory_drop_list;

        // 失败道具掉落
        UInt32Vector _fail_drop_list;

        // 生成规则
        uint32 _createion = 0u;

        // 里世界类型
        uint32 _inner_world = 0u;

        // 里世界进度参数
        uint32 _inner_parameter = 0u;

        // 粮食消耗
        uint32 _food_num = 0u;

        // 粮食消耗步数
        uint32 _food_step = 0u;

        // 生命消耗
        uint32 _hp_num = 0u;

        // 生命消耗步数
        uint32 _hp_step = 0u;

        // 消耗耐久度
        uint32 _durability = 0u;

        // 探索时间配置id
        uint32 _time_id = 0u;

        // 探索天气配置id
        uint32 _weather_id = 0u;
    };

    class KFRealmSeting : public KFIntSetting
    {
    public:
        // 探索层数
        std::unordered_map< uint32, KFRealmLevel > _levels;

        // 进入消耗
        KFElements _consume;

        // 道具惩罚规则
        UInt32Map _punish_list;
    public:
        const KFRealmLevel* FindRealmLevel( uint32 level ) const
        {
            auto iter = _levels.find( level );
            if ( iter == _levels.end() )
            {
                return nullptr;
            }

            return &iter->second;
        }

        // 惩罚扣除道具万分比
        uint32 GetFunishItemWeight( uint32 itemid ) const
        {
            auto iter = _punish_list.find( itemid );
            if ( iter == _punish_list.end() )
            {
                return KFRandEnum::TenThousand;
            }

            return KFRandEnum::TenThousand - iter->second;
        }
    };
    ////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////
    class KFRealmConfig : public KFConfigT< KFRealmSeting >, public KFInstance< KFRealmConfig >
    {
    public:
        KFRealmConfig()
        {
            _key_name = "RealmId";
            _file_name = "realm";
        }

        virtual void LoadAllComplete();

        // 查找探索
        const KFRealmLevel* FindRealmLevel( uint32 id, uint32 level )
        {
            auto kfsetting = FindSetting( id );
            if ( kfsetting == nullptr )
            {
                return nullptr;
            }

            return kfsetting->FindRealmLevel( level );
        }

    protected:
        // 读取配置
        virtual void ReadSetting( KFNode& xmlnode, KFRealmSeting* kfsetting );
    };
}

#endif