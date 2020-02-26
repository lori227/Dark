#include "KFDialogueConfig.hpp"

namespace KFrame
{
    void KFDialogueConfig::ReadSetting( KFNode& xmlnode, KFDialogueSetting* kfsetting )
    {
        auto strbranch = xmlnode.GetString( "Branch" );
        if ( strbranch != _invalid_string )
        {
            KFUtility::SplitList( kfsetting->_branch, strbranch, __AND_STRING__ );

            // 只保存有分支的序列
            kfsetting->_sequence = xmlnode.GetUInt32( "Sequence", true );
        }
    }
    //////////////////////////////////////////////////////////////////////////////////////////////////
}