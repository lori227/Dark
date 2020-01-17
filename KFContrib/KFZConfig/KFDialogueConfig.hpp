#ifndef __KF_DIALOGUE_CONFIG_H__
#define __KF_DIALOGUE_CONFIG_H__

#include "KFZConfig/KFConfig.h"

namespace KFrame
{
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    class KFDialogueSetting : public KFIntSetting
    {
    public:
        // 有分支的序列
        uint32 _sequence = 0u;

        // 分支
        UInt32Vector _branch;
    };

    ////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////
    class KFDialogueConfig : public KFConfigT< KFDialogueSetting >, public KFInstance< KFDialogueConfig >
    {
    public:
        KFDialogueConfig()
        {
            _file_name = "dialogue";
            _key_name = "DialogueId";
        }

    protected:
        // 读取配置
        virtual void ReadSetting( KFNode& xmlnode, KFDialogueSetting* kfsetting );
    };
}

#endif