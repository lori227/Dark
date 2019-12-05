#ifndef __KF_DIALOGUE_CONFIG_H__
#define __KF_DIALOGUE_CONFIG_H__

#include "KFZConfig/KFConfig.h"
#include "KFZConfig/KFSetting.h"
#include "KFCore/KFElement.h"
/*
#include "KFZConfig/KFRewardConfig.h"
*/

namespace KFrame
{
    class KFDialogueSetting : public KFIntSetting
    {
    public:
        
    };

    class KFDialogueConfig : public KFConfigT< KFDialogueSetting >, public KFInstance< KFDialogueConfig >
    {
    public:
        KFDialogueConfig()
        {
            _file_name = "dialogue";
        }

        virtual void LoadAllComplete();

    protected:
        // 读取配置
        virtual void ReadSetting( KFNode& xmlnode, KFDialogueSetting* kfsetting );
    };
}

#endif