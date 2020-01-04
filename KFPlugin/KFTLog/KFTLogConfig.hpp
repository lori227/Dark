#ifndef __KF_TLOG_CONFIG_H__
#define __KF_TLOG_CONFIG_H__

#include "KFZConfig/KFConfig.h"

namespace KFrame
{
    /////////////////////////////////////////////////////////////////////////////////
    class KFTLogSetting : public KFStrSetting
    {
    public:
        // 字符串数值
        std::string _str_value;

        // 整型数值
        uint64 _uint64_value = 0;
    };

    ////////////////////////////////////////////////////////////////////////////////////
    class KFTLogConfig : public KFConfigT< KFTLogSetting >, public KFInstance< KFTLogConfig >
    {
    public:
        KFTLogConfig()
        {
            _file_name = "tlog";
        }

        uint64 GetUInt64( const std::string& name );
        const std::string& GetString( const std::string& name );

    protected:
        // 读取配置
        virtual void ReadSetting( KFNode& xmlnode, KFTLogSetting* kfsetting );
    };
}

#endif