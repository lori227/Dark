#include "KFHeroModule.hpp"
#include "KFRaceConfig.hpp"
#include "KFNameConfig.hpp"
#include "KFQualityConfig.hpp"
#include "KFProfessionConfig.hpp"
#include "KFAttrProbabilityConfig.hpp"

namespace KFrame
{
    void KFHeroModule::InitModule()
    {
        __KF_ADD_CONFIG__( _kf_race_config, true );
        __KF_ADD_CONFIG__( _kf_name_config, true );
        __KF_ADD_CONFIG__( _kf_quality_config, true );
        __KF_ADD_CONFIG__( _kf_profession_config, true );
        __KF_ADD_CONFIG__( _kf_attr_probability_config, true );
    }

    void KFHeroModule::BeforeRun()
    {
        _kf_component = _kf_kernel->FindComponent( __KF_STRING__( player ) );
        _kf_component->RegisterAddElementFunction( __KF_STRING__( hero ), this, &KFHeroModule::AddHeroElement );
        //////////////////////////////////////////////////////////////////////////////////////////////////
    }

    void KFHeroModule::BeforeShut()
    {
        __KF_REMOVE_CONFIG__( _kf_race_config );
        __KF_REMOVE_CONFIG__( _kf_name_config );
        __KF_REMOVE_CONFIG__( _kf_quality_config );
        __KF_REMOVE_CONFIG__( _kf_profession_config );
        __KF_REMOVE_CONFIG__( _kf_attr_probability_config );

        _kf_component->UnRegisterAddElementFunction( __KF_STRING__( hero ) );
        //////////////////////////////////////////////////////////////////////////////////////////////////
    }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_ADD_ELEMENT_FUNCTION__( KFHeroModule::AddHeroElement )
    {
        if ( !kfelement->IsObject() )
        {
            return __LOG_ERROR_FUNCTION__( function, line, "element=[{}] not object!", kfelement->_parent->_data );
        }

        // 创建guid
        auto uuid = KFGlobal::Instance()->MakeUUID( KFMsg::UUidHero );

        // 创建新的英雄
        auto kfhero = _kf_kernel->CreateObject( kfparent->GetDataSetting() );

        auto kfelementobject = reinterpret_cast< KFElementObject* >( kfelement );
        auto rand = kfelementobject->_values.Find( __KF_STRING__( rand ) );
        if ( rand == nullptr )
        {
            // 有设定属性
            for ( auto& iter : kfelementobject->_values._objects )
            {
                auto kfdata = kfhero->FindData( iter.first );
                if ( kfdata != nullptr )
                {
                    auto kfvalue = iter.second;
                    kfdata->SetValue( kfvalue->CalcValue() );
                }
            }
        }
        else
        {
            // 随机种族
            auto kfracesetting = _kf_race_config->RandRace();
            kfhero->SetValue( __KF_STRING__( race ), kfracesetting->_race );

            // 随机身份
            auto kfidentitydata = kfracesetting->_identity_list.Rand();
            kfhero->SetValue( __KF_STRING__( identity ), kfidentitydata->_id );

            // 随机职业
            auto kfprofessiondata = kfracesetting->_profession_list.Rand();
            kfhero->SetValue( __KF_STRING__( profession ), kfprofessiondata->_id );

            // 随机性别
            auto kfsexdata = kfracesetting->_sex_list.Rand();
            kfhero->SetValue( __KF_STRING__( sex ), kfsexdata->_id );

            // 随机名字
            auto& name = RandHeroName( kfidentitydata->_name_id, kfsexdata->_name_id );
            kfhero->SetValue( __KF_STRING__( name ), name );

            // 随机年龄
            auto age = KFGlobal::Instance()->RandRange( kfracesetting->_min_age, kfracesetting->_max_age, 1 );
            kfhero->SetValue( __KF_STRING__( age ), age );

            // 随机品质
            auto kfqualitydata = _kf_quality_config->RandQuality();
            kfhero->SetValue( __KF_STRING__( quality ), kfqualitydata->_quality );

            // 随机属性
            for ( auto i = 0u; i < kfqualitydata->_basic_point; ++i )
            {
                auto kfattrdata = _kf_attr_probability_config->RandAttribute( kfracesetting->_attr_id );
                if ( kfattrdata == nullptr )
                {
                    __LOG_ERROR__( "attrid=[{}] can't rand data!", kfracesetting->_attr_id );
                    continue;
                }

                kfhero->OperateValue( kfattrdata->_data_name, KFEnum::Add, 1 );
            }

            // 天赋技能
            {
                auto kfinnatedata = kfhero->FindData( __KF_STRING__( innate ) );

                auto index = 0u;
                auto& innateskill = kfracesetting->_innate_list.Rand( kfracesetting->_innate_skill_count );
                for ( auto skilldata : innateskill )
                {
                    auto kfskill = kfinnatedata->FindData( index++ );
                    if ( kfskill != nullptr )
                    {
                        kfskill->SetValue( skilldata->_id );
                    }
                }
            }

            // 随机技能
            auto kfprofessonsetting = _kf_profession_config->FindProfessionSetting( kfprofessiondata->_id );
            if ( kfprofessonsetting != nullptr )
            {
                // 随机武器
                auto weapondata = kfprofessonsetting->_weapons.Rand();
                kfhero->SetValue( __KF_STRING__( weapon ), weapondata->_id );

                // 主动技能
                {
                    auto kfskilldata = kfhero->FindData( __KF_STRING__( active ) );

                    auto index = 0u;
                    auto& activeskill = kfprofessonsetting->_active_skills.Rand( kfprofessonsetting->_active_count );
                    for ( auto skilldata : activeskill )
                    {
                        auto kfskill = kfskilldata->FindData( index++ );
                        if ( kfskill != nullptr )
                        {
                            kfskill->SetValue( skilldata->_id );
                        }
                    }
                }

                // 被动技能
                {
                    auto kfskilldata = kfhero->FindData( __KF_STRING__( passivity ) );

                    auto index = 0u;
                    auto& passivityskill = kfprofessonsetting->_passivity_skills.Rand( kfprofessonsetting->_passivity_count );
                    for ( auto skilldata : passivityskill )
                    {
                        auto kfskill = kfskilldata->FindData( index++ );
                        if ( kfskill != nullptr )
                        {
                            kfskill->SetValue( skilldata->_id );
                        }
                    }
                }
            }
            else
            {
                __LOG_ERROR__( "profession=[{}] can't find setting!", kfprofessiondata->_id );
            }
        }

        player->AddData( kfparent, uuid, kfhero );
    }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::string& KFHeroModule::RandHeroName( uint32 firstid, uint32 secondid )
    {
        static std::string _name = "";
        static auto _option = _kf_option->FindOption( __KF_STRING__( joint ) );

        auto firstname = _kf_name_config->RandName( firstid );
        auto secondname = _kf_name_config->RandName( secondid );

        _name = firstname + _option->_str_value + secondname;
        return _name;
    }
}