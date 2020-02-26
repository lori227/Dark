#ifndef __KF_STORY_CONFIG_H__
#define __KF_STORY_CONFIG_H__

#include "KFZConfig/KFConfig.h"
#include "KFZConfig/KFChapterConfig.hpp"
#include "KFZConfig/KFStatusConfig.hpp"

namespace KFrame
{
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    class KFStorySequence
    {
    public:
        // 序列
        uint32 _sequence = 0u;

        // 执行类型
        uint32 _type = 0u;

        // 执行参数1
        uint32 _parameter1 = 0u;

        // 执行参数2
        uint32 _parameter2 = 0u;
    };

    class KFStorySetting : public KFIntSetting
    {
    public:
        // 触发类型
        uint32 _trigger_type = 0u;

        // 剧情序列
        std::unordered_map< uint32, KFStorySequence > _sequences;

    public:
        const KFStorySequence* FindStorySequence( uint32 sequence ) const
        {
            auto iter = _sequences.find( sequence );
            if ( iter == _sequences.end() )
            {
                return nullptr;
            }

            return &iter->second;
        }
    };

    ////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////
    class KFStoryConfig : public KFConfigT< KFStorySetting >, public KFInstance< KFStoryConfig >
    {
    public:
        KFStoryConfig()
        {
            _file_name = "story";
            _key_name = "StoryId";
        }

        const KFStorySequence* FindStorySequence( uint32 storyid, uint32 sequence )
        {
            auto kfsetting = FindSetting( storyid );
            if ( kfsetting == nullptr )
            {
                return nullptr;
            }

            return kfsetting->FindStorySequence( sequence );
        }

        virtual void LoadAllComplete();

    protected:
        // 读取配置
        virtual void ReadSetting( KFNode& xmlnode, KFStorySetting* kfsetting );
    };
}

#endif