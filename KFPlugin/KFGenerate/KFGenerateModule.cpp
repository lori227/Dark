#include "KFGenerateModule.hpp"
#include "KFProtocol/KFProtocol.h"


namespace KFrame
{
    void KFGenerateModule::BeforeRun()
    {
        _kf_component = _kf_kernel->FindComponent( __STRING__( player ) );
        __REGISTER_UPDATE_DATA_2__( __STRING__( hero ), __STRING__( level ), &KFGenerateModule::OnHeroLevelUpdateCallBack );
        __REGISTER_UPDATE_DATA_2__( __STRING__( hero ), __STRING__( profession ), &KFGenerateModule::OnHeroProUpdateCallBack );
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

        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 随机背景
        auto kfbackground = kfhero->Find( __STRING__( background ) );
        __RAND_WEIGHT_DATA_CLEAR__( kfgeneratesetting->_background_pool_id, KFBackGroundConfig::Instance(), IsValid( race, sex ), backgroundid, includelist );
        kfbackground->Set( backgroundid );

        // 随机品质
        auto qualityid = 1u;
        auto kfquality = kfhero->Find( __STRING__( quality ) );
        __RAND_WEIGHT_DATA_CLEAR__( kfgeneratesetting->_quality_pool_id, KFQualityConfig::Instance(), IsValid(), qualityid, includelist );
        kfquality->Set( qualityid );

        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        UInt32Vector character_pool_list = kfgeneratesetting->_character_pool_list;
        UInt32Vector innate_pool_list = kfgeneratesetting->_innate_pool_list;
        UInt32Vector active_pool_list = kfgeneratesetting->_active_pool_list;
        auto kflevelsetting = KFLevelConfig::Instance()->FindSetting( 1u );
        if ( kflevelsetting != nullptr )
        {
            innate_pool_list.insert( innate_pool_list.end(), kflevelsetting->_innate_pool_list.begin(), kflevelsetting->_innate_pool_list.end() );
            active_pool_list.insert( active_pool_list.end(), kflevelsetting->_active_pool_list.begin(), kflevelsetting->_active_pool_list.end() );
        }

        // 随机生成性格
        RandWeightData( player, kfhero, __STRING__( character ), kfgeneratesetting->_character_pool_list, false );

        // 随机生成主动技能
        RandWeightData( player, kfhero, __STRING__( active ), kfgeneratesetting->_active_pool_list, false );

        // 随机生成天赋
        auto randlist = RandWeightData( player, kfhero, __STRING__( innate ), kfgeneratesetting->_innate_pool_list, false );
        if ( !randlist.empty() )
        {
            UpdateInnateData( player, kfhero, randlist, false );
        }

        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 随机成长率
        auto kfgrowth = kfhero->Find( __STRING__( growth ) );
        auto kfqualitysetting = KFQualityConfig::Instance()->FindSetting( qualityid );
        if ( kfqualitysetting != nullptr )
        {
            for ( auto iter : kfqualitysetting->_growth._objects )
            {
                auto& name = iter.first;
                auto minvalue = iter.second->_min_value + mingrowth;
                auto maxvalue = iter.second->_max_value + maxgrowth;
                auto value = KFGlobal::Instance()->RandRange( minvalue, maxvalue );
                kfgrowth->Set( name, value );
            }
        }
        else
        {
            __LOG_ERROR__( "quality id={} can't find", qualityid );
        }

        // 获取英雄等级
        auto herolevel = kfhero->Get<uint32>( __STRING__( level ) );
        if ( kfgeneratesetting->_level > 0u )
        {
            // 生成池固定等级
            herolevel = kfgeneratesetting->_level;
            herolevel = __MIN__( herolevel, kfprofessionsetting->_max_level );
        }
        else
        {
            if ( generatelevel > 0u )
            {
                herolevel += generatelevel;
            }

            auto maxlevel = _kf_hero->GetPlayerMaxLevel( player );
            herolevel = __MIN__( herolevel, maxlevel );
        }

        // 英雄自动升级转职流程
        std::vector<KFGenerateData> generatelist;
        HeroAutoUpgradeAndTransfer( player, kfhero, herolevel, kfprofessionsetting->_class_lv, professionid, generatelist );
        kfprofession->Set( professionid );

        // 获取最初始的职业配置
        auto kfinitprofsetting = KFProfessionConfig::Instance()->FindSetting( professionid );
        if ( kfinitprofsetting == nullptr )
        {
            __LOG_ERROR__( "profession setting=[{}] can't find", professionid );
            return nullptr;
        }

        // 初始属性
        auto kfattributesetting = KFHeroAttributeConfig::Instance()->FindSetting( kfgeneratesetting->_attr_id );
        if ( kfattributesetting != nullptr )
        {
            auto kffighter = kfhero->Find( __STRING__( fighter ) );
            for ( auto iter : kfattributesetting->_datas._objects )
            {
                auto& name = iter.first;

                auto growthvalue = kfgrowth->Get< uint32 >( name );
                auto racevalue = kfracesetting->GetAttributeValue( name );
                auto professionvalue = kfinitprofsetting->GetAttributeValue( name );

                auto value = CalcHeroInitAttribute( kffighter, racevalue, professionvalue, growthvalue, 1u );
                kffighter->Set( name, value );
            }

            auto maxhp = kffighter->Get( __STRING__( maxhp ) );
            kffighter->Set( __STRING__( hp ), maxhp );
        }
        else
        {
            __LOG_ERROR__( "attribute id={} can't find", kfgeneratesetting->_attr_id );
        }

        // 执行升级和转职流程
        for ( auto iter : generatelist )
        {
            if ( iter.type == 0u )
            {
                // 英雄升级
                kfhero->Set( __STRING__( level ), iter.endlevel );
                OnHeroLevelUpdate( player, kfhero, iter.beginlevel, iter.endlevel, false );
            }
            else
            {
                // 正常英雄转职
                kfhero->Set( __STRING__( profession ), iter.endpro );
                OnHeroProUpdate( player, kfhero, iter.beginpro, iter.endpro, false );
            }
        }

        professionid = kfhero->Get<uint32>( __STRING__( profession ) );
        kfprofessionsetting = KFProfessionConfig::Instance()->FindSetting( professionid );
        if ( kfprofessionsetting == nullptr )
        {
            __LOG_ERROR__( "profession setting=[{}] can't find", professionid );
            return nullptr;
        }

        // 设置英雄耐久度
        auto durability = _invalid_int;
        if ( kfgeneratesetting->_durability > 0u )
        {
            durability = kfgeneratesetting->_durability;
        }
        else
        {
            durability = kfqualitysetting->_role_durability.CalcValue();
        }

        kfhero->Set( __STRING__( durability ), durability );
        kfhero->Set( __STRING__( maxdurability ), durability );

        // 职业等级
        kfhero->Set( __STRING__( classlv ), kfprofessionsetting->_class_lv );

        // 设置武器
        weapontype = kfhero->Get( __STRING__( weapontype ) );
        auto kfweapontypesetting = KFWeaponTypeConfig::Instance()->FindSetting( weapontype );
        if ( kfweapontypesetting != nullptr )
        {
            auto kfweapon = kfhero->Find( __STRING__( weapon ) );
            _kf_item->CreateItem( player, kfweapontypesetting->_default_weapon_id, kfweapon, __FUNC_LINE__ );
        }

        // 设置经验
        herolevel = kfhero->Get<uint32>( __STRING__( level ) );
        kflevelsetting = KFLevelConfig::Instance()->FindSetting( herolevel + 1u );
        if ( kflevelsetting != nullptr )
        {
            kfhero->Set( __STRING__( totalexp ), kflevelsetting->_exp );
        }
        kfhero->Set( __STRING__( exprate ), kfgeneratesetting->_exp_rate );
        kfhero->Set( __STRING__( custom ), kfgeneratesetting->_custom );

        // 设置英雄的最大等级
        auto maxlevel = _kf_hero->CalcMaxLevel( player, kfhero );
        kfhero->Set( __STRING__( maxlevel ), maxlevel );

        // 设置主动技能
        auto kfactiverecord = kfhero->Find( __STRING__( active ) );
        if ( kfactiverecord->Size() > 0u )
        {
            auto activeindex = kfactiverecord->First()->Get<uint32>( __STRING__( id ) );
            kfhero->Set( __STRING__( activeindex ), activeindex );
        }

        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 捏脸数据
        GeneratePinchFace( player, kfhero );

        // 音色类型
        GenerateVoice( player, kfhero, kfprofessionsetting );
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
                auto value = kfsetting->_range.CalcValue();
                kfdata->Set< int64 >( value * KFRandEnum::TenThousand );
            }
        }
    }

    void KFGenerateModule::GenerateVoice( KFEntity* player, KFData* kfhero, const KFProfessionSetting* kfsetting )
    {
        auto kfherorecord = player->Find( __STRING__( hero ) );

        UInt32Set excludes;
        for ( auto kfchild = kfherorecord->First(); kfchild != nullptr; kfchild = kfherorecord->Next() )
        {
            auto professionid = kfchild->Get( __STRING__( profession ) );
            if ( professionid != kfsetting->_id )
            {
                continue;
            }

            auto voice = kfchild->Get<uint32>( __STRING__( voice ) );
            if ( voice != 0u )
            {
                excludes.insert( voice );
            }
        }

        auto voice = KFUtility::RandVectorValue( kfsetting->_voice_list, excludes );
        kfhero->Set( __STRING__( voice ), voice );
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
        kfnpc->SetKeyID( KFGlobal::Instance()->STMakeUuid() );
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
            kfnpc->Set( __STRING__( dip ), dipweightdata->_range.CalcValue() );
        }
        else
        {
            __LOG_ERROR__( "npc generateid=[{}] san empty", generateid );
        }

        // 随机性格
        RandWeightData( player, kfnpc, __STRING__( character ), kfnpcsetting->_character_pool_list, false );

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
                RandWeightData( player, kfnpc, __STRING__( active ), kfskillsetting->_active_pool_list, false );

                // 随机天赋技能
                RandWeightData( player, kfnpc, __STRING__( innate ), kfskillsetting->_innate_pool_list, false );
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

                    // 种族和职业
                    auto racevalue = kfracesetting->GetAttributeValue( name );
                    auto professionvalue = kfprofessionsetting->GetAttributeValue( name );

                    // 初始属性
                    auto value = CalcHeroInitAttribute( kffighter, racevalue, professionvalue, 100u, level );

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

        // 音色数据
        GenerateVoice( player, kfnpc, kfprofessionsetting );

        // 设置主动技能
        auto kfactiverecord = kfnpc->Find( __STRING__( active ) );
        if ( kfactiverecord->Size() > 0u )
        {
            auto activeindex = kfactiverecord->First()->Get<uint32>( __STRING__( id ) );
            kfnpc->Set( __STRING__( activeindex ), activeindex );
        }

        return kfnpc;
    }

    uint64 KFGenerateModule::CalcFinalPreference( uint32 racevalue, uint32 professionvalue, uint32 growthvalue )
    {
        // 最终属性偏好率=（角色_属性偏好率+职业_属性偏好率）*属性成长率
        uint64 value = ( uint64 )( racevalue + professionvalue ) * ( uint64 )growthvalue;
        return value;
    }

    uint32 KFGenerateModule::CalcHeroInitAttribute( KFData* kffighter, uint32 racevalue, uint32 professionvalue, uint32 growthvalue, uint32 level )
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

        return totalvalue;
    }

    void KFGenerateModule::HeroAutoUpgradeAndTransfer( KFEntity* player, KFData* kfhero, uint32 herolevel, uint32 classlv, uint32& professionid, std::vector<KFGenerateData>& generatelist )
    {
        if ( classlv > 1u )
        {
            // 高阶职业英雄倒推升级转职流程
            KFGenerateData generatedata;

            auto endpro = professionid;
            for ( uint32 i = 1u; i < classlv; i++ )
            {
                auto beginpro = KFTransferConfig::Instance()->GetRandParentId( endpro );
                if ( beginpro == _invalid_int )
                {
                    break;
                }

                auto kfprofessionsetting = KFProfessionConfig::Instance()->FindSetting( beginpro );
                if ( herolevel > kfprofessionsetting->_max_level )
                {
                    // 升级
                    generatedata.type = 0u;
                    generatedata.beginpro = endpro;
                    generatedata.endpro = endpro;
                    generatedata.beginlevel = kfprofessionsetting->_max_level;
                    generatedata.endlevel = herolevel;

                    generatelist.emplace( generatelist.begin(), generatedata );
                    herolevel = kfprofessionsetting->_max_level;
                }

                // 转职
                generatedata.type = 1u;
                generatedata.beginpro = beginpro;
                generatedata.endpro = endpro;
                generatedata.beginlevel = herolevel;
                generatedata.endlevel = herolevel;

                generatelist.emplace( generatelist.begin(), generatedata );
                endpro = beginpro;

                professionid = beginpro;
            }

            if ( herolevel > 1u )
            {
                generatedata.type = 0u;
                generatedata.beginpro = professionid;
                generatedata.endpro = professionid;
                generatedata.beginlevel = 1u;
                generatedata.endlevel = herolevel;

                generatelist.emplace( generatelist.begin(), generatedata );
            }
        }
        else if ( herolevel > 1u )
        {
            // 高等级英雄顺推升级转职流程
            KFGenerateData generatedata;

            auto beginpro = professionid;
            auto beginlevel = 1u;

            auto transferlevel = KFTransferConfig::Instance()->GetProTransferLevel( professionid );
            while ( herolevel > transferlevel )
            {

                generatedata.type = 0u;
                generatedata.beginpro = beginpro;
                generatedata.endpro = beginpro;
                generatedata.beginlevel = beginlevel;
                generatedata.endlevel = transferlevel;

                generatelist.emplace_back( generatedata );
                beginlevel = transferlevel;

                // 再进行转职
                auto transferpro = KFTransferConfig::Instance()->GetRandChildId( beginpro );
                if ( transferpro == _invalid_int )
                {
                    break;
                }

                generatedata.type = 1u;
                generatedata.beginpro = beginpro;
                generatedata.endpro = transferpro;
                generatedata.beginlevel = beginlevel;
                generatedata.endlevel = beginlevel;

                generatelist.emplace_back( generatedata );
                beginpro = transferpro;

                transferlevel = KFTransferConfig::Instance()->GetProTransferLevel( beginpro );
                if ( transferlevel == _invalid_int )
                {
                    break;
                }
            }

            auto kfprosetting = KFProfessionConfig::Instance()->FindSetting( beginpro );
            if ( kfprosetting != nullptr && herolevel > kfprosetting->_max_level )
            {
                // 英雄等级不超过职业最大等级
                herolevel = kfprosetting->_max_level;
            }

            if ( herolevel > beginlevel )
            {
                generatedata.type = 0u;
                generatedata.beginpro = beginpro;
                generatedata.endpro = beginpro;
                generatedata.beginlevel = beginlevel;
                generatedata.endlevel = herolevel;

                generatelist.emplace_back( generatedata );
            }
        }
    }

    __KF_UPDATE_DATA_FUNCTION__( KFGenerateModule::OnHeroLevelUpdateCallBack )
    {
        OnHeroLevelUpdate( player, kfdata->GetParent(), oldvalue, newvalue );
    }

    void KFGenerateModule::OnHeroLevelUpdate( KFEntity* player, KFData* kfhero, uint32 oldvalue, uint32 newvalue, bool update )
    {
        // 只有升级才改变经验
        if ( newvalue <= oldvalue )
        {
            return;
        }

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

        // 添加等级技能和天赋
        UInt32Vector active_pool_list;
        UInt32Vector innate_pool_list;

        for ( auto level = oldvalue + 1u; level <= newvalue; ++level )
        {
            // 获得升级后的配置
            auto kflevelsetting = KFLevelConfig::Instance()->FindSetting( level );
            if ( kflevelsetting == nullptr )
            {
                __LOG_ERROR__( "level=[{}] setting can't find", level );
                continue;
            }

            active_pool_list.insert( active_pool_list.end(), kflevelsetting->_active_pool_list.begin(), kflevelsetting->_active_pool_list.end() );
            innate_pool_list.insert( innate_pool_list.end(), kflevelsetting->_innate_pool_list.begin(), kflevelsetting->_innate_pool_list.end() );

            for ( auto kfchild = kffighter->First(); kfchild != nullptr; kfchild = kffighter->Next() )
            {
                auto& attrname = kfchild->_data_setting->_name;
                if ( attrname == __STRING__( hp ) || attrname == __STRING__( ep ) )
                {
                    continue;
                }

                auto growthvalue = kfgrowth->Get<uint32>( attrname );
                auto racevalue = kfracesetting->GetAttributeValue( attrname );
                auto professionvalue = kfprofessionsetting->GetAttributeValue( attrname );

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
                if ( update )
                {
                    player->UpdateData( kfchild, KFEnum::Add, addvalue );
                }
                else
                {
                    kfchild->Operate( KFEnum::Add, addvalue );
                }

                if ( attrname == __STRING__( maxhp ) )
                {
                    // 升级当前hp增加为maxhp增加数量
                    if ( update )
                    {
                        player->UpdateData( kffighter, __STRING__( hp ), KFEnum::Add, addvalue );

                    }
                    else
                    {
                        auto kfhp = kffighter->Find( __STRING__( hp ) );
                        kfhp->Operate( KFEnum::Add, addvalue );
                    }
                }
            }
        }

        // 随机主动技能
        RandWeightData( player, kfhero, __STRING__( active ), active_pool_list, update );

        // 随机天赋
        auto randlist = RandWeightData( player, kfhero, __STRING__( innate ), innate_pool_list, update );
        if ( !randlist.empty() )
        {
            auto posflag = kfhero->Get<uint32>( __STRING__( posflag ) );
            if ( posflag == KFMsg::TrainBuild )
            {
                update = false;
            }
            UpdateInnateData( player, kfhero, randlist, update );
        }
    }

    __KF_UPDATE_DATA_FUNCTION__( KFGenerateModule::OnHeroProUpdateCallBack )
    {
        OnHeroProUpdate( player, kfdata->GetParent(), oldvalue, newvalue );
    }

    void KFGenerateModule::OnHeroProUpdate( KFEntity* player, KFData* kfhero, uint32 oldvalue, uint32 newvalue, bool update )
    {
        auto kfsetting = KFTransferConfig::Instance()->FindTransferSetting( newvalue, oldvalue );
        if ( kfsetting == nullptr )
        {
            return __LOG_ERROR__( "transfer setting can't find, childid=[{}], parentid=[{}] ", newvalue, oldvalue );
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

        auto race = kfhero->Get( __STRING__( race ) );
        auto kfracesetting = KFRaceConfig::Instance()->FindSetting( race );
        if ( kfracesetting == nullptr )
        {
            return __LOG_ERROR__( "transfer profession setting=[{}] can't find", newvalue );
        }

        static auto _transfer_option = KFGlobal::Instance()->FindConstant( "transferleveloffest" );
        static auto _direct_option = KFGlobal::Instance()->FindConstant( "generatedirectattribute" );
        static auto _random_option = KFGlobal::Instance()->FindConstant( "generaterandattribute" );

        // 更新属性fighter
        auto kffighter = kfhero->Find( __STRING__( fighter ) );
        auto kfgrowth = kfhero->Find( __STRING__( growth ) );
        auto level = kfhero->Get<uint32>( __STRING__( level ) );

        auto reallevel = level + _direct_option->_uint32_value + _random_option->_uint32_value - 1u;
        auto oldreallevel = reallevel + _transfer_option->_uint32_value * ( oldprosetting->_class_lv - 1u );
        auto newreallevel = reallevel + _transfer_option->_uint32_value * ( newprosetting->_class_lv - 1u );

        for ( auto kfchild = kffighter->First(); kfchild != nullptr; kfchild = kffighter->Next() )
        {
            auto& attrname = kfchild->_data_setting->_name;
            if ( attrname == __STRING__( hp ) || attrname == __STRING__( ep ) )
            {
                continue;
            }

            auto growthvalue = kfgrowth->Get<uint32>( attrname );
            auto racevalue = kfracesetting->GetAttributeValue( attrname );

            // 新旧职业的基础偏好率
            auto oldprovalue = oldprosetting->GetAttributeValue( attrname );
            auto newprovalue = newprosetting->GetAttributeValue( attrname );

            // 最终成长率
            auto oldfinalpreference = CalcFinalPreference( racevalue, oldprovalue, growthvalue ) / KFRandEnum::Hundred;
            auto newfinalpreference = CalcFinalPreference( racevalue, newprovalue, growthvalue ) / KFRandEnum::Hundred;

            // 计算属性修正值
            auto oldattrvalue = oldfinalpreference * oldreallevel;
            auto newattrvalue = newfinalpreference * newreallevel;
            auto value = ( newattrvalue > oldattrvalue ) ? ( newattrvalue - oldattrvalue ) : 0u;

            auto dvalue = static_cast<double>( value ) / KFRandEnum::Hundred;
            auto addvalue = static_cast<int>( dvalue );

            // 增加最终属性
            if ( update )
            {
                player->UpdateData( kfchild, KFEnum::Add, addvalue );
            }
            else
            {
                kfchild->Operate( KFEnum::Add, addvalue );
            }
        }

        auto oldweapontype = kfhero->Get( __STRING__( weapontype ) );
        auto newweapontype = oldweapontype;
        if ( !newprosetting->IsWeaponTypeValid( newweapontype ) )
        {
            // 随机新的武器类型
            newweapontype = newprosetting->RandWeapontype();
        }

        // 随机主动技能
        auto randlist = RandWeightData( player, kfhero, __STRING__( active ), kfsetting->_active_pool_list, update );
        if ( update && !randlist.empty() )
        {
            // 转职主动技能更新为最新的技能
            player->UpdateData( kfhero, __STRING__( activeindex ), KFEnum::Set, *randlist.rbegin() );
        }

        // 随机性格
        RandWeightData( player, kfhero, __STRING__( character ), kfsetting->_character_pool_list, update );

        // 随机天赋
        randlist = RandWeightData( player, kfhero, __STRING__( innate ), kfsetting->_innate_pool_list, update );
        if ( !randlist.empty() )
        {
            UpdateInnateData( player, kfhero, randlist, update );
        }

        auto kftransferrecord = kfhero->Find( __STRING__( transfer ) );
        auto kftransfer = player->CreateData( kftransferrecord );
        auto index = kftransferrecord->Size() + 1;
        kftransfer->Set( __STRING__( index ), index );
        kftransfer->Set( __STRING__( profession ), oldvalue );
        kftransfer->Set( __STRING__( weapontype ), oldweapontype );

        auto maxhp = kffighter->Get<uint32>( __STRING__( maxhp ) );
        auto maxlevel = _kf_hero->CalcMaxLevel( player, kfhero );
        if ( update )
        {
            // 保存之前的职业信息
            player->AddData( kftransferrecord, index, kftransfer );

            // 更新武器类型
            if ( newweapontype != oldweapontype )
            {
                player->UpdateData( kfhero, __STRING__( weapontype ), KFEnum::Set, newweapontype );
            }

            // 将血量回满
            player->UpdateData( kffighter, __STRING__( hp ), KFEnum::Set, maxhp );

            // 更新最大等级
            player->UpdateData( kfhero, __STRING__( maxlevel ), KFEnum::Set, maxlevel );

            // 更新职业等级
            player->UpdateData( kfhero, __STRING__( classlv ), KFEnum::Set, newprosetting->_class_lv );
        }
        else
        {
            kftransferrecord->Add( index, kftransfer );
            kfhero->Set( __STRING__( weapontype ), newweapontype );
            kffighter->Set( __STRING__( hp ), maxhp );
            kfhero->Set( __STRING__( maxlevel ), maxlevel );
            kfhero->Set( __STRING__( classlv ), newprosetting->_class_lv );
        }
    }

    UInt32Vector& KFGenerateModule::RandWeightData( KFEntity* player, KFData* kfhero, const std::string& dataname, const UInt32Vector& slist, bool update /*= true*/ )
    {
        static UInt32Vector randlist;
        randlist.clear();

        if ( slist.empty() )
        {
            return randlist;
        }

        auto kfdatarecord = kfhero->Find( dataname );
        if ( kfdatarecord == nullptr || kfdatarecord->IsFull() )
        {
            return randlist;
        }

        UInt32Set excludelist;
        for ( auto kfdata = kfdatarecord->First(); kfdata != nullptr; kfdata = kfdatarecord->Next() )
        {
            excludelist.insert( kfdata->Get<uint32>( kfdatarecord->_data_setting->_key_name ) );
        }

        static UInt32Set includelist;
        auto movetype = 0u;
        auto race = kfhero->Get( __STRING__( race ) );
        auto sex = kfhero->Get( __STRING__( sex ) );
        auto professionid = kfhero->Get( __STRING__( profession ) );
        auto backgroundid = kfhero->Get( __STRING__( background ) );
        auto weapontype = kfhero->Get( __STRING__( weapontype ) );

        auto quality = kfhero->Get<uint32>( __STRING__( quality ) );
        auto minquality = 0u;
        auto maxquality = 0u;

        auto kfqualitysetting = KFQualityConfig::Instance()->FindSetting( quality );
        if ( kfqualitysetting != nullptr )
        {
            if ( dataname == __STRING__( active ) )
            {
                minquality = kfqualitysetting->_active_quality._min_value;
                maxquality = kfqualitysetting->_active_quality._max_value;
            }
            else if ( dataname == __STRING__( innate ) )
            {
                minquality = kfqualitysetting->_innate_quality._min_value;
                maxquality = kfqualitysetting->_innate_quality._max_value;
            }
        }

        for ( auto poolid : slist )
        {
            auto randid = 0u;
            if ( dataname == __STRING__( active ) || dataname == __STRING__( innate ) )
            {
                __RAND_WEIGHT_DATA__( poolid, KFSkillConfig::Instance(), IsValid( race, professionid, backgroundid, weapontype, minquality, maxquality ), randid, includelist );
            }
            else if ( dataname == __STRING__( character ) )
            {
                __RAND_WEIGHT_DATA__( poolid, KFCharacterConfig::Instance(), IsValid( race, backgroundid, professionid ), randid, includelist );
            }

            if ( randid == 0u )
            {
                continue;
            }

            randlist.emplace_back( randid );
            excludelist.insert( randid );

            auto kfdata = player->CreateData( kfdatarecord );
            kfdata->Set( __STRING__( level ), 1u );

            if ( update )
            {
                player->AddData( kfdatarecord, randid, kfdata );
            }
            else
            {
                kfdatarecord->Add( randid, kfdata );
            }

            if ( kfdatarecord->IsFull() )
            {
                break;
            }
        }

        return randlist;
    }

    void KFGenerateModule::UpdateInnateData( KFEntity* player, KFData* kfhero, const UInt32Vector& randlist, bool update )
    {
        auto quality = kfhero->Get<uint32>( __STRING__( quality ) );
        auto kfqualitysetting = KFQualityConfig::Instance()->FindSetting( quality );
        if ( kfqualitysetting == nullptr )
        {
            return;
        }

        auto kfinnaterecord = kfhero->Find( __STRING__( innate ) );
        auto innatenum = kfinnaterecord->Size();
        if ( innatenum <= kfqualitysetting->_innate_num )
        {
            return;
        }

        if ( update )
        {
            for ( auto iter : randlist )
            {
                // 新增天赋设置标记
                player->UpdateData( kfinnaterecord, iter, __STRING__( flag ), KFEnum::Set, 1u );
            }
        }
        else
        {
            // 数据不更新，删除超过的技能数量
            auto removenum = innatenum - kfqualitysetting->_innate_num;

            UInt32Set randset;
            for ( auto iter : randlist )
            {
                randset.insert( iter );
            }

            // 英雄有AB天赋，新增CDE天赋，最后装备CD天赋
            UInt32Vector removelist;
            for ( auto kfinnate = kfinnaterecord->First(); kfinnate != nullptr; kfinnate = kfinnaterecord->Next() )
            {
                auto innateid = kfinnate->Get<uint32>( kfinnaterecord->_data_setting->_key_name );
                if ( randset.find( innateid ) == randset.end() )
                {
                    removelist.emplace_back( innateid );
                    if ( removelist.size() >= removenum )
                    {
                        break;
                    }
                }
            }

            if ( removelist.size() < removenum )
            {
                removelist.insert( removelist.end(), randlist.rbegin(), randlist.rbegin() + removenum - removelist.size() );
            }

            auto posflag = kfhero->Get<uint32>( __STRING__( posflag ) );
            if ( posflag == KFMsg::TrainBuild )
            {
                // 在训练所升级更新数据
                update = true;
            }

            for ( auto iter : removelist )
            {
                if ( update )
                {
                    player->RemoveData( kfinnaterecord, iter );
                }
                else
                {
                    kfinnaterecord->Remove( iter );
                }
            }
        }
    }

    void KFGenerateModule::RandInjuryData( KFEntity* player, KFData* kfhero, uint32 poolid )
    {
        auto kfinjuryrecord = kfhero->Find( __STRING__( injury ) );

        UInt32Set excludelist;

        if ( kfinjuryrecord->IsFull() )
        {
            // 伤病部位已满
            for ( auto& iter : KFInjuryConfig::Instance()->_settings._objects )
            {
                auto kfinjury = kfinjuryrecord->Find( iter.second->_id );
                if ( kfinjury == nullptr )
                {
                    excludelist.insert( iter.second->_id );
                }
            }
        }

        for ( auto kfinjury = kfinjuryrecord->First(); kfinjury != nullptr; kfinjury = kfinjuryrecord->Next() )
        {
            // 剔除满级的伤病
            auto id = kfinjury->Get<uint32>( __STRING__( id ) );
            auto level = kfinjury->Get<uint32>( __STRING__( level ) );

            auto kfinjurysetting = KFInjuryConfig::Instance()->FindSetting( id );
            if ( level >= kfinjurysetting->_max_level )
            {
                excludelist.insert( id );
            }
        }

        auto weightpool = KFWeightConfig::Instance()->FindWeightPool( poolid );
        if ( weightpool == nullptr )
        {
            return;
        }

        auto weightdata = weightpool->Rand( excludelist, true );
        if ( weightdata == nullptr )
        {
            return;
        }

        auto randid = weightdata->_id;
        auto kfinjurysetting = KFInjuryConfig::Instance()->FindSetting( randid );
        if ( kfinjurysetting == nullptr )
        {
            return;
        }

        auto kfinjury = kfinjuryrecord->Find( randid );
        if ( kfinjury == nullptr )
        {
            auto kfinjury = player->CreateData( kfinjuryrecord );
            kfinjury->Set( __STRING__( level ), 1u );

            player->AddData( kfinjuryrecord, randid, kfinjury );
        }
        else
        {
            player->UpdateData( kfinjury, __STRING__( level ), KFEnum::Add, 1u );
        }
    }
}