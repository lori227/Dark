#include "KFProfessionConfig.hpp"

namespace KFrame
{
    /////////////////////////////////////////////////////////////////////////////////
    bool KFProfessionConfig::LoadConfig()
    {
        _settings.Clear();
        //////////////////////////////////////////////////////////////////
        KFXml kfxml( _file );
        auto config = kfxml.RootNode();
        auto xmlnode = config.FindNode( "Setting" );
        while ( xmlnode.IsValid() )
        {
            auto profession = xmlnode.GetUInt32( "Profession" );
            auto kfsetting = _settings.Create( profession );

            kfsetting->_profession = profession;
            kfsetting->_attr_id = xmlnode.GetUInt32( "AttrPoint" );
            kfsetting->_active_count = xmlnode.GetUInt32( "ActiveCount" );
            kfsetting->_passivity_count = xmlnode.GetUInt32( "PassivityCount" );

            // 武器列表
            auto weaponline = xmlnode.GetString( "Weapon" );
            while ( !weaponline.empty() )
            {
                auto strweapon = KFUtility::SplitString( weaponline, DEFAULT_SPLIT_STRING );
                if ( !strweapon.empty() )
                {
                    auto id = KFUtility::SplitValue< uint32 >( strweapon, FUNCTION_RANGE_STRING );
                    auto weight = KFUtility::SplitValue< uint32 >( strweapon, FUNCTION_RANGE_STRING );
                    if ( id != 0u )
                    {
                        kfsetting->_weapons.Create( id, weight );
                    }
                }
            }

            // active skill
            auto activeskillline = xmlnode.GetString( "ActiveSkill" );
            while ( !activeskillline.empty() )
            {
                auto stractiveskill = KFUtility::SplitString( activeskillline, DEFAULT_SPLIT_STRING );
                if ( !stractiveskill.empty() )
                {
                    auto id = KFUtility::SplitValue< uint32 >( stractiveskill, FUNCTION_RANGE_STRING );
                    auto weight = KFUtility::SplitValue< uint32 >( stractiveskill, FUNCTION_RANGE_STRING );
                    if ( id != 0u )
                    {
                        kfsetting->_active_skills.Create( id, weight );
                    }
                }
            }

            // passivity skill
            auto passivitykillline = xmlnode.GetString( "PassivitySkill" );
            while ( !passivitykillline.empty() )
            {
                auto strpassivityskill = KFUtility::SplitString( passivitykillline, DEFAULT_SPLIT_STRING );
                if ( !strpassivityskill.empty() )
                {
                    auto id = KFUtility::SplitValue< uint32 >( strpassivityskill, FUNCTION_RANGE_STRING );
                    auto weight = KFUtility::SplitValue< uint32 >( strpassivityskill, FUNCTION_RANGE_STRING );
                    if ( id != 0u )
                    {
                        kfsetting->_passivity_skills.Create( id, weight );
                    }
                }
            }
            xmlnode.NextNode();
        }
        //////////////////////////////////////////////////////////////////

        return true;
    }

    /////////////////////////////////////////////////////////////////////////////////
    const KFProfessionSetting* KFProfessionConfig::FindProfessionSetting( uint32 profession ) const
    {
        return _settings.Find( profession );
    }
}