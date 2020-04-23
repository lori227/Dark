#include "KFNpcSkillConfig.hpp"

namespace KFrame
{
    void KFNpcSkillConfig::ReadSetting( KFNode& xmlnode, KFNpcSkillSetting* kfsetting )
    {
        kfsetting->_innate_pool_list = xmlnode.GetUInt32Vector( "InnateSkill" );
        kfsetting->_active_pool_list = xmlnode.GetUInt32Vector( "ActiveSkill" );
    }
}