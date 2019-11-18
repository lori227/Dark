#ifndef __KF_CHAPTER_CONFIG_H__
#define __KF_CHAPTER_CONFIG_H__

#include "KFZConfig/KFConfig.h"
#include "KFZConfig/KFTimeConfig.h"

namespace KFrame
{
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    class KFChapterSetting : public KFIntSetting
    {
    public:
        // 默认状态id
        uint32 _init_status_id = 0u;

        // 结束状态id
        uint32 _finish_status_id = 0u;
    };

    ////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////
    class KFChapterConfig : public KFConfigT< KFChapterSetting >, public KFInstance< KFChapterConfig >
    {
    public:
        KFChapterConfig()
        {
            _file_name = "chapter";
        }

    protected:
        // 读取配置
        virtual void ReadSetting( KFNode& xmlnode, KFChapterSetting* kfsetting );
    };
}

#endif