#include "KFRewardModule.hpp"

namespace KFrame
{
    void KFRewardModule::InitModule()
    {
        KFElementConfig::Instance()->SetParseFunction( KFRewardConfig::Instance(), &KFRewardConfig::ParseElement );
        KFElementConfig::Instance()->SetStringFunction( KFRewardConfig::Instance(), &KFRewardConfig::StringElement );
        KFElementConfig::Instance()->SetFormatFunction( KFRewardConfig::Instance(), &KFRewardConfig::FormatElement );
    }
}