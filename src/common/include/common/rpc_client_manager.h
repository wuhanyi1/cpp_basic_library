/*
 * @Author: wuhanyi1
 * @Date: 2022-12-20 21:14:52
 * @LastEditors: wuhanyi1 874864297@qq.com
 * @LastEditTime: 2023-01-01 16:13:52
 * @FilePath: /cpp_basic_library/src/common/include/common/rpc_client_manager.h
 * @Description: 
 * 
 * Copyright (c) 2023 by wuhanyi1 874864297@qq.com, All Rights Reserved. 
 */
#ifndef __WHY_RPC_CLIENT_MANAGER_H__
#define __WHY_RPC_CLIENT_MANAGER_H__

#include "singleton.h"
#include "rpc/client.h"
#include <unordered_map>

namespace why {

class RpcClientManager {
public:
    struct Config {
        std::string ip;
        uint16_t port;
        uint64_t timeout;
        bool keep_alive{false};
    };

    RpcClientManager() = default;
    ~RpcClientManager() { m_clients.clear(); };

    void Erase(const std::string &ip, const std::string &port);

    std::shared_ptr<rpc::client> Get(const Config &);

private:
    std::mutex m_mutex;
    struct RpcClientWrapper {
        int64_t thread_id;
        std::shared_ptr<rpc::client> rpc_client;
    };
    // [uri, RpcClientWrapper]
    std::unordered_multimap<std::string, RpcClientWrapper> m_clients;
    
};

using ClientMgr = Singleton<RpcClientManager>;

}

#endif