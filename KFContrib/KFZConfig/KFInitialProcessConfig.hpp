#ifndef __KF_INITIAL_PROCESS_CONFIG_H__
#define __KF_INITIAL_PROCESS_CONFIG_H__

#include "KFZConfig/KFConfig.h"
#include "KFZConfig/KFChapterConfig.hpp"
#include "KFZConfig/KFStatusConfig.hpp"

namespace KFrame
{
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    class KFInitialProcessSetting : public KFIntSetting
    {
    public:
        // 执行类型
        uint32 _type = 0u;

        // 执行参数
        uint32 _parameter = 0u;
    };

    ////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////
    class KFInitialProcessConfig : public KFConfigT< KFInitialProcessSetting >, public KFInstance< KFInitialProcessConfig >
    {
    public:
        KFInitialProcessConfig()
        {
            _file_name = "initialprocess";
        }

        virtual void LoadAllComplete();

    protected:
        // 读取配置
        virtual void ReadSetting( KFNode& xmlnode, KFInitialProcessSetting* kfsetting );
    };
}

#endif