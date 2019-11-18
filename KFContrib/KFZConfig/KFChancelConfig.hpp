#ifndef __KF_CHANCEL_CONFIG_H__
#define __KF_CHANCEL_CONFIG_H__

#include "KFZConfig/KFConfig.h"
#include "KFCore/KFElement.h"

namespace KFrame
{
    class KFChancelSetting : public KFIntSetting
    {
    public:
        // 类型
        int _type = 0u;

        // 等级
        int _level = 0u;

        // 触发条件
        std::map<uint32, uint32> _condition_map;
    };

    class KFChancelConfig : public KFConfigT< KFChancelSetting >, public KFInstance< KFChancelConfig >
    {
    public:
        KFChancelConfig()
        {
            _file_name = "herochancel";
        }

        // 获取圣坛id
        uint32 GetChancelId( uint32 type, uint32 level );

        // 是否圣坛死亡类型
        bool IsValid( uint32 type );

    protected:
        // 清空配置
        virtual void ClearSetting();

        // 读取配置
        virtual void ReadSetting( KFNode& xmlnode, KFChancelSetting* kfsetting );

    private:
        // 死亡类型列表
        std::set< uint32 > _type_list;
    };
}

#endif