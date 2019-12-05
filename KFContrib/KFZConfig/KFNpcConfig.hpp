#ifndef __KF_NPC_CONFIG_H__
#define __KF_NPC_CONFIG_H__

#include "KFZConfig/KFConfig.h"

namespace KFrame
{
    class KFSanWeight : public KFWeight
    {
    public:
        uint32 _min_value = 0u;
        uint32 _max_value = 0u;
    };
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    class KFNpcSetting : public KFIntSetting
    {
    public:
        // 种族
        uint32 _race_id = 0u;

        // 职业
        uint32 _profession_id = 0u;

        // 武器id
        uint32 _weapon_id = 0u;

        // 性别
        uint32 _sex = 0u;

        // 等级类型
        uint32 _level_type = 0u;
        int32 _level_value = 0u;

        // 随机技能组
        KFWeightList< KFWeight > _rand_skill_list;

        // 性格权重池列表
        VectorUInt32 _character_pool_list;

        // 侵染值随机
        KFWeightList< KFSanWeight > _rand_dip_list;

        // 初始属性偏转率id
        uint32 _attr_id = 0u;

        // 订制属性
        std::unordered_map< std::string, uint32 > _attribute;

    public:
        // 计算npc等级
        uint32 CalcNpcLevel( uint32 level ) const;
    };

    ////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////
    class KFNpcConfig : public KFConfigT< KFNpcSetting >, public KFInstance< KFNpcConfig >
    {
    public:
        KFNpcConfig()
        {
            _file_name = "npc";
        }

    protected:
        // 读取配置
        virtual void ReadSetting( KFNode& xmlnode, KFNpcSetting* kfsetting );
    };
}

#endif