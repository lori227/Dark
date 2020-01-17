#ifndef __KF_DIALOGUE_BRANCH_CONFIG_H__
#define __KF_DIALOGUE_BRANCH_CONFIG_H__

#include "KFZConfig/KFConfig.h"
#include "KFZConfig/KFSetting.h"
#include "KFZConfig/KFElementConfig.h"
#include "KFCore/KFCondition.h"
#include "KFExecuteData.h"

namespace KFrame
{
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    class KFDialogueBranchSetting : public KFIntSetting
    {
    public:
        // 条件
        uint32 _condition_type = 0u;
        UInt32Vector _condition;

        // 消耗
        std::string _str_cost;
        KFElements _cost;

        // 执行功能
        KFExecuteData _execute_data;
    };

    ////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////
    class KFDialogueBranchConfig : public KFConfigT< KFDialogueBranchSetting >, public KFInstance< KFDialogueBranchConfig >
    {
    public:
        KFDialogueBranchConfig()
        {
            _file_name = "dialoguebranch";
        }

        virtual void LoadAllComplete();

    protected:
        // 读取配置
        virtual void ReadSetting( KFNode& xmlnode, KFDialogueBranchSetting* kfsetting );
    };
}

#endif