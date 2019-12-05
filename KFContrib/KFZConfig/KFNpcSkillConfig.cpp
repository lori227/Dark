#include "KFNpcSkillConfig.hpp"

namespace KFrame
{
    void KFNpcSkillConfig::ReadSetting( KFNode& xmlnode, KFNpcSkillSetting* kfsetting )
    {
        auto strinnate = xmlnode.GetString( "InnateSkill" );
        KFUtility::SplitList( kfsetting->_innate_pool_list, strinnate, __SPLIT_STRING__ );

        auto stractive = xmlnode.GetString( "ActiveSkill" );
        KFUtility::SplitList( kfsetting->_active_pool_list, stractive, __SPLIT_STRING__ );
    }
}