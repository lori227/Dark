#ifndef __KF_EXPLORE_CONFIG_H__
#define __KF_EXPLORE_CONFIG_H__

#include "KFZConfig/KFConfig.h"
#include "KFZConfig/KFElementConfig.h"

namespace KFrame
{
    class KFExploreLevel
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
    };

    class KFExploreSeting : public KFIntSetting
    {
    public:
        // 探索层数
        std::unordered_map< uint32, KFExploreLevel > _levels;

        // 进入消耗
        std::string _str_consume;
        KFElements _consume;

    public:
        const KFExploreLevel* FindExploreLevel( uint32 level ) const
        {
            auto iter = _levels.find( level );
            if ( iter == _levels.end() )
            {
                return nullptr;
            }

            return &iter->second;
        }
    };
    ////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////
    class KFExploreConfig : public KFConfigT< KFExploreSeting >, public KFInstance< KFExploreConfig >
    {
    public:
        KFExploreConfig()
        {
            _file_name = "explore";
        }

        virtual void LoadAllComplete();

        // 查找探索
        const KFExploreLevel* FindExploreLevel( uint32 id, uint32 level )
        {
            auto kfsetting = FindSetting( id );
            if ( kfsetting == nullptr )
            {
                return nullptr;
            }

            return kfsetting->FindExploreLevel( level );
        }

    protected:
        // 读取配置
        virtual void ReadSetting( KFNode& xmlnode, KFExploreSeting* kfsetting );
    };
}

#endif