#ifndef __KF_GENTERATE_MOUDLE_H__
#define __KF_GENTERATE_MOUDLE_H__

/************************************************************************
//    @Moudle			:    英雄生成系统
//    @Author           :    __凌_痕__
//    @QQ				:    7969936
//    @Mail			    :    lori227@qq.com
//    @Date             :    2019-7-19
************************************************************************/
#include "KFrameEx.h"
#include "KFGenerateInterface.h"
#include "KFItem/KFItemInterface.h"
#include "KFOption/KFOptionInterface.h"
#include "KFKernel/KFKernelInterface.h"
#include "KFZConfig/KFRaceConfig.hpp"
#include "KFZConfig/KFNpcConfig.hpp"
#include "KFZConfig/KFNpcSkillConfig.hpp"
#include "KFZConfig/KFPinchFaceConfig.hpp"
#include "KFZConfig/KFGenerateConfig.hpp"
#include "KFZConfig/KFProfessionConfig.hpp"
#include "KFZConfig/KFBackGroundConfig.hpp"
#include "KFZConfig/KFCharacterConfig.hpp"
#include "KFZConfig/KFSkillConfig.hpp"
#include "KFZConfig/KFHeroAttrConfig.hpp"
#include "KFZConfig/KFWeightConfig.hpp"
#include "KFZConfig/KFDivisorConfig.hpp"
#include "KFZConfig/KFLevelConfig.hpp"
#include "KFZConfig/KFItemConfig.hpp"
#include "KFZConfig/KFTransferConfig.hpp"
#include "KFZConfig/KFWeaponTypeConfig.hpp"

namespace KFrame
{
    class KFGenerateModule : public KFGenerateInterface
    {
    public:
        KFGenerateModule() = default;
        ~KFGenerateModule() = default;

        virtual void BeforeRun();
        virtual void BeforeShut();
        ////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////
        // 生成玩家英雄
        virtual KFData* GeneratePlayerHero( KFEntity* player, KFData* kfhero, uint32 generateid );
        virtual KFData* GeneratePlayerHero( KFEntity* player, KFData* kfhero, uint32 generateid,
                                            const DivisorList& divisorlist, const UInt32Set& professionlist,
                                            uint32 generatelevel, uint32 mingrowth, uint32 maxgrowth );

        // 生成npc英雄
        virtual KFData* GenerateNpcHero( KFEntity* player, KFData* kfnpcrecord, uint32 generateid, uint32 level );

        // 随机权重数据
        virtual uint32 RandWeightData( KFEntity* player, KFData* kfhero, const std::string& str, const UInt32Vector& slist );
    protected:
        // 英雄等级更新
        __KF_UPDATE_DATA_FUNCTION__( OnHeroLevelUpdate );

        // 英雄职业更新
        __KF_UPDATE_DATA_FUNCTION__( OnHeroProfessionUpdate );
        ///////////////////////////////////////////////////////////////////////
        ///////////////////////////////////////////////////////////////////////
        KFData* GenerateHero( KFEntity* player, KFData* kfhero, uint32 generateid,
                              const DivisorList& divisorlist, const UInt32Set& professionlist,
                              uint32 level, uint32 mingrowth, uint32 maxgrowth );

        // 计算种族
        uint32 CalcRace( uint32 racepoolid, const DivisorList& divisorlist, const std::map< uint32, const KFProfessionSetting* >& professionlist );

        // 计算性别
        uint32 CalcSex( uint32 sexpoolid );

        // 计算移动方式
        uint32 CalcMove( const DivisorList& divisorlist, const std::map< uint32, const KFProfessionSetting* >& professionlist );

        // 计算移动方式
        uint32 CalcWeapon( const DivisorList& divisorlist, const std::map< uint32, const KFProfessionSetting* >& professionlist );

        // 计算移动方式和武器类型
        std::tuple< uint32, uint32 > CalcMoveAndWeapon( KFEntity* player, uint32 professionpoolid, uint32 race, uint32 sex,
                const DivisorList& divisorlist, const std::map< uint32, const KFProfessionSetting* >& professionlist );

        // 捏脸数据
        void GeneratePinchFace( KFEntity* player, KFData* kfhero );

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 计算最终属性偏好率
        uint64 CalcFinalPreference( uint32 racevalue, uint32 professionvalue, uint32 growthvalue );

        // 计算初始属性
        uint32 CalcHeroInitAttribute( KFData* kffighter, uint32 initvalue, uint32 racevalue, uint32 professionvalue, uint32 growthvalue, uint32 level );

    private:
        // 玩家组件上下文
        KFComponent* _kf_component = nullptr;
    };
}



#endif