#ifndef __KF_RECORD_CONFIG_H__
#define __KF_RECORD_CONFIG_H__

#include "KFZConfig/KFConfig.h"

namespace KFrame
{
    class KFRecordSetting : public KFIntSetting
    {
    public:
        // 记录
        std::string _record = "";

        // 新纪录
        std::string _newrecord = "";

        // 最后查询时间
        std::string _onlinetime = "";

        // 新消息提醒
        uint32 _notice = 0u;

        // 是否需要上线更新
        bool _login_update = false;
    };

    class KFRecordConfig : public KFIntConfigT< KFRecordSetting >, public KFInstance< KFRecordConfig >
    {
    public:
        KFRecordConfig()
        {
            _file_name = "record";
        }

    protected:

        // 读取配置
        virtual void ReadSetting( KFNode& xmlnode, KFRecordSetting* kfsetting );
    };
}

#endif