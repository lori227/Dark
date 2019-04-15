#ifndef __KF_NAME_CONFIG_H__
#define __KF_NAME_CONFIG_H__

#include "KFConfig/KFConfigInterface.h"

namespace KFrame
{
    class KFNameSetting
    {
    public:
        // 名字列表
        std::vector< std::string > _name_list;
    };
    ////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////
    class KFNameConfig : public KFConfig, public KFSingleton< KFNameConfig >
    {
    public:
        KFNameConfig()
        {
            _file = "name.config";
        }

        // 读取配置
        bool LoadConfig();

        // 随机名字
        const std::string& RandName( uint32 id ) const;
    private:
        // 名字列表
        KFHashMap< uint32, uint32, KFNameSetting > _name_setting;
    };

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static auto _kf_name_config = KFNameConfig::Instance();
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

#endif