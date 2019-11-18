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
#include "KFGenerateConfig.hpp"
#include "KFPinchFaceConfig.hpp"
#include "KFNpcConfig.hpp"
#include "KFNpcSkillConfig.hpp"
#include "KFZConfig/KFRaceConfig.hpp"
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
        // 生成英雄
        virtual KFData* GeneratePlayerHero( KFEntity* player, KFData* kfhero, uint32 generateid, bool usedivisor, const char* function, uint32 line );

        // 生成npc英雄
        virtual KFData* GenerateNpcHero( KFEntity* player, uint32 generateid, uint32 level, const char* function, uint32 line );
    protected:
        // 英雄等级更新
        __KF_UPDATE_DATA_FUNCTION__( OnHeroLevelUpdate );

        // 英雄职业更新
        __KF_UPDATE_DATA_FUNCTION__( OnHeroProfessionUpdate );
        ///////////////////////////////////////////////////////////////////////
        ///////////////////////////////////////////////////////////////////////
        // 生成种族
        uint32 GenerateRace( KFEntity* player, uint32 racepoolid, bool usedivisor, const char* function, uint32 line );

        // 随机招募因子
        uint32 RandDivisorByType( KFEntity* player, uint32 type, const KFWeightList< KFWeight >* weightpool );

        // 计算移动方式和武器类型
        std::tuple< uint32, uint32 > CalcDivisor( KFEntity* player, uint32 race, uint32 sex, uint32 professionpoolid );

        // 捏脸数据
        void GeneratePinchFace( KFData* kfhero );

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 计算最终属性偏好率
        uint64 CalcFinalPreference( uint32 racevalue, uint32 professionvalue, uint32 growthvalue );

        // 计算初始属性
        uint32 CalcHeroInitAttribute( KFData* kffighter, uint32 initvalue, uint32 racevalue, uint32 professionvalue, uint32 growthvalue );

        // 随机权重数据
        void RandWeightData( KFEntity* player, KFData* kfhero, const std::string& str, const VectorUInt32& slist, const char* function, uint32 line );
    private:
        // 玩家组件上下文
        KFComponent* _kf_component = nullptr;
    };
}



#endif