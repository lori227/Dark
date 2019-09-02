#ifndef __KF_MATCH_CONFIG_H__
#define __KF_MATCH_CONFIG_H__

#include "KFrame.h"
#include "KFZConfig/KFConfig.h"

namespace KFrame
{
    class KFMatchSetting : public KFIntSetting
    {
    public:
    };
    ////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////
    class KFMatchConfig : public KFIntConfigT< KFMatchSetting >, public KFInstance< KFMatchConfig >
    {
    public:
        KFMatchConfig()
        {
            _file_name = "match";
        }

    protected:
        // 读取配置
        void ReadSetting( KFNode& xmlnode, KFMatchSetting* kfsetting );
    };
}

#endif