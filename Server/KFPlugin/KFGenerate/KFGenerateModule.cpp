#include "KFGenerateModule.hpp"
#include "KFProtocol/KFProtocol.h"


namespace KFrame
{

    void KFGenerateModule::BeforeRun()
    {
        _kf_component = _kf_kernel->FindComponent( __KF_STRING__( player ) );
        _kf_component->SetLogicValue( __KF_STRING__( hero ), __KF_STRING__( head ), KFPinchFaceConfig::Instance()->_settings.Size() );

        _kf_npc_data = _kf_kernel->CreateObject( __KF_STRING__( npc ) );
        __REGISTER_UPDATE_DATA_2__( __KF_STRING__( hero ), __KF_STRING__( level ), &KFGenerateModule::OnHeroLevelUpdate );
    }

    void KFGenerateModule::BeforeShut()
    {
        _kf_kernel->ReleaseObject( _kf_npc_data );
        __UN_UPDATE_DATA_2__( __KF_STRING__( hero ), __KF_STRING__( level ) );
        ///////////////////////////////////////////////////////////////////////////////////////////////////////

    }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    uint32 KFGenerateModule::RandDivisorByType( KFEntity* player, uint32 type, const KFWeightList< KFWeight >* weightpool )
    {
        auto kfobject = player->GetData();
        auto kfdivisorrecord = kfobject->FindData( __KF_STRING__( divisor ) );

        std::vector< const KFDivisorSetting* > outlist;
        for ( auto kfdivisor = kfdivisorrecord->FirstData(); kfdivisor != nullptr; kfdivisor = kfdivisorrecord->NextData() )
        {
            auto id = kfdivisor->GetKeyID();
            auto kfsetting = KFDivisorConfig::Instance()->FindSetting( id );
            if ( kfsetting != nullptr && kfsetting->_type == type )
            {
                // 判断是否在权重列表中
                if ( weightpool == nullptr || weightpool->Have( kfsetting->_value ) )
                {
                    outlist.push_back( kfsetting );
                }
            }
        }

        if ( !outlist.empty() )
        {
            auto index = KFGlobal::Instance()->RandRatio( ( uint32 )outlist.size() );
            auto kfsetting = outlist.at( index );

            // 判断是否生效
            auto rand = KFGlobal::Instance()->RandRatio( KFRandEnum::Hundred );
            if ( rand < kfsetting->_probability )
            {
                return kfsetting->_value;
            }
        }

        return 0u;
    }

    uint32 KFGenerateModule::GenerateRace( KFEntity* player, uint32 racepoolid, bool usedivisor, const char* function, uint32 line )
    {
        auto race = 0u;
        auto racepool = KFWeightConfig::Instance()->FindWeightPool( racepoolid );
        if ( usedivisor )
        {
            // 使用种族偏好因子
            race = RandDivisorByType( player, KFMsg::DivisorOfRace, racepool );
            if ( race != 0u )
            {
                return race;
            }
        }

        // 如果随机因子失败, 继续随机
        if ( racepool != nullptr )
        {
            race = racepool->Rand()->_id;
        }
        else
        {
            __LOG_ERROR_FUNCTION__( function, line, "race pool=[{}] can't find!", racepoolid );
        }

        return race;
    }

    std::tuple< uint32, uint32 > KFGenerateModule::CalcDivisor( KFEntity* player, uint32 race, bool usedivisor )
    {
        auto movetype = 0u;
        auto weapontype = 0u;
        if ( usedivisor )
        {
            movetype = RandDivisorByType( player, KFMsg::DivisorOfMove, nullptr );
            weapontype = RandDivisorByType( player, KFMsg::DivisorOfWeapon, nullptr );
            while ( movetype != 0u || weapontype != 0u )
            {
                // 遍历职业表, 找出满足 race, movetype, weapontype 的职业
                for ( auto& iter : KFProfessionConfig::Instance()->_settings._objects )
                {
                    auto kfsetting = iter.second;
                    if ( kfsetting->IsValid( race, movetype, weapontype ) )
                    {
                        // 有满足要求的, 直接返回, 在后面使用权重随机
                        break;
                    }
                }

                // 没有找到, 随机清除掉一个因子
                if ( movetype != 0u && weapontype != 0u )
                {
                    auto rand = KFGlobal::Instance()->RandRatio( 2 );
                    if ( rand == 0 )
                    {
                        movetype = 0u;
                    }
                    else
                    {
                        weapontype = 0u;
                    }
                }
                else if ( movetype != 0u )
                {
                    movetype = 0u;
                }
                else if ( weapontype != 0u )
                {
                    weapontype = 0u;
                }
            }
        }

        return std::make_tuple( movetype, weapontype );
    }

#define __RAND_WEIGHT_DATA_( poolid, config, isvalid, randid )\
    {\
        auto weightpool = KFWeightConfig::Instance()->FindWeightPool( poolid );\
        if ( weightpool != nullptr )\
        {\
            std::set< uint32 > excludelist; \
            for ( auto weightdata : weightpool->_weight_data )\
            {   \
                auto kfconfigsetting = config->FindSetting( weightdata->_id ); \
                if ( kfconfigsetting == nullptr || !kfconfigsetting->isvalid )\
                {   \
                    excludelist.insert( weightdata->_id ); \
                }\
            }\
            const KFWeight* weightdata = nullptr; \
            if ( excludelist.empty() )\
            {   \
                weightdata = weightpool->Rand(); \
            }\
            else\
            {   \
                weightdata = weightpool->Rand( excludelist ); \
            }\
            if ( weightdata != nullptr )\
            {   \
                randid = weightdata->_id; \
            }\
            else\
            {   \
                __LOG_ERROR_FUNCTION__( function, line, "{} pool=[{}] race=[{}] profession=[{}] move=[{}] weapon=[{}] can't rand weight!", #config, poolid, race, professionid, movetype, weapontype );\
            }\
        }\
        else\
        {\
            __LOG_ERROR_FUNCTION__( function, line, "pool=[{}] can't find!", poolid );\
        }\
    }\

    KFData* KFGenerateModule::GeneratePlayerHero( KFEntity* player, KFData* kfhero, uint32 generateid, bool usedivisor, const char* function, uint32 line )
    {
        auto kfgeneratesetting = KFGenerateConfig::Instance()->FindSetting( generateid );
        if ( kfgeneratesetting == nullptr )
        {
            __LOG_ERROR_FUNCTION__( function, line, "hero generate=[{}] can't find!", generateid );
            return nullptr;
        }
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 随机种族
        auto race = GenerateRace( player, kfgeneratesetting->_race_pool_id, usedivisor, function, line );
        auto kfracesetting = KFRaceConfig::Instance()->FindSetting( race );
        if ( kfracesetting == nullptr )
        {
            __LOG_ERROR_FUNCTION__( function, line, "race setting=[{}] can't find!", race );
            return nullptr;
        }

        kfhero->SetValue( __KF_STRING__( race ), race );
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 如果需要使用权重因子, 计算出 移动方式, 武器类型
        auto movetype = 0u;
        auto weapontype = 0u;
        std::tie( movetype, weapontype ) = CalcDivisor( player, race, usedivisor );
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 随机职业
        auto kfprofession = kfhero->FindData( __KF_STRING__( profession ) );
        auto professionid = kfprofession->_data_setting->_int_init_value;
        __RAND_WEIGHT_DATA_( kfgeneratesetting->_profession_pool_id, KFProfessionConfig::Instance(), IsValid( race, movetype, weapontype ), professionid );
        auto kfprofessionsetting = KFProfessionConfig::Instance()->FindSetting( professionid );
        if ( kfprofessionsetting == nullptr )
        {
            __LOG_ERROR_FUNCTION__( function, line, "profession setting=[{}] can't find!", professionid );
            return nullptr;
        }
        kfprofession->SetValue( professionid );

        // 武器类型
        if ( weapontype == 0u )
        {
            weapontype = kfprofessionsetting->RandWeapontype();
        }
        kfhero->SetValue( __KF_STRING__( weapontype ), weapontype );

        // 武器id
        auto kfequiptypesetting = KFEquipTypeConfig::Instance()->FindSetting( weapontype );
        if ( kfequiptypesetting != nullptr )
        {
            kfhero->SetValue( __KF_STRING__( weaponid ), kfequiptypesetting->_default_id );
        }
        else
        {
            __LOG_ERROR_FUNCTION__( function, line, "weapon type =[{}] can't find setting!", weapontype );
        }
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 随机性别
        uint32 sex = KFMsg::Male;
        auto sexpool = KFWeightConfig::Instance()->FindWeightPool( kfgeneratesetting->_sex_pool_id );
        if ( sexpool != nullptr )
        {
            sex = sexpool->Rand()->_id;
        }
        else
        {
            __LOG_ERROR_FUNCTION__( function, line, "sex pool=[{}] can't find!", kfgeneratesetting->_sex_pool_id );
        }
        kfhero->SetValue( __KF_STRING__( sex ), sex );
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 随机背景
        auto kfbackground = kfhero->FindData( __KF_STRING__( background ) );
        auto backgroundid = kfbackground->_data_setting->_int_init_value;
        __RAND_WEIGHT_DATA_( kfgeneratesetting->_background_pool_id, KFBackGroundConfig::Instance(), IsValid( race, sex ), backgroundid );
        kfbackground->SetValue( backgroundid );
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 随机性格
        auto kfcharacterdata = kfhero->FindData( __KF_STRING__( character ) );
        uint32 characterindex = KFDataDefine::Array_Index;
        for ( auto poolid : kfgeneratesetting->_character_pool_list )
        {
            auto characterid = kfcharacterdata->_data_setting->_int_init_value;
            __RAND_WEIGHT_DATA_( poolid, KFCharacterConfig::Instance(), IsValid( race, backgroundid, professionid ), characterid );
            if ( characterid != 0u )
            {
                auto kfcharacter = kfcharacterdata->FindData( characterindex );
                if ( kfcharacter != nullptr )
                {
                    ++characterindex;
                    kfcharacter->SetValue( characterid );
                }
                else
                {
                    __LOG_ERROR_FUNCTION__( function, line, "character count=[{}] error!", characterindex );
                }
            }
        }
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 随机天赋
        auto kfinnatedata = kfhero->FindData( __KF_STRING__( innate ) );
        uint32 innateindex = KFDataDefine::Array_Index;
        for ( auto poolid : kfgeneratesetting->_innate_pool_list )
        {
            auto innateid = kfinnatedata->_data_setting->_int_init_value;
            __RAND_WEIGHT_DATA_( poolid, KFSkillConfig::Instance(), IsValid( race, professionid, backgroundid, weapontype ), innateid );
            if ( innateid != 0u )
            {
                auto kfinnate = kfinnatedata->FindData( innateindex );
                if ( kfinnate != nullptr )
                {
                    ++innateindex;
                    kfinnate->SetValue( innateid );
                }
                else
                {
                    __LOG_ERROR_FUNCTION__( function, line, "innate count=[{}] error!", innateindex );
                }
            }
        }
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 随机主动技能
        auto kfactivedata = kfhero->FindData( __KF_STRING__( active ) );
        uint32 activeindex = KFDataDefine::Array_Index;
        for ( auto poolid : kfgeneratesetting->_active_pool_list )
        {
            auto activeid = kfactivedata->_data_setting->_int_init_value;
            __RAND_WEIGHT_DATA_( poolid, KFSkillConfig::Instance(), IsValid( race, professionid, backgroundid, weapontype ), activeid );
            if ( activeid != 0u )
            {
                auto kfactive = kfactivedata->FindData( activeindex );
                if ( kfactive != nullptr )
                {
                    ++activeindex;
                    kfactive->SetValue( activeid );
                }
                else
                {
                    __LOG_ERROR_FUNCTION__( function, line, "active count={} error!", activeindex );
                }
            }
        }
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 随机成长率
        auto kfgrowth = kfhero->FindData( __KF_STRING__( growth ) );
        auto kfgrowthsetting = KFHeroAttributeConfig::Instance()->FindSetting( kfgeneratesetting->_growth_id );
        if ( kfgrowthsetting != nullptr )
        {
            for ( auto iter : kfgrowthsetting->_datas._objects )
            {
                auto& name = iter.first;
                auto value = iter.second->GetValue();
                kfgrowth->SetValue( name, value );
            }
        }
        else
        {
            __LOG_ERROR_FUNCTION__( function, line, "attribute id={} can't find!", kfgeneratesetting->_growth_id );
        }

        // 初始属性
        auto kfattributesetting = KFHeroAttributeConfig::Instance()->FindSetting( kfgeneratesetting->_attr_id );
        if ( kfattributesetting != nullptr )
        {
            auto kffighter = kfhero->FindData( __KF_STRING__( fighter ) );
            for ( auto iter : kfattributesetting->_datas._objects )
            {
                auto& name = iter.first;
                auto initvalue = iter.second->GetValue();

                auto growthvalue = kfgrowth->GetValue< uint32 >( name );
                auto racevalue = kfracesetting->GetAttributeValue( name );
                auto professionvalue = kfprofessionsetting->GetAttributeValue( name );

                auto value = CalcHeroInitAttribute( kffighter, initvalue, racevalue, professionvalue, growthvalue );
                kffighter->SetValue( name, value );
            }

            auto maxhp = kffighter->GetValue( __KF_STRING__( maxhp ) );
            kffighter->SetValue( __KF_STRING__( hp ), maxhp );
        }
        else
        {
            __LOG_ERROR_FUNCTION__( function, line, "attribute id={} can't find!", kfgeneratesetting->_attr_id );
        }
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 捏脸数据
        GeneratePinchFace( kfhero );
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        return kfhero;
    }

    void KFGenerateModule::GeneratePinchFace( KFData* kfhero )
    {
        auto kfhead = kfhero->FindData( __KF_STRING__( head ) );
        for ( auto& iter : KFPinchFaceConfig::Instance()->_settings._objects )
        {
            auto kfsetting = iter.second;
            auto kfdata = kfhead->FindData( kfsetting->_id );
            if ( kfdata != nullptr )
            {
                auto value = KFGlobal::Instance()->RandDouble( kfsetting->_min_value, kfsetting->_max_value );
                kfdata->SetValue< int64 >( value * KFRandEnum::TenThousand );
            }
        }
    }

    KFData* KFGenerateModule::GenerateNpcHero( KFEntity* player, uint32 generateid, uint32 level, const char* function, uint32 line )
    {
        auto kfnpcsetting = KFNpcConfig::Instance()->FindSetting( generateid );
        if ( kfnpcsetting == nullptr )
        {
            __LOG_ERROR_FUNCTION__( function, line, "npc generateid=[{}] can't find!", generateid );
            return nullptr;
        }

        auto kfracesetting = KFRaceConfig::Instance()->FindSetting( kfnpcsetting->_race_id );
        if ( kfracesetting == nullptr )
        {
            __LOG_ERROR_FUNCTION__( function, line, "race setting=[{}] can't find!", kfnpcsetting->_race_id );
            return nullptr;
        }

        auto kfprofessionsetting = KFProfessionConfig::Instance()->FindSetting( kfnpcsetting->_profession_id );
        if ( kfprofessionsetting == nullptr )
        {
            __LOG_ERROR_FUNCTION__( function, line, "profession setting=[{}] can't find!", kfnpcsetting->_profession_id );
            return nullptr;
        }


        auto movetype = 0u;
        auto background = 0u;
        auto race = kfnpcsetting->_race_id;
        auto professionid = kfnpcsetting->_profession_id;

        _kf_npc_data->Reset();
        _kf_npc_data->SetKeyID( KFGlobal::Instance()->MakeUUID() );
        _kf_npc_data->SetValue( __KF_STRING__( race ), kfnpcsetting->_race_id );
        _kf_npc_data->SetValue( __KF_STRING__( profession ), kfnpcsetting->_profession_id );
        _kf_npc_data->SetValue( __KF_STRING__( weaponid ), kfnpcsetting->_weapon_id );

        // 升级
        auto npclevel = kfnpcsetting->CalcNpcLevel( level );
        _kf_npc_data->SetValue( __KF_STRING__( level ), npclevel );

        // 性别
        auto sex = kfnpcsetting->_sex;
        if ( sex == KFMsg::UnknowSex )
        {
            sex = KFGlobal::Instance()->RandRange( KFMsg::Male, KFMsg::Female, 1 );
        }
        _kf_npc_data->SetValue( __KF_STRING__( sex ), sex );

        // 随机侵染值
        auto dipweightdata = kfnpcsetting->_rand_dip_list.Rand();
        if ( dipweightdata != nullptr )
        {
            auto dip = KFGlobal::Instance()->RandRange( dipweightdata->_min_value, dipweightdata->_max_value, 1 );
            _kf_npc_data->SetValue( __KF_STRING__( dip ), dip );
        }
        else
        {
            __LOG_ERROR_FUNCTION__( function, line, "npc generateid=[{}] san empty!", generateid );
        }

        // 随机性格
        auto kfcharacterdata = _kf_npc_data->FindData( __KF_STRING__( character ) );
        uint32 characterindex = KFDataDefine::Array_Index;
        for ( auto poolid : kfnpcsetting->_character_pool_list )
        {
            auto weightpool = KFWeightConfig::Instance()->FindWeightPool( poolid );
            if ( weightpool == nullptr )
            {
                __LOG_ERROR_FUNCTION__( function, line, "npc generateid=[{}] character=[{}] no pool!", generateid, poolid );
                continue;
            }

            auto weightdata = weightpool->Rand();
            if ( weightdata == nullptr )
            {
                __LOG_ERROR_FUNCTION__( function, line, "npc generateid=[{}] character=[{}] rand empty!", generateid, poolid );
                continue;
            }

            auto kfcharacter = kfcharacterdata->FindData( characterindex );
            if ( kfcharacter == nullptr )
            {
                __LOG_ERROR_FUNCTION__( function, line, "character count=[{}] error!", characterindex );
                continue;
            }

            ++characterindex;
            kfcharacter->SetValue( weightdata->_id );
        }

        // 技能组id
        auto skillgroupweight = kfnpcsetting->_rand_skill_list.Rand();
        if ( skillgroupweight != nullptr )
        {
            auto kfskillsetting = KFNpcSkillConfig::Instance()->FindSetting( skillgroupweight->_id );
            if ( kfskillsetting != nullptr )
            {
                auto weapontype = 0u;
                auto kfitemsetting = KFItemConfig::Instance()->FindSetting( kfnpcsetting->_weapon_id );
                if ( kfitemsetting != nullptr )
                {
                    weapontype = kfitemsetting->_weapon_type;
                }

                // 随机主动技能
                auto kfactivedata = _kf_npc_data->FindData( __KF_STRING__( active ) );
                uint32 activeindex = KFDataDefine::Array_Index;
                for ( auto poolid : kfskillsetting->_active_pool_list )
                {
                    auto kfactive = kfactivedata->FindData( activeindex );
                    if ( kfactive == nullptr )
                    {
                        __LOG_ERROR_FUNCTION__( function, line, "active count={} error!", activeindex );
                        continue;
                    }

                    auto activeid = 0u;
                    __RAND_WEIGHT_DATA_( poolid, KFSkillConfig::Instance(), IsValid( race, professionid, background, weapontype ), activeid );

                    ++activeindex;
                    kfactive->SetValue( activeid );
                }

                // 天赋技能
                auto kfinnatedata = _kf_npc_data->FindData( __KF_STRING__( innate ) );
                uint32 innateindex = KFDataDefine::Array_Index;
                for ( auto poolid : kfskillsetting->_innate_pool_list )
                {
                    auto kfinnate = kfinnatedata->FindData( innateindex );
                    if ( kfinnate == nullptr )
                    {
                        __LOG_ERROR_FUNCTION__( function, line, "innate count=[{}] error!", innateindex );
                        continue;
                    }

                    auto innateid = 0u;
                    __RAND_WEIGHT_DATA_( poolid, KFSkillConfig::Instance(), IsValid( race, professionid, background, weapontype ), innateid );

                    ++innateindex;
                    kfinnate->SetValue( innateid );
                }
            }
            else
            {
                __LOG_ERROR_FUNCTION__( function, line, "npc generateid=[{}] skillgroup=[{}] no setting!", generateid, skillgroupweight->_id );
            }
        }
        else
        {
            __LOG_ERROR_FUNCTION__( function, line, "npc generateid=[{}] skill empty!", generateid );
        }

        // 属性
        auto kffighter = _kf_npc_data->FindData( __KF_STRING__( fighter ) );
        if ( kfnpcsetting->_attr_id == 0u )
        {
            for ( auto& iter : kfnpcsetting->_attribute )
            {
                kffighter->SetValue( iter.first, iter.second );
            }
        }
        else
        {
            auto kfattributesetting = KFHeroAttributeConfig::Instance()->FindSetting( kfnpcsetting->_attr_id );
            if ( kfattributesetting != nullptr )
            {
                for ( auto iter : kfattributesetting->_datas._objects )
                {
                    auto& name = iter.first;
                    auto initvalue = iter.second->GetValue();

                    // 种族和职业
                    auto racevalue = kfracesetting->GetAttributeValue( name );
                    auto professionvalue = kfprofessionsetting->GetAttributeValue( name );

                    // 初始属性
                    auto value = CalcHeroInitAttribute( kffighter, initvalue, racevalue, professionvalue, 100u );
                    // 等级属性
                    uint64 finalpreference = CalcFinalPreference( racevalue, professionvalue, 100u ) / KFRandEnum::TenThousand;
                    auto levelvalue = ( uint32 )( finalpreference * npclevel );

                    // 保存属性
                    kffighter->SetValue( name, value + levelvalue );
                }
            }
            else
            {
                __LOG_ERROR_FUNCTION__( function, line, "attribute id={} can't find!", kfnpcsetting->_attr_id );
            }
        }

        auto maxhp = kffighter->GetValue<uint32>( __KF_STRING__( maxhp ) );
        kffighter->SetValue( __KF_STRING__( hp ), maxhp );

        // 捏脸数据
        GeneratePinchFace( _kf_npc_data );
        return _kf_npc_data;
    }

    uint64 KFGenerateModule::CalcFinalPreference( uint32 racevalue, uint32 professionvalue, uint32 growthvalue )
    {
        // 最终属性偏好率=（角色_属性偏好率+职业_属性偏好率）*属性成长率
        uint64 value = ( uint64 )( racevalue + professionvalue ) * ( uint64 )growthvalue;
        return value;
    }

    uint32 KFGenerateModule::CalcHeroInitAttribute( KFData* kffighter, uint32 initvalue, uint32 racevalue, uint32 professionvalue, uint32 growthvalue )
    {
        // 直接计算 成长率 x count(9)
        auto value = CalcFinalPreference( racevalue, professionvalue, growthvalue );
        auto totalvalue = ( uint32 )( ( value * kffighter->_data_setting->_int_max_value ) / KFRandEnum::TenThousand );

        // 随机计算 x count(2) 如: 210%  100%+2属性, 另外10%+1额外属性
        value = value / KFRandEnum::Hundred;
        auto addvalue = value / KFRandEnum::Hundred;
        auto randvalue = value % KFRandEnum::Hundred;
        for ( auto i = 0u; i < kffighter->_data_setting->_int_logic_value; ++i )
        {
            totalvalue += addvalue;
            auto rand = KFGlobal::Instance()->RandRatio( KFRandEnum::Hundred );
            if ( rand < randvalue )
            {
                totalvalue += 1u;
            }
        }

        auto finalvalue = totalvalue * initvalue / KFRandEnum::Hundred;
        return finalvalue;
    }

    __KF_UPDATE_DATA_FUNCTION__( KFGenerateModule::OnHeroLevelUpdate )
    {
        // 只有升级才改变经验
        if ( newvalue <= oldvalue )
        {
            return;
        }

        auto kfhero = kfdata->GetParent();
        auto race = kfhero->GetValue<uint32>( __KF_STRING__( race ) );
        auto kfracesetting = KFRaceConfig::Instance()->FindSetting( race );
        if ( kfracesetting == nullptr )
        {
            return __LOG_ERROR__( "race setting=[{}] can't find!", race );
        }

        auto profession = kfhero->GetValue<uint32>( __KF_STRING__( profession ) );
        auto kfprofessionsetting = KFProfessionConfig::Instance()->FindSetting( profession );
        if ( kfprofessionsetting == nullptr )
        {
            return __LOG_ERROR__( "profession setting=[{}] can't find!", profession );
        }

        // 成长率
        auto kfgrowth = kfhero->FindData( __KF_STRING__( growth ) );

        // 战斗属性
        auto kffighter = kfhero->FindData( __KF_STRING__( fighter ) );

        auto level = newvalue - oldvalue;
        for ( auto i = 0u; i < level; ++i )
        {
            // 获得上一次属性
            auto lastlevel = oldvalue + i;
            auto kflevelsetting = KFLevelConfig::Instance()->FindSetting( lastlevel );
            if ( kflevelsetting == nullptr )
            {
                __LOG_ERROR__( "level=[{}] setting can't find!", lastlevel );
                continue;
            }

            for ( auto kfchild = kffighter->FirstData(); kfchild != nullptr; kfchild = kffighter->NextData() )
            {
                if ( kfchild->_data_setting->_name == __KF_STRING__( hp ) )
                {
                    continue;
                }

                auto growthvalue = kfgrowth->GetValue<uint32>( kfchild->_data_setting->_name );
                auto racevalue = kfracesetting->GetAttributeValue( kfchild->_data_setting->_name );
                auto professionvalue = kfprofessionsetting->GetAttributeValue( kfchild->_data_setting->_name );

                // 最终成长率
                auto finalpreference = CalcFinalPreference( racevalue, professionvalue, growthvalue );
                auto value = finalpreference / KFRandEnum::Hundred;

                // 直接添加的属性
                auto addvalue = value / KFRandEnum::Hundred;

                // 需要随机的属性概率
                auto randvalue = value % KFRandEnum::Hundred;

                auto fighterattribute = kfchild->GetValue<uint32>();
                auto totalvalue = ( uint32 )( ( finalpreference * lastlevel ) / KFRandEnum::TenThousand );
                if ( fighterattribute < ( totalvalue - __MIN__( totalvalue, kflevelsetting->_floor_attribute ) ) )
                {
                    // 小于下限, 直接+1
                    addvalue += 1;
                }
                else if ( fighterattribute > ( totalvalue + kflevelsetting->_upper_attribute ) )
                {
                    // 大于上限, 不添加额外的随机属性
                }
                else
                {
                    // 随机是否添加属性
                    auto rand = KFGlobal::Instance()->RandRatio( KFRandEnum::Hundred );
                    if ( rand < randvalue )
                    {
                        addvalue += 1;
                    }
                }

                // 更新属性
                player->UpdateData( kfchild, KFEnum::Add, addvalue );
            }
        }
    }
}