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

        // 获取转职配置
        const KFTransferSetting* FindTransferSetting( uint32 childid, uint32 parentid );

        // 获取转职ID
        uint32 GetTransferId( uint32 childid, uint32 parentid );

        // 通过父职业获取子职业
        uint32 GetRandChildId( uint32 parentid );

        // 通过子职业获取父职业
        uint32 GetRandParentId( uint32 childid );

        // 获得职业的转职等级
        uint32 GetProTransferLevel( uint32 profession );

    protected:
        // 清空配置
        virtual void ClearSetting();

        // 读取配置
        virtual void ReadSetting( KFNode& xmlnode, KFTransferSetting* kfsetting );

        // 加载完成
        virtual void LoadAllComplete();

    private:
        // 职业对应表
        typedef std::unordered_map<uint32, UInt32Vector> TransferMap;

        // 父子表
        TransferMap _parent_child_map;

        // 子父表
        TransferMap _child_parent_map;

        // 职业转职等级
        UInt32Map _transfer_level_map;
    };
}

#endif