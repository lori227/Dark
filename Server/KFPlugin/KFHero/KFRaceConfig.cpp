#include "KFRaceConfig.hpp"

namespace KFrame
{
    /////////////////////////////////////////////////////////////////////////////////
    bool KFRaceConfig::LoadConfig()
    {
        _total_weight = 0u;
        _settings.Clear();
        //////////////////////////////////////////////////////////////////
        KFXml kfxml( _file );
        auto config = kfxml.RootNode();
        auto xmlnode = config.FindNode( "Setting" );
        while ( xmlnode.IsValid() )
        {
            auto race = xmlnode.GetUInt32( "Race" );
            auto kfsetting = _settings.Create( race );

            kfsetting->_race = race;
            kfsetting->_weight = xmlnode.GetUInt32( "Weight" );
            kfsetting->_attr_id = xmlnode.GetUInt32( "AttrPoint" );

            auto strage = xmlnode.GetString( "Age" );
            kfsetting->_min_age = KFUtility::SplitValue< uint32 >( strage, FUNCTION_RANGE_STRING );
            kfsetting->_max_age = KFUtility::SplitValue< uint32 >( strage, FUNCTION_RANGE_STRING );

            // identity
            auto identityline = xmlnode.GetString( "Identity" );
            while ( !identityline.empty() )
            {
                auto stridentity = KFUtility::SplitString( identityline, DEFAULT_SPLIT_STRING );
                if ( !stridentity.empty() )
                {
                    auto identity = KFUtility::SplitValue< uint32 >( stridentity, FUNCTION_RANGE_STRING );
                    auto weight = KFUtility::SplitValue< uint32 >( stridentity, FUNCTION_RANGE_STRING );
                    auto nameid = KFUtility::SplitValue< uint32 >( stridentity, FUNCTION_RANGE_STRING );
                    if ( identity != 0u )
                    {
                        auto data = kfsetting->_identity_list.Create( identity, weight );
                        data->_name_id = nameid;
                    }
                }
            }

            // sex
            auto sexline = xmlnode.GetString( "Sex" );
            while ( !sexline.empty() )
            {
                auto strsex = KFUtility::SplitString( sexline, DEFAULT_SPLIT_STRING );
                if ( !strsex.empty() )
                {
                    auto sex = KFUtility::SplitValue< uint32 >( strsex, FUNCTION_RANGE_STRING );
                    auto weight = KFUtility::SplitValue< uint32 >( strsex, FUNCTION_RANGE_STRING );
                    auto nameid = KFUtility::SplitValue< uint32 >( strsex, FUNCTION_RANGE_STRING );
                    if ( sex != 0u )
                    {
                        auto data = kfsetting->_sex_list.Create( sex, weight );
                        data->_name_id = nameid;
                    }
                }
            }

            // profession
            auto professionline = xmlnode.GetString( "Profession" );
            while ( !professionline.empty() )
            {
                auto strprofession = KFUtility::SplitString( professionline, DEFAULT_SPLIT_STRING );
                if ( !strprofession.empty() )
                {
                    auto profession = KFUtility::SplitValue< uint32 >( strprofession, FUNCTION_RANGE_STRING );
                    auto weight = KFUtility::SplitValue< uint32 >( strprofession, FUNCTION_RANGE_STRING );
                    if ( profession != 0u )
                    {
                        kfsetting->_profession_list.Create( profession, weight );
                    }
                }
            }

            // 天赋技能
            kfsetting->_innate_skill_count = xmlnode.GetUInt32( "InnateCount" );
            auto innateline = xmlnode.GetString( "InnateSkill" );
            while ( !innateline.empty() )
            {
                auto strinnate = KFUtility::SplitString( innateline, DEFAULT_SPLIT_STRING );
                if ( !strinnate.empty() )
                {
                    auto innate = KFUtility::SplitValue< uint32 >( strinnate, FUNCTION_RANGE_STRING );
                    auto weight = KFUtility::SplitValue< uint32 >( strinnate, FUNCTION_RANGE_STRING );
                    if ( innate != 0u )
                    {
                        kfsetting->_innate_list.Create( innate, weight );
                    }
                }
            }

            _total_weight += kfsetting->_weight;
            xmlnode.NextNode();
        }
        //////////////////////////////////////////////////////////////////

        return true;
    }

    /////////////////////////////////////////////////////////////////////////////////
    const KFRaceSetting* KFRaceConfig::RandRace() const
    {
        auto rand = KFGlobal::Instance()->RandRatio( _total_weight );

        auto value = 0u;
        for ( auto& iter : _settings._objects )
        {
            auto kfsetting = iter.second;
            value += kfsetting->_weight;

            if ( rand < value )
            {
                return kfsetting;
            }
        }

        return nullptr;
    }

    const KFRaceSetting* KFRaceConfig::FindRaceSetting( uint32 race ) const
    {
        return _settings.Find( race );
    }
}