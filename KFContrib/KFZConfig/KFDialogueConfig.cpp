#include "KFDialogueConfig.hpp"

namespace KFrame
{
    void KFDialogueConfig::ReadSetting( KFNode& xmlnode, KFDialogueSetting* kfsetting )
    {
        kfsetting->_branch = xmlnode.GetUInt32Vector( "Branch" );
        if ( !kfsetting->_branch.empty() )
        {
            // 只保存有分支的序列
            kfsetting->_sequence = xmlnode.GetUInt32( "Sequence", true );
        }
    }
    //////////////////////////////////////////////////////////////////////////////////////////////////
}