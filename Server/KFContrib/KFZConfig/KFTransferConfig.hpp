#ifndef __KF_TRANSFER_CONFIG_H__
#define __KF_TRANSFER_CONFIG_H__

#include "KFZConfig/KFConfig.h"
#include "KFCore/KFElement.h"

namespace KFrame
{
    class KFTransferSetting : public KFIntSetting
    {
    public:
        // 转职列表
        std::set< uint32 > _transfer_list;

        // 所需等级
        uint32 _level = 0u;

        // 主动技能
        std::list< uint32 > _active_pool_list;

        // 性格
        std::list< uint32 > _character_pool_list;

        // 天赋
        std::list< uint32 > _innate_pool_list;

        // 消耗
        KFElements _cost;
    };

    class KFTransferConfig : public KFIntConfigT< KFTransferSetting >, public KFInstance< KFTransferConfig >
    {
    public:
        KFTransferConfig()
        {
            _file_name = "herotransfer";
        }

    protected:
        // 读取配置
        virtual void ReadSetting( KFNode& xmlnode, KFTransferSetting* kfsetting );
    };
}

#endif