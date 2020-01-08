#ifndef __KF_REWARD_MOUDLE_H__
#define __KF_REWARD_MOUDLE_H__

/************************************************************************
//    @Module			:    奖励配置模块
//    @Author           :    __凌_痕__
//    @QQ				:    7969936
//    @Mail			    :    lori227@qq.com
//    @Date             :    2020-1-1
************************************************************************/

#include "KFRewardInterface.h"
#include "KFRewardConfig.hpp"
#include "KFZConfig/KFElementConfig.h"

namespace KFrame
{
    class KFRewardModule : public KFRewardInterface
    {
    public:
        KFRewardModule() = default;
        ~KFRewardModule() = default;

        // 初始化
        virtual void InitModule();
    };
}



#endif