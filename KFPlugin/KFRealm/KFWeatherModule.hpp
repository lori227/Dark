#ifndef __KF_WEATHER_MOUDLE_H__
#define __KF_WEATHER_MOUDLE_H__

/************************************************************************
//    @Module			:    秘境天气系统
//    @Author           :    __凌_痕__
//    @QQ				:    7969936
//    @Mail			    :    lori227@qq.com
//    @Date             :    2020-3-19
************************************************************************/

#include "KFrameEx.h"
#include "KFRealmData.hpp"
#include "KFPVEInterface.h"
#include "KFRealmInterface.h"
#include "KFWeatherInterface.h"
#include "KFKernel/KFKernelInterface.h"
#include "KFPlayer/KFPlayerInterface.h"
#include "KFMessage/KFMessageInterface.h"
#include "KFZConfig/KFRealmConfig.hpp"
#include "KFZConfig/KFPVEConfig.hpp"
#include "KFZConfig/KFWeatherConfig.hpp"

namespace KFrame
{
    ///////////////////////////////////////////////////////////////////////////////////////
    class KFWeatherModule : public KFWeatherInterface
    {
    public:
        KFWeatherModule() = default;
        ~KFWeatherModule() = default;

        // 逻辑
        virtual void BeforeRun();

        // 关闭
        virtual void BeforeShut();

    protected:
        // 秘境进入
        __KF_REALM_ENTER_FUNCTION__( OnReamlEnterWeatherModule );

        // 秘境移动
        __KF_REALM_MOVE_FUNCTION__( OnRealmMoveWeatherModule );

        // 战斗开始
        __KF_PVE_START_FUNCTION__( OnPVEStartWeatherModule );

        // 战斗结束
        __KF_PVE_FINISH_FUNCTION__( OnPVEFinishWeatherModule );

        // 回合开始
        __KF_TURN_START_FUNCTION__( OnPVETurnStartWeatherModule );

    protected:
        // 请求更新天气
        __KF_MESSAGE_FUNCTION__( HandleUpdateWeatherReq );

    protected:
        // 获得天气配置
        const KFWeatherSetting* FindWeatherSetting( KFRealmData* kfrealmdata, KFRealmData* kfpvedata );

        // 发送更新天气消息
        void SendUpdateWeatherToClient( KFEntity* player, KFRealmData* kfrealmdata );

        // 设置下一次天气
        void RandNextWeatherData( KFRealmData* kfrealmdata, const KFWeatherSetting* kfweathersetting );
    };
}

#endif