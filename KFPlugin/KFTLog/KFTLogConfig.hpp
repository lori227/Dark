#ifndef __KF_TLOG_CONFIG_H__
#define __KF_TLOG_CONFIG_H__

#include "KFZConfig/KFConfig.h"

namespace KFrame
{
    /////////////////////////////////////////////////////////////////////////////////
    class KFTLogSetting : public KFIntSetting
    {
    public:
        // 日志名
        std::string _log_name;

        // 标识名
        std::string _table_name;

        //
    };

    ////////////////////////////////////////////////////////////////////////////////////
    class KFTLogConfig : public KFConfigT< KFTLogSetting >, public KFInstance< KFTLogConfig >
    {
    public:
        KFTLogConfig()
        {
            _file_name = "tlog";
        }

    protected:
        // 读取配置
        virtual void ReadSetting( KFNode& xmlnode, KFTLogSetting* kfsetting );
    };
}

#endif