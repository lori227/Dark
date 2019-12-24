#ifndef __KF_ICON_CONFIG_H__
#define __KF_ICON_CONFIG_H__

#include "KFZConfig/KFConfig.h"
#include "KFZConfig/KFSetting.h"

namespace KFrame
{
    class KFIconSetting : public KFIntSetting
    {
    public:
        // 默认标记
        bool _default = false;

    };

    class KFIconConfig : public KFConfigT< KFIconSetting >, public KFInstance< KFIconConfig >
    {
    public:
        KFIconConfig()
        {
            _file_name = "icon";
        }

        KFIconSetting* GetDedaultIconSetting()
        {
            return _dedaulticon;
        }

    protected:
        // 读取配置
        virtual void ReadSetting( KFNode& xmlnode, KFIconSetting* kfsetting )
        {
            kfsetting->_default = xmlnode.GetBoolen( "Default", true );
            if ( kfsetting->_default && _dedaulticon == nullptr )
            {
                _dedaulticon = kfsetting;
            }
        }

    protected:
        // 默认头像id
        KFIconSetting* _dedaulticon;
    };
}

#endif