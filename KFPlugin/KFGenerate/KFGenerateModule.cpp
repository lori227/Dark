#include "KFGenerateModule.hpp"
#include "KFProtocol/KFProtocol.h"


namespace KFrame
{
    void KFGenerateModule::BeforeRun()
    {
        _kf_component = _kf_kernel->FindComponent( __STRING__( player ) );
        __REGISTER_UPDATE_DATA_2__( __STRING__( hero ), __STRING__( level ), &KFGenerateModule::OnHeroLevelUpdate );
        __REGISTER_UPDATE_DATA_2__( __STRING__( hero ), __STRING__( profession ), &KFGenerateModule::OnHeroProfessionUpdate );
    }

    void KFGenerateModule::BeforeShut()
    {
        __UN_UPDATE_DATA_2__( __STRING__( hero ), __STRING__( level ) );
        __UN_UPDATE_DATA_2__( __STRING__( hero ), __STRING__( profession ) );
        ///////////////////////////////////////////////////////////////////////////////////////////////////////

    }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    uint32 KFGenerateModule::RandDivisorByType( KFEntity* player, uint32 type, const KFWeightList< KFWeight >* weightpool )
    {
        auto kfdivisorrecord = player->Find( __STRING__( divisor ) );

        std::vector< const KFDivisorSetting* > outlist;
        for ( auto kfdivisor = kfdivisorrecord->First(); kfdivisor != nullptr; kfdivisor = kfdivisorrecord->Next() )
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

    std::tuple< uint32, uint32 > KFGenerateModule::CalcDivisor( KFEntity* player, uint32 race, uint32 sex, uint32 professionpoolid )
    {
        auto weightpool = KFWeightConfig::Instance()->FindWeightPool( professionpoolid );
        if ( weightpool == nullptr )
        {
            return std::make_tuple( 0u, 0u );
        }

        auto movetype = RandDivisorByType( player, KFMsg::DivisorOfMove, nullptr );
        auto weapontype = RandDivisorByType( player, KFMsg::DivisorOfWeapon, nullptr );
        while ( movetype != 0u || weapontype != 0u )
        {
            // 遍历权重表, 找出满足 race, movetype, weapontype 的职业
            for ( auto weightdata : weightpool->_weight_data )
            {
                if ( weightdata->_id == 0u || weightdata->_weight == 0u )
                {
                    continue;
                }

                auto kfsetting = KFProfessionConfig::Instance()->FindSetting( weightdata->_id );
                if ( kfsetting != nullptr && kfsetting->IsValid( race, sex, movetype, weapontype ) )
                {
                    // 有满足要求的, 直接返回, 在后面使用权重随机
                    return std::make_tuple( movetype, weapontype );
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

        return std::make_tuple( movetype, weapontype );
    }

#define __RAND_WEIGHT_DATA__( poolid, config, isvalid, excludelist, randid )\
    {\
        auto weightpool = KFWeightConfig::Instance()->FindWeightPool( poolid );\
        if ( weightpool != nullptr )\
        {\
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
                __LOG_ERROR_FUNCTION__( function, line, "{} pool=[{}] race=[{}] sex=[{}] profession=[{}] move=[{}] weapon=[{}] can't rand weight!", \
                                        #config, poolid, race, sex, professionid, movetype, weapontype );\
            }\
        }\
        else\
        {\
            __LOG_ERROR_FUNCTION__( function, line, "pool=[{}] can't find!", poolid );\
        }\
    }\

#define __RAND_WEIGHT_DATA_CLEAR__( poolid, config, isvalid, excludelist, randid )\
    {\
        excludelist.clear();\
        __RAND_WEIGHT_DATA__( poolid, config, isvalid, excludelist, randid );\
    }

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
        kfhero->Set( __STRING__( race ), race );
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        auto sex = kfracesetting->_sex;
        if ( sex == KFMsg::UnknowSex )
        {
            auto sexdata = KFWeightConfig::Instance()->RandWeight( kfgeneratesetting->_sex_pool_id );
            if ( sexdata != nullptr )
            {
                sex = sexdata->_id;
            }
            else
            {
                __LOG_ERROR_FUNCTION__( function, line, "sex pool=[{}] can't find!", kfgeneratesetting->_sex_pool_id );
            }
        }
        kfhero->Set( __STRING__( sex ), sex );
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 如果需要使用权重因子, 计算出 移动方式, 武器类型
        auto movetype = 0u;
        auto weapontype = 0u;
        if ( usedivisor )
        {
            std::tie( movetype, weapontype ) = CalcDivisor( player, race, sex, kfgeneratesetting->_profession_pool_id );
        }
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 随机职业
        auto kfprofession = kfhero->Find( __STRING__( profession ) );
        auto professionid = kfprofession->_data_setting->_int_init_value;

        std::set< uint32 > excludelist;
        __RAND_WEIGHT_DATA_CLEAR__( kfgeneratesetting->_profession_pool_id, KFProfessionConfig::Instance(), IsValid( race, sex, movetype, weapontype ), excludelist, professionid );
        auto kfprofessionsetting = KFProfessionConfig::Instance()->FindSetting( professionid );
        if ( kfprofessionsetting == nullptr )
        {
            __LOG_ERROR_FUNCTION__( function, line, "profession setting=[{}] can't find!", professionid );
            return nullptr;
        }
        kfprofession->Set( professionid );

        // 移动类型
        kfhero->Set( __STRING__( movetype ), kfprofessionsetting->_move_type );

        // 武器类型
        if ( weapontype == 0u )
        {
            weapontype = kfprofessionsetting->RandWeapontype();
        }
        kfhero->Set( __STRING__( weapontype ), weapontype );
        auto kfweapontypesetting = KFWeaponTypeConfig::Instance()->FindSetting( weapontype );
        if ( kfweapontypesetting != nullptr )
        {
            auto kfweapon = kfhero->Find( __STRING__( weapon ) );
            _kf_item->CreateItem( player, kfweapontypesetting->_default_weapon_id, kfweapon, __FUNC_LINE__ );
        }
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 随机背景
        auto kfbackground = kfhero->Find( __STRING__( background ) );
        auto backgroundid = kfbackground->_data_setting->_int_init_value;

        __RAND_WEIGHT_DATA_CLEAR__( kfgeneratesetting->_background_pool_id, KFBackGroundConfig::Instance(), IsValid( race, sex ), excludelist, backgroundid );
        kfbackground->Set( backgroundid );
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 随机性格
        auto kfcharacterdata = kfhero->Find( __STRING__( character ) );
        uint32 characterindex = KFDataDefine::Array_Index;
        for ( auto poolid : kfgeneratesetting->_character_pool_list )
        {
            auto characterid = kfcharacterdata->_data_setting->_int_init_value;

            __RAND_WEIGHT_DATA_CLEAR__( poolid, KFCharacterConfig::Instance(), IsValid( race, backgroundid, professionid ), excludelist, characterid );
            if ( characterid != 0u )
            {
                auto kfcharacter = kfcharacterdata->Find( characterindex );
                if ( kfcharacter != nullptr )
                {
                    ++characterindex;
                    kfcharacter->Set( characterid );
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
        auto kfinnatedata = kfhero->Find( __STRING__( innate ) );
        uint32 innateindex = KFDataDefine::Array_Index;
        for ( auto poolid : kfgeneratesetting->_innate_pool_list )
        {
            auto innateid = kfinnatedata->_data_setting->_int_init_value;

            __RAND_WEIGHT_DATA_CLEAR__( poolid, KFSkillConfig::Instance(), IsValid( race, professionid, backgroundid, weapontype ), excludelist, innateid );
            if ( innateid != 0u )
            {
                auto kfinnate = kfinnatedata->Find( innateindex );
                if ( kfinnate != nullptr )
                {
                    ++innateindex;
                    kfinnate->Set( innateid );
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
        auto kfactivedata = kfhero->Find( __STRING__( active ) );
        uint32 activeindex = KFDataDefine::Array_Index;
        for ( auto poolid : kfgeneratesetting->_active_pool_list )
        {
            auto activeid = kfactivedata->_data_setting->_int_init_value;

            __RAND_WEIGHT_DATA_CLEAR__( poolid, KFSkillConfig::Instance(), IsValid( race, professionid, backgroundid, weapontype ), excludelist, activeid );
            if ( activeid != 0u )
            {
                auto kfactive = kfactivedata->Find( activeindex );
                if ( kfactive != nullptr )
                {
                    ++activeindex;
                    kfactive->Set( activeid );
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
        auto kfgrowth = kfhero->Find( __STRING__( growth ) );
        auto kfgrowthsetting = KFHeroAttributeConfig::Instance()->FindSetting( kfgeneratesetting->_growth_id );
        if ( kfgrowthsetting != nullptr )
        {
            for ( auto iter : kfgrowthsetting->_datas._objects )
            {
                auto& name = iter.first;
                auto value = iter.second->GetValue();
                kfgrowth->Set( name, value );
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
            auto kffighter = kfhero->Find( __STRING__( fighter ) );
            for ( auto iter : kfattributesetting->_datas._objects )
            {
                auto& name = iter.first;
                auto initvalue = iter.second->GetValue();

                auto growthvalue = kfgrowth->Get< uint32 >( name );
                auto racevalue = kfracesetting->GetAttributeValue( name );
                auto professionvalue = kfprofessionsetting->GetAttributeValue( name );

                auto value = CalcHeroInitAttribute( kffighter, initvalue, racevalue, professionvalue, growthvalue );
                kffighter->Set( name, value );
            }

            auto maxhp = kffighter->Get( __STRING__( maxhp ) );
            kffighter->Set( __STRING__( hp ), maxhp );
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
        auto kfhead = kfhero->Find( __STRING__( head ) );

        _kf_kernel->InitArray( kfhead, KFPinchFaceConfig::Instance()->_settings.Size() );
        for ( auto& iter : KFPinchFaceConfig::Instance()->_settings._objects )
        {
            auto kfsetting = iter.second;

            auto kfdata = kfhead->Find( kfsetting->_id );
            if ( kfdata != nullptr )
            {
                auto value = KFGlobal::Instance()->RandDouble( kfsetting->_min_value, kfsetting->_max_value );
                kfdata->Set< int64 >( value * KFRandEnum::TenThousand );
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

        auto kfnpc = _kf_kernel->CreateObject( __STRING__( npc ) );
        kfnpc->Set( __STRING__( id ), generateid );
        kfnpc->SetKeyID( KFGlobal::Instance()->STMakeUUID() );
        kfnpc->Set( __STRING__( race ), kfnpcsetting->_race_id );
        kfnpc->Set( __STRING__( profession ), kfnpcsetting->_profession_id );
        kfnpc->Set( __STRING__( movetype ), kfprofessionsetting->_move_type );

        // 武器
        auto kfweapon = kfnpc->Find( __STRING__( weapon ) );
        _kf_item->CreateItem( player, kfnpcsetting->_weapon_id, kfweapon, __FUNC_LINE__ );
        auto kfitemsetting = KFItemConfig::Instance()->FindSetting( kfnpcsetting->_weapon_id );
        if ( kfitemsetting != nullptr )
        {
            kfnpc->Set( __STRING__( weapontype ), kfitemsetting->_weapon_type );
        }

        // 等级
        level = kfnpcsetting->CalcNpcLevel( level );
        kfnpc->Set( __STRING__( level ), level );

        // 性别
        auto sex = kfnpcsetting->_sex;
        if ( sex == KFMsg::UnknowSex )
        {
            sex = kfracesetting->_sex;
            if ( sex == KFMsg::UnknowSex )
            {
                sex = kfprofessionsetting->_sex_limit;
                if ( sex == KFMsg::UnknowSex )
                {
                    sex = KFGlobal::Instance()->RandRange( KFMsg::Male, KFMsg::Female, 1 );
                }
            }
        }
        kfnpc->Set( __STRING__( sex ), sex );

        // 随机侵染值
        auto dipweightdata = kfnpcsetting->_rand_dip_list.Rand();
        if ( dipweightdata != nullptr )
        {
            auto dip = KFGlobal::Instance()->RandRange( dipweightdata->_min_value, dipweightdata->_max_value, 1 );
            kfnpc->Set( __STRING__( dip ), dip );
        }
        else
        {
            __LOG_ERROR_FUNCTION__( function, line, "npc generateid=[{}] san empty!", generateid );
        }

        // 随机性格
        auto kfcharacterdata = kfnpc->Find( __STRING__( character ) );
        uint32 characterindex = KFDataDefine::Array_Index;
        for ( auto poolid : kfnpcsetting->_character_pool_list )
        {
            auto weightdata = KFWeightConfig::Instance()->RandWeight( poolid );
            if ( weightdata == nullptr )
            {
                __LOG_ERROR_FUNCTION__( function, line, "npc generateid=[{}] character=[{}] rand empty!", generateid, poolid );
                continue;
            }

            auto kfcharacter = kfcharacterdata->Find( characterindex );
            if ( kfcharacter == nullptr )
            {
                __LOG_ERROR_FUNCTION__( function, line, "character count=[{}] error!", characterindex );
                continue;
            }

            ++characterindex;
            kfcharacter->Set( weightdata->_id );
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
                    if ( weapontype == 0u )
                    {
                        __LOG_ERROR__( "weapon=[{}] weapontype = 0!", kfnpcsetting->_weapon_id );
                    }
                }
                else
                {
                    __LOG_ERROR__( "weapon=[{}] can't find setting!", kfnpcsetting->_weapon_id );
                }

                // 随机主动技能
                auto kfactivedata = kfnpc->Find( __STRING__( active ) );
                uint32 activeindex = KFDataDefine::Array_Index;
                for ( auto poolid : kfskillsetting->_active_pool_list )
                {
                    auto kfactive = kfactivedata->Find( activeindex );
                    if ( kfactive == nullptr )
                    {
                        __LOG_ERROR_FUNCTION__( function, line, "active count={} error!", activeindex );
                        continue;
                    }

                    auto activeid = 0u;
                    std::set<uint32> excludelist;
                    __RAND_WEIGHT_DATA_CLEAR__( poolid, KFSkillConfig::Instance(), IsValid( race, professionid, background, weapontype ), excludelist, activeid );

                    ++activeindex;
                    kfactive->Set( activeid );
                }

                // 天赋技能
                auto kfinnatedata = kfnpc->Find( __STRING__( innate ) );
                uint32 innateindex = KFDataDefine::Array_Index;
                for ( auto poolid : kfskillsetting->_innate_pool_list )
                {
                    auto kfinnate = kfinnatedata->Find( innateindex );
                    if ( kfinnate == nullptr )
                    {
                        __LOG_ERROR_FUNCTION__( function, line, "innate count=[{}] error!", innateindex );
                        continue;
                    }

                    auto innateid = 0u;
                    std::set<uint32> excludelist;
                    __RAND_WEIGHT_DATA_CLEAR__( poolid, KFSkillConfig::Instance(), IsValid( race, professionid, background, weapontype ), excludelist, innateid );

                    ++innateindex;
                    kfinnate->Set( innateid );
                }
            }
            else
            {
                __LOG_ERROR_FUNCTION__( function, line, "npc generateid=[{}] skillgroup=[{}] no setting!", generateid, skillgroupweight->_id );
            }
        }

        // 属性
        auto kffighter = kfnpc->Find( __STRING__( fighter ) );
        if ( kfnpcsetting->_attr_id == 0u )
        {
            for ( auto& iter : kfnpcsetting->_attribute )
            {
                kffighter->Set( iter.first, iter.second );
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
                    uint64 finalpreference = CalcFinalPreference( racevalue, professionvalue, 100u );
                    auto levelvalue = ( uint32 )( finalpreference * ( level - 1 ) / KFRandEnum::TenThousand );

                    // 保存属性
                    kffighter->Set( name, value + levelvalue );
                }
            }
            else
            {
                __LOG_ERROR_FUNCTION__( function, line, "attribute id={} can't find!", kfnpcsetting->_attr_id );
            }
        }

        auto maxhp = kffighter->Get<uint32>( __STRING__( maxhp ) );
        kffighter->Set( __STRING__( hp ), maxhp );

        // 捏脸数据
        GeneratePinchFace( kfnpc );
        return kfnpc;
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
        auto race = kfhero->Get<uint32>( __STRING__( race ) );
        auto kfracesetting = KFRaceConfig::Instance()->FindSetting( race );
        if ( kfracesetting == nullptr )
        {
            return __LOG_ERROR__( "race setting=[{}] can't find!", race );
        }

        auto profession = kfhero->Get<uint32>( __STRING__( profession ) );
        auto kfprofessionsetting = KFProfessionConfig::Instance()->FindSetting( profession );
        if ( kfprofessionsetting == nullptr )
        {
            return __LOG_ERROR__( "profession setting=[{}] can't find!", profession );
        }

        // 成长率
        auto kfgrowth = kfhero->Find( __STRING__( growth ) );

        // 战斗属性
        auto kffighter = kfhero->Find( __STRING__( fighter ) );

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

            for ( auto kfchild = kffighter->First(); kfchild != nullptr; kfchild = kffighter->Next() )
            {
                if ( kfchild->_data_setting->_name == __STRING__( hp ) || kfchild->_data_setting->_name == __STRING__( ep ) )
                {
                    continue;
                }

                auto growthvalue = kfgrowth->Get<uint32>( kfchild->_data_setting->_name );
                auto racevalue = kfracesetting->GetAttributeValue( kfchild->_data_setting->_name );
                auto professionvalue = kfprofessionsetting->GetAttributeValue( kfchild->_data_setting->_name );

                // 最终成长率
                auto finalpreference = CalcFinalPreference( racevalue, professionvalue, growthvalue );
                auto value = finalpreference / KFRandEnum::Hundred;

                // 直接添加的属性
                auto addvalue = value / KFRandEnum::Hundred;

                // 需要随机的属性概率
                auto randvalue = value % KFRandEnum::Hundred;

                // todo 要加上初始属性
                auto fighterattribute = kfchild->Get<uint32>();
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

                if ( kfchild->_data_setting->_name == __STRING__( maxhp ) )
                {
                    // 升级当前hp增加为maxhp增加数量
                    player->UpdateData( kffighter, __STRING__( hp ), KFEnum::Add, addvalue );
                }
            }
        }
    }

    __KF_UPDATE_DATA_FUNCTION__( KFGenerateModule::OnHeroProfessionUpdate )
    {
        auto transferid = KFTransferConfig::Instance()->GetTransferId( newvalue, oldvalue );
        auto kfsetting = KFTransferConfig::Instance()->FindSetting( transferid );
        if ( kfsetting == nullptr )
        {
            return __LOG_ERROR__( "transfer setting=[{}] can't find!", transferid );
        }

        auto oldprosetting = KFProfessionConfig::Instance()->FindSetting( oldvalue );
        if ( oldprosetting == nullptr )
        {
            return __LOG_ERROR__( "transfer profession setting=[{}] can't find!", oldvalue );
        }

        auto newprosetting = KFProfessionConfig::Instance()->FindSetting( newvalue );
        if ( newprosetting == nullptr )
        {
            return __LOG_ERROR__( "transfer profession setting=[{}] can't find!", newvalue );
        }

        auto kfhero = kfdata->GetParent();
        auto race = kfhero->Get( __STRING__( race ) );
        auto racesetting = KFRaceConfig::Instance()->FindSetting( race );
        if ( racesetting == nullptr )
        {
            return __LOG_ERROR__( "transfer profession setting=[{}] can't find!", newvalue );
        }

        auto weapontype = kfhero->Get( __STRING__( weapontype ) );
        if ( !newprosetting->IsWeaponTypeValid( weapontype ) )
        {
            // 随机新的武器类型
            weapontype = newprosetting->RandWeapontype();
            player->UpdateData( kfhero, __STRING__( weapontype ), KFEnum::Set, weapontype );
        }

        // 更新属性fighter
        auto kfgrowth = kfhero->Find( __STRING__( growth ) );
        auto kffighter = kfhero->Find( __STRING__( fighter ) );
        auto level = kfhero->Get<uint32>( __STRING__( level ) );
        for ( auto kfchild = kffighter->First(); kfchild != nullptr; kfchild = kffighter->Next() )
        {
            auto& attrname = kfchild->_data_setting->_name;
            if ( attrname == __STRING__( hp ) || attrname == __STRING__( ep ) )
            {
                continue;
            }

            // 新旧职业的基础偏好率
            auto oldprovalue = oldprosetting->GetAttributeValue( attrname );
            auto newprovalue = newprosetting->GetAttributeValue( attrname );

            // 计算属性修正值
            auto value = ( newprovalue > oldprovalue ) ? ( newprovalue - oldprovalue ) : 0u;
            auto growthvalue = kfgrowth->Get<uint32>( attrname );
            value = value * level * growthvalue;

            auto dvalue = static_cast<double>( value ) / KFRandEnum::TenThousand;
            auto addvalue = static_cast<int>( round( dvalue ) );

            // 计算等级偏差属性
            auto racevalue = racesetting->GetAttributeValue( attrname );
            auto finalvalue = CalcFinalPreference( racevalue, newprovalue, growthvalue ) / KFRandEnum::Hundred;

            auto offestvalue = finalvalue / KFRandEnum::Hundred;
            auto randvalue = finalvalue % KFRandEnum::Hundred;

            static auto _option = _kf_option->FindOption( "transferleveloffest" );
            offestvalue = offestvalue * _option->_uint32_value;
            for ( uint32 i = 0u; i < _option->_uint32_value; i++ )
            {
                // 随机是否添加属性
                auto rand = KFGlobal::Instance()->RandRatio( KFRandEnum::Hundred );
                if ( rand < randvalue )
                {
                    offestvalue += 1;
                }
            }

            // 增加最终属性
            player->UpdateData( kfchild, KFEnum::Add, addvalue + offestvalue );
        }

        // 将血量回满
        auto maxhp = kffighter->Get<uint32>( __STRING__( maxhp ) );
        player->UpdateData( kffighter, __STRING__( hp ), KFEnum::Set, maxhp );

        // 更新职业等级
        player->UpdateData( kfhero, __STRING__( classlv ), KFEnum::Set, newprosetting->_class_lv );

        // 随机主动技能
        RandWeightData( player, kfhero, __STRING__( active ), kfsetting->_active_pool_list, __FUNC_LINE__ );

        // 随机性格
        RandWeightData( player, kfhero, __STRING__( character ), kfsetting->_character_pool_list, __FUNC_LINE__ );

        // 随机天赋
        RandWeightData( player, kfhero, __STRING__( innate ), kfsetting->_innate_pool_list, __FUNC_LINE__ );
    }

    void KFGenerateModule::RandWeightData( KFEntity* player, KFData* kfhero, const std::string& str, const VectorUInt32& slist, const char* function, uint32 line )
    {
        if ( slist.size() == 0u )
        {
            return;
        }

        auto kfdataarray = kfhero->Find( str );
        if ( kfdataarray == nullptr )
        {
            return;
        }

        std::set<uint32> excludelist;
        auto arraylen = kfdataarray->Size() - KFDataDefine::Array_Index;
        for ( uint32 index = KFDataDefine::Array_Index; index <= arraylen; ++index )
        {
            auto dataid = kfdataarray->Get<uint64>( index );
            if ( dataid != 0u )
            {
                excludelist.insert( dataid );
            }
        }

        auto datanum = static_cast<uint32>( excludelist.size() );
        if ( datanum >= arraylen )
        {
            return;
        }

        auto race = kfhero->Get( __STRING__( race ) );
        auto sex = kfhero->Get( __STRING__( sex ) );
        auto professionid = kfhero->Get( __STRING__( profession ) );
        auto background = kfhero->Get( __STRING__( background ) );
        auto weapontype = kfhero->Get( __STRING__( weapontype ) );
        auto movetype = 0u;

        for ( auto poolid : slist )
        {
            auto randid = 0u;
            if ( str == __STRING__( active ) || str == __STRING__( innate ) )
            {
                __RAND_WEIGHT_DATA__( poolid, KFSkillConfig::Instance(), IsValid( race, professionid, background, weapontype ), excludelist, randid );
            }
            else if ( str == __STRING__( character ) )
            {
                __RAND_WEIGHT_DATA__( poolid, KFCharacterConfig::Instance(), IsValid( race, background, professionid ), excludelist, randid );
            }

            if ( randid == 0u )
            {
                continue;
            }

            for ( uint32 index = KFDataDefine::Array_Index; index <= arraylen; ++index )
            {
                auto dataid = kfdataarray->Get<uint64>( index );
                if ( dataid == 0u )
                {
                    player->UpdateData( kfdataarray, index, KFEnum::Set, randid );
                    excludelist.insert( randid );

                    datanum++;

                    if ( str == __STRING__( active ) )
                    {
                        // 转职主动技能更新为最新的技能
                        player->UpdateData( kfhero, __STRING__( activeindex ), KFEnum::Set, index );
                    }

                    break;
                }
            }

            if ( datanum >= arraylen )
            {
                break;
            }
        }
    }
}