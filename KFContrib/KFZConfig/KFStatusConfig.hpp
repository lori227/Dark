#ifndef __KF_STATUS_CONFIG_H__
#define __KF_STATUS_CONFIG_H__

#include "KFZConfig/KFConfig.h"
#include "KFZConfig/KFExecuteData.h"

namespace KFrame
{
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    class KFStatusSetting : public KFIntSetting
    {
    public:
        // 执行逻辑
        KFExecuteData _execute_data;
    };

    ////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////
    class KFStatusConfig : public KFConfigT< KFStatusSetting >, public KFInstance< KFStatusConfig >
    {
    public:
        KFStatusConfig()
        {
            _file_name = "status";
        }

    protected:
        // 读取配置
        virtual void ReadSetting( KFNode& xmlnode, KFStatusSetting* kfsetting );
    };
}

#endif