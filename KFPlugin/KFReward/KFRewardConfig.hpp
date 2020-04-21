#ifndef __KF_REWARD_CONFIG_H__
#define __KF_REWARD_CONFIG_H__

#include "KFCore/KFElement.h"
#include "KFZConfig/KFConfig.h"
#include "KFZConfig/KFSetting.h"

namespace KFrame
{
    class KFRewardSetting : public KFStrSetting
    {
    public:
        // 类型
        uint32 _type;

        // 编码
        uint32 _code;

        // 格式化字串
        std::string _element_template;
    };

    class KFRewardConfig : public KFConfigT< KFRewardSetting >, public KFInstance< KFRewardConfig >
    {
    public:
        KFRewardConfig()
        {
            _file_name = "reward";
        }

        // 获取奖励字符串
        const std::string& StringElement( const std::string& dataname, uint32 datavalue, uint32 dataid );

        // 格式化奖励
        bool FormatElement( KFElements& kfelements, const std::string& dataname, const std::string& datavalue, uint32 dataid );

        // 解析奖励字符串
        bool ParseElement( KFElements& kfelements );

    protected:
        virtual void ClearSetting();

        // 读取配置
        virtual void ReadSetting( KFNode& xmlnode, KFRewardSetting* kfsetting );

        // 保存type和code的对应
        uint64 CalcTypeCodeValue( uint64 type, uint64 code );

        // 格式化字串
        const std::string& FormatElement( uint32 type, uint32 code, uint32 num );

    private:
        std::unordered_map<uint64, std::string> _id_name_list;
    };
}

#endif