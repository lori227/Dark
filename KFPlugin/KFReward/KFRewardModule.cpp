#include "KFRewardModule.hpp"

namespace KFrame
{
    void KFRewardModule::InitModule()
    {
        KFElementConfig::Instance()->SetParseStringFunction( KFRewardConfig::Instance(), &KFRewardConfig::ParseString );
        KFElementConfig::Instance()->SetIntStringFunction( KFRewardConfig::Instance(), &KFRewardConfig::FormatIntString );
        KFElementConfig::Instance()->SetStrStringFunction( KFRewardConfig::Instance(), &KFRewardConfig::FormatStrString );
        KFElementConfig::Instance()->SetSettingStringFunction( KFRewardConfig::Instance(), &KFRewardConfig::FormatSettingString );
    }
}