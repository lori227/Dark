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
    uint32 KFGenerateModule::CalcRace( uint32 racepoolid, const DivisorList& divisorlist, const std::map< uint32, const KFProfessionSetting* >& professionlist )
    {
        auto racepool = KFWeightConfig::Instance()->FindWeightPool( racepoolid );
        if ( racepool == nullptr )
        {
            __LOG_ERROR__( "race pool=[{}] can't find", racepoolid );
            return 0u;
        }

        // 计算出解锁职业的种族列表
        UInt32Set racelist;
        for ( auto& iter : professionlist )
        {
            auto kfprofessionsetting = iter.second;
            racelist.insert( kfprofessionsetting->_race_list.begin(), kfprofessionsetting->_race_list.end() );
        }

        std::vector< const KFDivisorSetting* > randlist;
        for ( auto kfsetting : divisorlist )
        {
            // 不是种族偏好因子
            if ( kfsetting->_type != KFMsg::DivisorOfRace )
            {
                continue;
            }

            // 不在解锁职业的种族列表中
            if ( !racelist.empty() )
            {
                if ( racelist.find( kfsetting->_value ) == racelist.end() )
                {
                    continue;
                }
            }

            // 随机池没有该种族
            if ( !racepool->Have( kfsetting->_value ) )
            {
                continue;
            }

            randlist.push_back( kfsetting );
        }

        // 使用种族偏好因子
        auto divisorcount = ( uint32 )randlist.size();
        if ( divisorcount > 0u )
        {
            auto index = KFGlobal::Instance()->RandRatio( divisorcount );
            auto kfsetting = randlist.at( index );
            auto rand = KFGlobal::Instance()->RandRatio( KFRandEnum::Hundred );
            if ( rand < kfsetting->_probability )
            {
                return kfsetting->_value;
            }
        }

        // 权重池里随机
        const KFWeight* kfweight = nullptr;
        if ( racelist.empty() )
        {
            kfweight = racepool->Rand();
        }
        else
        {
            kfweight = racepool->Rand( racelist, false );
        }
        if ( kfweight == nullptr )
        {
            __LOG_ERROR__( "pool=[{}] can't rand race", racepoolid );
            return 0u;
        }

        return kfweight->_id;
    }

    uint32 KFGenerateModule::CalcSex( uint32 sexpoolid )
    {
        auto sexpool = KFWeightConfig::Instance()->FindWeightPool( sexpoolid );
        if ( sexpool == nullptr )
        {
            return KFGlobal::Instance()->RandRange( KFMsg::Male, KFMsg::Female, 1u );
        }

        // 按权重随机
        auto kfweight = sexpool->Rand();
        if ( kfweight == nullptr )
        {
            return KFGlobal::Instance()->RandRange( KFMsg::Male, KFMsg::Female, 1u );
        }

        return kfweight->_id;
    }

    uint32 KFGenerateModule::CalcMove( const DivisorList& divisorlist, const std::map<uint32, const KFProfessionSetting*>& professionlist )
    {
        // 计算出解锁职业的移动方式列表
        UInt32Set movelist;
        for ( auto& iter : professionlist )
        {
            auto kfprofessionsetting = iter.second;
            movelist.insert( kfprofessionsetting->_move_type );
        }

        std::vector< const KFDivisorSetting* > randlist;
        for ( auto kfsetting : divisorlist )
        {
            // 不是移动偏好因子
            if ( kfsetting->_type != KFMsg::DivisorOfMove )
            {
                continue;
            }

            // 不在解锁职业的移动列表中
            if ( !movelist.empty() )
            {
                if ( movelist.find( kfsetting->_value ) == movelist.end() )
                {
                    continue;
                }
            }

            randlist.push_back( kfsetting );
        }

        // 使用移动方式偏好因子
        auto divisorcount = ( uint32 )randlist.size();
        if ( divisorcount > 0u )
        {
            auto index = KFGlobal::Instance()->RandRatio( divisorcount );
            auto kfsetting = randlist.at( index );
            auto rand = KFGlobal::Instance()->RandRatio( KFRandEnum::Hundred );
            if ( rand < kfsetting->_probability )
            {
                return kfsetting->_value;
            }
        }

        return 0u;
    }

    uint32 KFGenerateModule::CalcWeapon( const DivisorList& divisorlist, const std::map<uint32, const KFProfessionSetting*>& professionlist )
    {
        // 计算出解锁职业的武器类型列表
        UInt32Set weaponlist;
        for ( auto& iter : professionlist )
        {
            auto kfprofessionsetting = iter.second;
            weaponlist.insert( kfprofessionsetting->_weapon_type_list.begin(), kfprofessionsetting->_weapon_type_list.end() );
        }

        std::vector< const KFDivisorSetting* > randlist;
        for ( auto kfsetting : divisorlist )
        {
            // 不是武器类型偏好因子
            if ( kfsetting->_type != KFMsg::DivisorOfWeapon )
            {
                continue;
            }

            // 不在解锁职业的武器类型中
            if ( !weaponlist.empty() )
            {
                if ( weaponlist.find( kfsetting->_value ) == weaponlist.end() )
                {
                    continue;
                }
            }

            randlist.push_back( kfsetting );
        }

        // 使用武器类型偏好因子
        auto divisorcount = ( uint32 )randlist.size();
        if ( divisorcount > 0u )
        {
            auto index = KFGlobal::Instance()->RandRatio( divisorcount );
            auto kfsetting = randlist.at( index );
            auto rand = KFGlobal::Instance()->RandRatio( KFRandEnum::Hundred );
            if ( rand < kfsetting->_probability )
            {
                return kfsetting->_value;
            }
        }

        return 0u;
    }

    std::tuple< uint32, uint32 > KFGenerateModule::CalcMoveAndWeapon( KFEntity* player, uint32 professionpoolid, uint32 race, uint32 sex,
            const DivisorList& divisorlist, const std::map<uint32, const KFProfessionSetting*>& professionlist )
    {
        // 木有招募因子
        if ( divisorlist.empty() )
        {
            return std::make_tuple( 0u, 0u );
        }

        auto movetype = CalcMove( divisorlist, professionlist );
        auto weapontype = CalcWeapon( divisorlist, professionlist );
        if ( movetype == 0u && weapontype == 0u )
        {
            return std::make_tuple( 0u, 0u );
        }

        auto weightpool = KFWeightConfig::Instance()->FindWeightPool( professionpoolid );
        if ( weightpool == nullptr )
        {
            return std::make_tuple( 0u, 0u );
        }

        do
        {
            // 遍历权重表, 找出满足 movetype, weapontype 的职业
            for ( auto weightdata : weightpool->_weight_data )
            {
                if ( weightdata->_id == 0u || weightdata->_weight == 0u )
                {
                    continue;
                }

                // 判断是否解锁的职业
                if ( !professionlist.empty() )
                {
                    auto iter = professionlist.find( weightdata->_id );
                    if ( iter == professionlist.end() )
                    {
                        continue;
                    }
                }

                auto kfsetting = KFProfessionConfig::Instance()->FindSetting( weightdata->_id );
                if ( kfsetting != nullptr && kfsetting->IsValid( race, sex, movetype, weapontype ) )
                {
                    // 有满足要求的, 直接返回, 在后面使用权重随机
                    return std::make_tuple( movetype, weapontype );
                }
            }

            // 没有找到, 随机清除掉一个因子
            if ( movetype == 0u )
            {
                weapontype = 0u;
            }
            else if ( weapontype == 0u )
            {
                movetype = 0u;
            }
            else
            {
                auto rand = KFGlobal::Instance()->RandRatio( 2u );
                if ( rand == 0u )
                {
                    movetype = 0u;
                }
                else
                {
                    weapontype = 0u;
                }
            }
        } while ( movetype != 0u || weapontype != 0u );

        return std::make_tuple( movetype, weapontype );
    }

#define __RAND_WEIGHT_DATA__( poolid, config, isvalid, randid, includelist )\
    {\
        auto weightpool = KFWeightConfig::Instance()->FindWeightPool( poolid );\
        if ( weightpool != nullptr )\
        {\
            for ( auto weightdata : weightpool->_weight_data )\
            {   \
                if ( !includelist.empty() && includelist.find( weightdata->_id ) == includelist.end() )\
                {\
                    excludelist.insert( weightdata->_id ); \
                    continue;\
                }\
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
                weightdata = weightpool->Rand( excludelist, true ); \
            }\
            if ( weightdata != nullptr )\
            {   \
                randid = weightdata->_id; \
            }\
            else\
            {   \
                if ( excludelist.empty() )\
                {\
                    __LOG_ERROR__( "{} pool=[{}] race=[{}] sex=[{}] profession=[{}] move=[{}] weapon=[{}] background=[{}] can't rand weight", \
                                   #config, poolid, race, sex, professionid, movetype, weapontype, backgroundid ); \
                }\
            }\
        }\
        else\
        {\
            __LOG_ERROR__( "pool=[{}] can't find", poolid );\
        }\
    }\

#define __RAND_WEIGHT_DATA_CLEAR__( poolid, config, isvalid, randid, includelist )\
    {\
        excludelist.clear();\
        __RAND_WEIGHT_DATA__( poolid, config, isvalid, randid, includelist );\
    }

    KFData* KFGenerateModule::GeneratePlayerHero( KFEntity* player, KFData* kfhero, uint32 generateid )
    {
        static UInt32Set _profession_list;
        static DivisorList _divisor_list;
        return GenerateHero( player, kfhero, generateid, _divisor_list, _profession_list, 0u, 0u, 0u );
    }

    KFData* KFGenerateModule::GeneratePlayerHero( KFEntity* player, KFData* kfhero, uint32 generateid,
            const DivisorList& divisorlist, const UInt32Set& professionlist, uint32 generatelevel, uint32 mingrowth, uint32 maxgrowth )
    {
        return GenerateHero( player, kfhero, generateid, divisorlist, professionlist, generatelevel, mingrowth, maxgrowth );
    }

    KFData* KFGenerateModule::GenerateHero( KFEntity* player, KFData* kfhero, uint32 generateid,
                                            const DivisorList& divisorlist, const UInt32Set& professionlist, uint32 generatelevel, uint32 mingrowth, uint32 maxgrowth )
    {
        auto kfgeneratesetting = KFGenerateConfig::Instance()->FindSetting( generateid );
        if ( kfgeneratesetting == nullptr )
        {
            __LOG_ERROR__( "hero generate=[{}] can't find", generateid );
            return nullptr;
        }

        std::map< uint32, const KFProfessionSetting* > professionsettinglist;
        for ( auto professionid : professionlist )
        {
            auto kfprofessionsetting = KFProfessionConfig::Instance()->FindSetting( professionid );
            if ( kfprofessionsetting != nullptr )
            {
                professionsettinglist[ professionid ] = kfprofessionsetting;
            }
        }
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        kfhero->Set( __STRING__( id ), generateid );
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 随机种族
        auto race = CalcRace( kfgeneratesetting->_race_pool_id, divisorlist, professionsettinglist );
        auto kfracesetting = KFRaceConfig::Instance()->FindSetting( race );
        if ( kfracesetting == nullptr )
        {
            __LOG_ERROR__( "race setting=[{}] can't find", race );
            return nullptr;
        }
        kfhero->Set( __STRING__( race ), race );
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        auto sex = kfracesetting->_sex;
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 如果需要使用权重因子, 计算出 移动方式, 武器类型
        auto movetype = 0u;
        auto weapontype = 0u;
        auto backgroundid = 0u;
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        std::tie( movetype, weapontype ) = CalcMoveAndWeapon( player, kfgeneratesetting->_profession_pool_id, race, sex, divisorlist, professionsettinglist );
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 随机职业
        auto kfprofession = kfhero->Find( __STRING__( profession ) );
        auto professionid = kfprofession->_data_setting->_int_init_value;

        static UInt32Set excludelist;
        static UInt32Set includelist;
        __RAND_WEIGHT_DATA_CLEAR__( kfgeneratesetting->_profession_pool_id, KFProfessionConfig::Instance(), IsValid( race, sex, movetype, weapontype ), professionid, professionlist );
        auto kfprofessionsetting = KFProfessionConfig::Instance()->FindSetting( professionid );
        if ( kfprofessionsetting == nullptr )
        {
            __LOG_ERROR__( "profession setting=[{}] can't find", professionid );
            return nullptr;
        }
        kfprofession->Set( professionid );

        // 职业等级
        kfhero->Set( __STRING__( classlv ), kfprofessionsetting->_class_lv );
        if ( kfprofessionsetting->_class_lv > 1u )
        {
            // 高阶职业添加转职信息
            AddTransferData( player, kfhero, professionid, kfprofessionsetting->_class_lv );
        }
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        if ( sex == KFMsg::UnknowSex )
        {
            sex = kfprofessionsetting->_sex_limit;
            if ( sex == KFMsg::UnknowSex )
            {
                sex = CalcSex( kfgeneratesetting->_sex_pool_id );
            }
        }
        kfhero->Set( __STRING__( sex ), sex );
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
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
        __RAND_WEIGHT_DATA_CLEAR__( kfgeneratesetting->_background_pool_id, KFBackGroundConfig::Instance(), IsValid( race, sex ), backgroundid, includelist );
        kfbackground->Set( backgroundid );

        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 随机性格
        auto kfcharacterdata = kfhero->Find( __STRING__( character ) );
        for ( auto poolid : kfgeneratesetting->_character_pool_list )
        {
            auto characterid = kfcharacterdata->_data_setting->_int_init_value;

            __RAND_WEIGHT_DATA_CLEAR__( poolid, KFCharacterConfig::Instance(), IsValid( race, backgroundid, professionid ), characterid, includelist );
            if ( characterid != 0u )
            {
                kfcharacterdata->Insert( characterid );
            }
        }
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 随机天赋
        auto kfinnatedata = kfhero->Find( __STRING__( innate ) );
        for ( auto poolid : kfgeneratesetting->_innate_pool_list )
        {
            auto innateid = kfinnatedata->_data_setting->_int_init_value;

            __RAND_WEIGHT_DATA_CLEAR__( poolid, KFSkillConfig::Instance(), IsValid( race, professionid, backgroundid, weapontype ), innateid, includelist );
            if ( innateid != 0u )
            {
                kfinnatedata->Insert( innateid );
            }
        }
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 随机主动技能
        auto kfactivedata = kfhero->Find( __STRING__( active ) );
        for ( auto poolid : kfgeneratesetting->_active_pool_list )
        {
            auto activeid = kfactivedata->_data_setting->_int_init_value;
            __RAND_WEIGHT_DATA_CLEAR__( poolid, KFSkillConfig::Instance(), IsValid( race, professionid, backgroundid, weapontype ), activeid, includelist );
            if ( activeid != 0u )
            {
                kfactivedata->Insert( activeid );
            }
        }

        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 生成等级
        if ( generatelevel > 0u )
        {
            generatelevel += kfhero->Get<uint32>( __STRING__( level ) );

            // 英雄职业上限修正
            if ( generatelevel > kfprofessionsetting->_max_level )
            {
                generatelevel = kfprofessionsetting->_max_level;
            }

            auto kflevelsetting = KFLevelConfig::Instance()->FindSetting( generatelevel );
            if ( kflevelsetting != nullptr )
            {
                kfhero->Set( __STRING__( level ), generatelevel );
                kfhero->Set( __STRING__( exp ), kflevelsetting->_exp );
            }
        }

        auto herolevel = kfhero->Get<uint32>( __STRING__( level ) );
        {
            // 添加生成技能和天赋后 再添加等级技能和天赋
            UInt32Vector active_pool_list;
            UInt32Vector innate_pool_list;

            for ( uint32 i = 1u; i <= herolevel; i++ )
            {
                auto kflevelsetting = KFLevelConfig::Instance()->FindSetting( i );
                if ( kflevelsetting == nullptr )
                {
                    continue;
                }

                active_pool_list.insert( active_pool_list.end(), kflevelsetting->_active_pool_list.begin(), kflevelsetting->_active_pool_list.end() );
                innate_pool_list.insert( innate_pool_list.end(), kflevelsetting->_innate_pool_list.begin(), kflevelsetting->_innate_pool_list.end() );
            }

            // 随机主动技能
            RandWeightData( player, kfhero, __STRING__( active ), active_pool_list, false );

            // 随机天赋
            RandWeightData( player, kfhero, __STRING__( innate ), innate_pool_list, false );
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
                auto minvalue = iter.second->_min_value + mingrowth;
                auto maxvalue = iter.second->_max_value + maxgrowth;
                auto value = KFGlobal::Instance()->RandRange( minvalue, maxvalue, 1u );
                kfgrowth->Set( name, value );
            }
        }
        else
        {
            __LOG_ERROR__( "attribute id={} can't find", kfgeneratesetting->_growth_id );
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

                auto value = CalcHeroInitAttribute( kffighter, initvalue, racevalue, professionvalue, growthvalue, generatelevel );
                kffighter->Set( name, value );
            }

            auto maxhp = kffighter->Get( __STRING__( maxhp ) );
            kffighter->Set( __STRING__( hp ), maxhp );
        }
        else
        {
            __LOG_ERROR__( "attribute id={} can't find", kfgeneratesetting->_attr_id );
        }

        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 捏脸数据
        GeneratePinchFace( player, kfhero );
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        return kfhero;
    }

    void KFGenerateModule::GeneratePinchFace( KFEntity* player, KFData* kfhero )
    {
        auto kfhead = kfhero->Find( __STRING__( head ) );

        player->InitArray( kfhead, KFPinchFaceConfig::Instance()->_settings.Size() );
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

    void KFGenerateModule::AddTransferData( KFEntity* player, KFData* kfhero, uint32 profession, uint32 classlv )
    {
        UInt32Vector prolist;

        for ( uint32 i = 1u; i < classlv; i++ )
        {
            profession = KFTransferConfig::Instance()->GetParentPro( profession );
            if ( profession == _invalid_int )
            {
                break;
            }

            prolist.emplace( prolist.begin(), profession );
        }

        auto kftransferrecord = kfhero->Find( __STRING__( transfer ) );

        for ( auto iter : prolist )
        {
            auto weapontypelist = KFProfessionConfig::Instance()->FindSetting( iter )->_weapon_type_list;
            if ( weapontypelist.size() == 0 )
            {
                continue;
            }

            uint32 weapontype = *( weapontypelist.begin() );

            auto kftransfer = player->CreateData( kftransferrecord );
            auto index = kftransferrecord->Size() + 1;
            kftransfer->Set( __STRING__( index ), index );
            kftransfer->Set( __STRING__( profession ), iter );
            kftransfer->Set( __STRING__( weapontype ), weapontype );

            kftransferrecord->Add( index, kftransfer );
        }
    }

    KFData* KFGenerateModule::GenerateNpcHero( KFEntity* player, KFData* kfnpcrecord, uint32 generateid, uint32 level )
    {
        auto kfnpcsetting = KFNpcConfig::Instance()->FindSetting( generateid );
        if ( kfnpcsetting == nullptr )
        {
            __LOG_ERROR__( "npc generateid=[{}] can't find", generateid );
            return nullptr;
        }

        auto kfracesetting = KFRaceConfig::Instance()->FindSetting( kfnpcsetting->_race_id );
        if ( kfracesetting == nullptr )
        {
            __LOG_ERROR__( "race setting=[{}] can't find", kfnpcsetting->_race_id );
            return nullptr;
        }

        auto kfprofessionsetting = KFProfessionConfig::Instance()->FindSetting( kfnpcsetting->_profession_id );
        if ( kfprofessionsetting == nullptr )
        {
            __LOG_ERROR__( "profession setting=[{}] can't find", kfnpcsetting->_profession_id );
            return nullptr;
        }

        auto movetype = 0u;
        auto backgroundid = 0u;
        auto race = kfnpcsetting->_race_id;
        auto professionid = kfnpcsetting->_profession_id;

        auto kfnpc = player->CreateData( kfnpcrecord );
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
            __LOG_ERROR__( "npc generateid=[{}] san empty", generateid );
        }

        // 随机性格
        auto kfcharacterdata = kfnpc->Find( __STRING__( character ) );
        for ( auto poolid : kfnpcsetting->_character_pool_list )
        {
            auto weightdata = KFWeightConfig::Instance()->RandWeight( poolid );
            if ( weightdata == nullptr )
            {
                __LOG_ERROR__( "npc generateid=[{}] character=[{}] rand empty", generateid, poolid );
                continue;
            }

            kfcharacterdata->Insert( weightdata->_id );
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
                        __LOG_ERROR__( "weapon=[{}] weapontype = 0", kfnpcsetting->_weapon_id );
                    }
                }
                else
                {
                    __LOG_ERROR__( "weapon=[{}] can't find setting", kfnpcsetting->_weapon_id );
                }

                // 随机主动技能
                static UInt32Set excludelist;
                static UInt32Set includelist;
                auto kfactivedata = kfnpc->Find( __STRING__( active ) );
                for ( auto poolid : kfskillsetting->_active_pool_list )
                {
                    auto activeid = 0u;
                    __RAND_WEIGHT_DATA_CLEAR__( poolid, KFSkillConfig::Instance(), IsValid( race, professionid, backgroundid, weapontype ), activeid, includelist );
                    if ( activeid != 0u )
                    {
                        kfactivedata->Insert( activeid );
                    }
                }

                // 天赋技能
                auto kfinnatedata = kfnpc->Find( __STRING__( innate ) );
                for ( auto poolid : kfskillsetting->_innate_pool_list )
                {
                    auto innateid = 0u;
                    __RAND_WEIGHT_DATA_CLEAR__( poolid, KFSkillConfig::Instance(), IsValid( race, professionid, backgroundid, weapontype ), innateid, includelist );
                    if ( innateid != 0u )
                    {
                        kfinnatedata->Insert( innateid );
                    }
                }
            }
            else
            {
                __LOG_ERROR__( "npc generateid=[{}] skillgroup=[{}] no setting", generateid, skillgroupweight->_id );
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
                    auto value = CalcHeroInitAttribute( kffighter, initvalue, racevalue, professionvalue, 100u, level );

                    // 保存属性
                    kffighter->Set( name, value );
                }
            }
            else
            {
                __LOG_ERROR__( "attribute id={} can't find", kfnpcsetting->_attr_id );
            }
        }

        auto maxhp = kffighter->Get<uint32>( __STRING__( maxhp ) );
        kffighter->Set( __STRING__( hp ), maxhp );

        // 捏脸数据
        GeneratePinchFace( player, kfnpc );
        return kfnpc;
    }

    uint64 KFGenerateModule::CalcFinalPreference( uint32 racevalue, uint32 professionvalue, uint32 growthvalue )
    {
        // 最终属性偏好率=（角色_属性偏好率+职业_属性偏好率）*属性成长率
        uint64 value = ( uint64 )( racevalue + professionvalue ) * ( uint64 )growthvalue;
        return value;
    }

    uint32 KFGenerateModule::CalcHeroInitAttribute( KFData* kffighter, uint32 initvalue, uint32 racevalue, uint32 professionvalue, uint32 growthvalue, uint32 level )
    {
        // 直接计算 成长率 x count(9)
        auto finalpreference = CalcFinalPreference( racevalue, professionvalue, growthvalue );
        auto totalvalue = ( uint32 )( ( finalpreference * kffighter->_data_setting->_int_max_value ) / KFRandEnum::TenThousand );

        // 随机计算 x count(2) 如: 210%  100%+2属性, 另外10%+1额外属性
        auto value = finalpreference / KFRandEnum::Hundred;
        auto addvalue = value / KFRandEnum::Hundred;
        auto randvalue = value % KFRandEnum::Hundred;

        // 循环次数加上等级
        auto logiccount = kffighter->_data_setting->_int_logic_value + level - 1u;
        for ( auto i = 0u; i < logiccount; ++i )
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
            return __LOG_ERROR__( "race setting=[{}] can't find", race );
        }

        auto profession = kfhero->Get<uint32>( __STRING__( profession ) );
        auto kfprofessionsetting = KFProfessionConfig::Instance()->FindSetting( profession );
        if ( kfprofessionsetting == nullptr )
        {
            return __LOG_ERROR__( "profession setting=[{}] can't find", profession );
        }

        // 成长率
        auto kfgrowth = kfhero->Find( __STRING__( growth ) );

        // 战斗属性
        auto kffighter = kfhero->Find( __STRING__( fighter ) );

        for ( auto level = oldvalue + 1u; level <= newvalue; ++level )
        {
            // 获得升级后的配置
            auto kflevelsetting = KFLevelConfig::Instance()->FindSetting( level );
            if ( kflevelsetting == nullptr )
            {
                __LOG_ERROR__( "level=[{}] setting can't find", level );
                continue;
            }

            // 随机主动技能
            RandWeightData( player, kfhero, __STRING__( active ), kflevelsetting->_active_pool_list );

            // 随机天赋
            RandWeightData( player, kfhero, __STRING__( innate ), kflevelsetting->_innate_pool_list );

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
                auto totalvalue = ( uint32 )( ( finalpreference * level ) / KFRandEnum::TenThousand );

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
            return __LOG_ERROR__( "transfer setting=[{}] can't find", transferid );
        }

        auto oldprosetting = KFProfessionConfig::Instance()->FindSetting( oldvalue );
        if ( oldprosetting == nullptr )
        {
            return __LOG_ERROR__( "transfer profession setting=[{}] can't find", oldvalue );
        }

        auto newprosetting = KFProfessionConfig::Instance()->FindSetting( newvalue );
        if ( newprosetting == nullptr )
        {
            return __LOG_ERROR__( "transfer profession setting=[{}] can't find", newvalue );
        }

        auto kfhero = kfdata->GetParent();
        auto race = kfhero->Get( __STRING__( race ) );
        auto racesetting = KFRaceConfig::Instance()->FindSetting( race );
        if ( racesetting == nullptr )
        {
            return __LOG_ERROR__( "transfer profession setting=[{}] can't find", newvalue );
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
        auto randindex = RandWeightData( player, kfhero, __STRING__( active ), kfsetting->_active_pool_list );
        if ( randindex != 0u )
        {
            // 转职主动技能更新为最新的技能
            player->UpdateData( kfhero, __STRING__( activeindex ), KFEnum::Set, randindex );
        }

        // 随机性格
        RandWeightData( player, kfhero, __STRING__( character ), kfsetting->_character_pool_list );

        // 随机天赋
        RandWeightData( player, kfhero, __STRING__( innate ), kfsetting->_innate_pool_list );
    }

    uint32 KFGenerateModule::RandWeightData( KFEntity* player, KFData* kfhero, const std::string& dataname, const UInt32Vector& slist, bool update /*= true*/ )
    {
        uint32 randindex = 0u;
        if ( slist.empty() )
        {
            return randindex;
        }

        auto kfdataarray = kfhero->Find( dataname );
        if ( kfdataarray == nullptr )
        {
            return randindex;
        }

        if ( kfdataarray->IsFull() )
        {
            return randindex;
        }

        UInt32Set excludelist;
        kfdataarray->GetHashs( excludelist );

        static UInt32Set includelist;
        auto movetype = 0u;
        auto race = kfhero->Get( __STRING__( race ) );
        auto sex = kfhero->Get( __STRING__( sex ) );
        auto professionid = kfhero->Get( __STRING__( profession ) );
        auto backgroundid = kfhero->Get( __STRING__( background ) );
        auto weapontype = kfhero->Get( __STRING__( weapontype ) );

        for ( auto poolid : slist )
        {
            auto randid = 0u;
            if ( dataname == __STRING__( active ) || dataname == __STRING__( innate ) )
            {
                __RAND_WEIGHT_DATA__( poolid, KFSkillConfig::Instance(), IsValid( race, professionid, backgroundid, weapontype ), randid, includelist );
            }
            else if ( dataname == __STRING__( character ) )
            {
                __RAND_WEIGHT_DATA__( poolid, KFCharacterConfig::Instance(), IsValid( race, backgroundid, professionid ), randid, includelist );
            }

            if ( randid == 0u )
            {
                continue;
            }

            auto index = kfdataarray->GetEmpty();
            if ( index >= kfdataarray->MaxSize() )
            {
                break;
            }

            randindex = index;
            excludelist.insert( randid );

            if ( update )
            {
                player->UpdateData( kfdataarray, index, KFEnum::Set, randid );
            }
            else
            {
                kfdataarray->Insert( randid );
            }

        }

        return randindex;
    }
}