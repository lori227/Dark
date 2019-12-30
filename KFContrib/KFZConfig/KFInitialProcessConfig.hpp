#ifndef __KF_INITIAL_PROCESS_CONFIG_H__
#define __KF_INITIAL_PROCESS_CONFIG_H__

#include "KFZConfig/KFConfig.h"

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

    protected:
        // 读取配置
        virtual void ReadSetting( KFNode& xmlnode, KFInitialProcessSetting* kfsetting );
    };
}

#endif