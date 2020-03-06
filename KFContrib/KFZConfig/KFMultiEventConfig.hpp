#ifndef __KF_MULTI_OPTION_CONFIG_H__
#define __KF_MULTI_OPTION_CONFIG_H__

#include "KFZConfig/KFConfig.h"
#include "KFCore/KFElement.h"

namespace KFrame
{
    class KFMultiEventData
    {
    public:
        // 花费
        std::string _str_cost;
        KFElements _cost_elements;

        // 事件id
        uint32 _event_id = 0u;
    };

    class KFMultiEventSetting : public KFIntSetting
    {
    public:
        // 选项列表
        KFHashMap<uint32, uint32, KFMultiEventData> _multi_event_data_list;
    };

    ////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////
    class KFMultiEventConfig : public KFConfigT< KFMultiEventSetting >, public KFInstance< KFMultiEventConfig >
    {
    public:
        KFMultiEventConfig()
        {
            _key_name = "MultiId";
            _file_name = "multievent";
        }

        virtual void LoadAllComplete();

    protected:
        // 读取配置
        virtual void ReadSetting( KFNode& xmlnode, KFMultiEventSetting* kfsetting );
    };
}

#endif