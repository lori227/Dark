#ifndef __KF_TRANSFER_CONFIG_H__
#define __KF_TRANSFER_CONFIG_H__

#include "KFZConfig/KFConfig.h"
#include "KFZConfig/KFSetting.h"
#include "KFZConfig/KFElementConfig.h"

namespace KFrame
{
    class KFTransferSetting : public KFIntSetting
    {
    public:
        // 子转职id
        uint32 _child_id = 0u;

        // 父转职id
        uint32 _parent_id = 0u;

        // 所需等级
        uint32 _level = 0u;

        // 主动技能
        UInt32Vector _active_pool_list;

        // 性格
        UInt32Vector _character_pool_list;

        // 天赋
        UInt32Vector _innate_pool_list;

        // 消耗
        std::string _str_cost;
        KFElements _cost;
    };

    class KFTransferConfig : public KFConfigT< KFTransferSetting >, public KFInstance< KFTransferConfig >
    {
    public:
        KFTransferConfig()
        {
            _file_name = "herotransfer";
        }

        virtual void LoadAllComplete();

        // 获取转职ID
        uint32 GetTransferId( uint32 childid, uint32 parentid );

        // 通过子职业获取父职业
        uint32 GetParentPro( uint32 childpro );

    protected:
        // 读取配置
        virtual void ReadSetting( KFNode& xmlnode, KFTransferSetting* kfsetting );
    };
}

#endif