#include "KFGenerateConfig.hpp"

namespace KFrame
{
    void KFGenerateConfig::ReadSetting( KFNode& xmlnode, KFGenerateSetting* kfsetting )
    {
        kfsetting->_race_pool_id = xmlnode.GetUInt32( "RacePool" );
        kfsetting->_sex_pool_id = xmlnode.GetUInt32( "SexPool" );
        kfsetting->_profession_pool_id = xmlnode.GetUInt32( "ProfessionPool" );
        kfsetting->_background_pool_id = xmlnode.GetUInt32( "BackGroundPool" );
        kfsetting->_attr_id = xmlnode.GetUInt32( "AttrId" );
        kfsetting->_quality_pool_id = xmlnode.GetUInt32( "QualityPool" );
        kfsetting->_formula_list = xmlnode.GetUInt32Vector( "FormulaId" );
        kfsetting->_durability = xmlnode.GetUInt32( "RoleDurability" );
        kfsetting->_level = xmlnode.GetUInt32( "Level" );
        kfsetting->_custom = xmlnode.GetUInt32( "Custom" );
        kfsetting->_exp_rate = xmlnode.GetUInt32( "ExpRate" );
        kfsetting->_character_pool_list = xmlnode.GetUInt32Vector( "CharacterPool", true );
        kfsetting->_innate_pool_list = xmlnode.GetUInt32Vector( "InnatePool", true );
        kfsetting->_active_pool_list = xmlnode.GetUInt32Vector( "ActivePool", true );
    }
}