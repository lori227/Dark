#ifndef __KF_TLOG_MODULE_H__
#define __KF_TLOG_MODULE_H__

/************************************************************************
//    @Module			:    TLog模块
//    @Author           :    __凌_痕__
//    @QQ				:    7969936
//    @Mail			    :    lori227@qq.com
//    @Date             :    2019-12-18
************************************************************************/

#include "KFrameEx.h"
#include "KFLogger/KFSpdLog.h"
#include "KFTLogInterface.h"
#include "KFTimer/KFTimerInterface.h"
#include "KFPlayer/KFPlayerInterface.h"
#include "KFKernel/KFKernelInterface.h"
#include "KFIpAddress/KFIpAddressInterface.h"
#include "KFTLogConfig.hpp"
#include "KFZConfig/KFItemConfig.hpp"
#include "KFZConfig/KFCurrencyConfig.hpp"

namespace KFrame
{
    class KFTLogModule : public KFTLogInterface
    {
    public:
        KFTLogModule() = default;
        ~KFTLogModule() = default;

        // 初始化
        virtual void BeforeRun();

        // 关闭
        virtual void ShutDown();

    protected:
        // 打印服务器状态
        __KF_TIMER_FUNCTION__( OnTimerLogServerStatus );

        // 打印在线总数
        __KF_TIMER_FUNCTION__( OnTimerLogOnlineCount );

        // 玩家进入游戏
        __KF_ENTER_PLAYER_FUNCTION__( OnEnterTLogModule );

        // 玩家离开游戏
        __KF_ENTER_PLAYER_FUNCTION__( OnLeaveTLogModule );

        // 打印货币
        __KF_LOG_ELEMENT_FUNCTION__( LogCurrencyElement );

        // 打印道具
        __KF_LOG_ELEMENT_FUNCTION__( LogItemElement );

        // 创建角色
        __KF_UPDATE_STRING_FUNCTION__( OnUpdateNameLogCreateRole );

    protected:
        // 创建日志
        KFSpdLog* CreateLog( const std::string& name );

        // 初始化log数据
        void InitTLogData();

        // 打印在线时长
        void LogOnlineTime( KFEntity* player );

        // 打印登录游戏
        void LogLeaveGame( KFEntity* player );

    protected:
        // 玩家组件上下文
        KFComponent* _kf_component = nullptr;

        // 游戏appid
        std::string _game_app_id;

        // 日志版本号
        uint32 _log_version;
    };
}



#endif