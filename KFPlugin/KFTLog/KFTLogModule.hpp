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
#include "KFTLogInterface.h"
#include "KFPlayer/KFPlayerInterface.h"
#include "KFKernel/KFKernelInterface.h"
#include "KFLogger/KFSpdLog.h"

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
        // 注册玩家
        __KF_NEW_PLAYER_FUNCTION__( OnNewTLogModule );

        // 玩家进入游戏
        __KF_ENTER_PLAYER_FUNCTION__( OnEnterTLogModule );

        // 玩家离开游戏
        __KF_ENTER_PLAYER_FUNCTION__( OnLeaveTLogModule );

        // 打印货币
        __KF_LOG_ELEMENT_FUNCTION__( LogCurrenyElement );

        // 打印道具
        __KF_LOG_ELEMENT_FUNCTION__( LogItemElement );

    protected:
        template<typename... P>
        void TLog( const std::string& myfmt, P&& ... args )
        {
            auto content = __FORMAT__( myfmt, std::forward<P>( args )... );
            _spdlog->Log( KFLogEnum::Info, content );
        }

    protected:
        // 玩家组件上下文
        KFComponent* _kf_component = nullptr;

        // 日志逻辑
        KFSpdLog* _spdlog = nullptr;
    };
}



#endif