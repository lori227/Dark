#include "KFNpcSkillConfig.hpp"

namespace KFrame
{
    void KFNpcSkillConfig::ReadSetting( KFNode& xmlnode, KFNpcSkillSetting* kfsetting )
    {
        auto strinnate = xmlnode.GetString( "InnateSkill" );
        kfsetting->_innate_pool_list = KFUtility::SplitList< std::list< uint32 > >( strinnate, DEFAULT_SPLIT_STRING );

        auto stractive = xmlnode.GetString( "ActiveSkill" );
        kfsetting->_active_pool_list = KFUtility::SplitList< std::list< uint32 > >( stractive, DEFAULT_SPLIT_STRING );
    }
}