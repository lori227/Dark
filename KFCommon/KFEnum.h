﻿#ifndef __KF_ENUM_H__
#define __KF_ENUM_H__

namespace KFrame
{
    namespace KFEnum
    {
        enum MyEnum
        {
            Error = 0,	// 错误
            Ok = 1,		// 成功

            //////////////////////////////////////
            // 操作类型
            Null = 0,
            Add = 1,		// +
            Dec = 2,		// -
            Set = 3,		// =
            And = 4,		// &
            Or  = 5,		// |
            Xor = 6,		// &~
            Mul = 7,		// *
            Div = 8,		// /

            Greater = 11,		// >
            Less = 12,			// <
            Equal = 13,			// ==
            GreaterEqual = 14,	// >=
            LessEqual = 15,		// <=
            UnEqual = 16,		// !=
            //////////////////////////////////////
        };
    }

    ////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////
    // 消息类型
    namespace KFMessageEnum
    {
        enum MyEnum
        {
            Server = 1,		// 服务器与服务器之间消息
            Client = 2,		// 客户端与服务器之间消息
        };
    }
    ////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////

    namespace KFServerEnum
    {
        enum MyEnum
        {
            TotalServerList = 1,	// 所有服务器列表
            SelectServerData = 2,	// 选择服务器登录
            /////////////////////////////////////////////////////////////////////

            Local = 1,			// 内网
            Internet = 2,		// 外网
            /////////////////////////////////////////////////////////////////////

            Debug = 1,			// 调试
            Develop = 2,		// 研发
            Beta = 3,			// 测试
            Audit = 4,			// 审核
            Online = 8,			// 正式
            /////////////////////////////////////////////////////////////////////
            /////////////////////////////////////////////////////////////////////
            DeployTool = 10,	// 部署tool
            DeployServer = 11,	// 部署server
            DeployAgent = 12,	// 部署Agent

            LogShard = 20,		// 日志服务器
            Auth = 21,			// 认证服务器

            RouteMaster = 22,	// 转发服务器master
            RouteProxy = 23,	// 转发服务器proxy
            RouteShard = 24,	// 转发服务器shard

            //////////////////////////////////////////////////////////////////////
            Data = 31,			// 数据服务器
            Public = 32,		// 公共属性服务器
            Relation = 33,		// 关系服务器shard
            Mail = 34,			// 邮件服务器shard
            Rank = 35,			// 排行榜服务器shard
            Team = 36,			// 组队服务器shard
            Guild = 37,			// 公会服务器shard
            Match = 38,			// 匹配服务器shard
            Room = 39,			// 房间服务器shard
            Battle = 40,		// 战斗服务器

            ZoneMaster = 91,	// 小区服务器master
            ZoneWorld = 92,		// 小区服务器world
            ZoneGate = 93,		// 小区服务器gate
            ZoneLogin = 94,		// 小区服务器login
            ZoneGame = 95,		// 小区服务器game
            /////////////////////////////////////////////////////////////////////

        };
    }

    namespace KFThreadEnum
    {
        enum MyEnum
        {
            ST = 1,		// 单线程
            MT = 2,		// 多线程
        };
    }

    namespace KFBufferEnum
    {
        enum MyEnum
        {
            Buff_1M = 1024 * 1024,		// buff大小
            Buff_10M = 10 * Buff_1M,
            Buff_20M = 20 * Buff_1M,
            Buff_40M = 40 * Buff_1M,
            Buff_100M = 100 * Buff_1M,
            Buff_200M = 200 * Buff_1M,
        };
    }
    //////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////
    namespace KFRankEnum
    {
        enum MyEnum
        {
            TotalRank = 1,		// 全区全服排行榜
            ZoneRank = 2,		// 分区排行榜
        };
    }
    //////////////////////////////////////////////////////////////////////////////////////////////////
    namespace KFRandEnum
    {
        enum MyEnum
        {
            Hundred = 100,
            Thousand = 1000,
            TenThousand = 10000,
            //////////////////////////////////////////////////
            Weight = 1,			// 权重随机
            Probability = 2,	// 概率随机
        };
    }
    //////////////////////////////////////////////////////////////////////////////////////////////////
    namespace KFCompressEnum
    {
        enum MyEnum
        {
            None = 0x0,			// 无操作
            Convert = 0x1,		// 转换
            Zib = 0x2,			// 压缩
            Compress = 0x3,		// 压缩转换
        };
    }
    //////////////////////////////////////////////////////////////////////////////////////////////////
    namespace KFDatabaseEnum
    {
        enum MyEnum
        {
            Write = 1,	// 写数据
            Read = 2,	// 读数据
        };
    }
    //////////////////////////////////////////////////////////////////////////////////////////////////
    namespace KFSaveEnum
    {
        enum MyEnum
        {
            UpdateSave = 1,	    // 更新保存数据
            OfflineSave = 2,	// 离线保存数据
        };
    }
    //////////////////////////////////////////////////////////////////////////////////////////////////
    namespace KFConfigEnum
    {
        enum MyEnum
        {
            None = 0x0,				// 无操作
            CanReload = 0x1,	    // 支持重新加载
            NeedClearData = 0x2,	// 加载需要清除数据
            ClearFileData = 0x4,	// 清空文件数据
        };
    }
    //////////////////////////////////////////////////////////////////////////////////////////////////
    enum KFConditionEnum
    {
        // 使用数据
        UseSetting = 0,		// 使用配置属性值
        UseOperate = 1,		// 使用操作属性值
        UseFinal = 2,		// 使用最终属性值
        UseCalc = 3,		// 使用初始计算值
        //////////////////////////////////////////////////////////////////
        // 更新数据
        UpdateFailed = 0,	// 更新条件失败
        UpdateLimit = 1,	// 更新条件限制
        UpdateOk = 2,		// 更新条件完成
        UpdateDone = 3,		// 更新条件完成
        //////////////////////////////////////////////////////////////////
        // 初始值计算方式
        InitNull = 0,		// 不初始化
        InitCalcCount = 1,	// 统计数量
        InitGetValue = 2,	// 获得属性值
        InitCheckExist = 3,	// 是否存在
        InitCalcValue = 4,	// 统计数值

        //////////////////////////////////////////////////////////////////
        AddCall = 1,		// 添加时回调
        RemoveCall = 2,		// 删除时回调
        UpdateCall = 3,		// 更新时回调
        //////////////////////////////////////////////////////////////////
        CalculateNormal = 0,	// 完成后停止计数
        CalculateContinue = 1,	// 持续计数
        //////////////////////////////////////////////////////////////////
        // 条件限制类型
        LimitNull = 0x0,		// 无限制
        LimitStatus = 0x1,		// 完成后继续计数
        LimitPlace = 0x2,		// 地点限制
        LimitStop = 0xFFFF,		// 限制停止
    };
}

#endif