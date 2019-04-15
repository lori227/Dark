#ifndef __KF_ATTR_PROBABILITY_CONFIG_H__
#define __KF_ATTR_PROBABILITY_CONFIG_H__

#include "KFConfig/KFConfigInterface.h"

namespace KFrame
{
    class KFAttrProbabilityData : public KFWeight
    {
    public:
        std::string _data_name;
    };

    class KFAttrProbabilitySetting
    {
    public:
        KFWeightList< KFAttrProbabilityData > _probability_list;
    };
    ////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////
    class KFAttrProbabilityConfig : public KFConfig, public KFSingleton< KFAttrProbabilityConfig >
    {
    public:
        KFAttrProbabilityConfig()
        {
            _file = "attrprobability.config";
        }

        // 读取配置
        bool LoadConfig();

        // 随机名字
        const KFAttrProbabilityData* RandAttribute( uint32 id ) const;
        const KFAttrProbabilitySetting* FindAttrProbabilitySetting( uint32 id ) const;

    private:
        // 英雄列表
        KFHashMap< uint32, uint32, KFAttrProbabilitySetting > _probability_setting;
    };

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static auto _kf_attr_probability_config = KFAttrProbabilityConfig::Instance();
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

#endif