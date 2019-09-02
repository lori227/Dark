#ifndef __KF_BACKGROUND_CONFIG_H__
#define __KF_BACKGROUND_CONFIG_H__

#include "KFZConfig/KFConfig.h"

namespace KFrame
{
    class KFBackGroundSetting : public KFIntSetting
    {
    public:
        // 种族限制
        std::set< uint32 > _race_list;

        // 性别限定
        std::set< uint32 > _sex_list;

        // 是否有效( 被限制 )
        bool IsValid( uint32 race, uint32 sex ) const;
    };
    ////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////
    class KFBackGroundConfig : public KFIntConfigT< KFBackGroundSetting >, public KFInstance< KFBackGroundConfig >
    {
    public:
        KFBackGroundConfig()
        {
            _file_name = "background";
        }

    protected:
        // 读取配置
        virtual void ReadSetting( KFNode& xmlnode, KFBackGroundSetting* kfsetting );
    };
}

#endif