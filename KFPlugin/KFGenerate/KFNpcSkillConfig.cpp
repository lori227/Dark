#include "KFNpcSkillConfig.hpp"

namespace KFrame
{
    void KFNpcSkillConfig::ReadSetting( KFNode& xmlnode, KFNpcSkillSetting* kfsetting )
    {
        auto strinnate = xmlnode.GetString( "InnateSkill" );
        kfsetting->_innate_pool_list = KFUtility::SplitList< VectorUInt32 >( strinnate, __SPLIT_STRING__ );

        auto stractive = xmlnode.GetString( "ActiveSkill" );
        kfsetting->_active_pool_list = KFUtility::SplitList< VectorUInt32 >( stractive, __SPLIT_STRING__ );
    }
}