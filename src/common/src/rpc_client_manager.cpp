/*
 * @Author: wuhanyi
 * @Date: 2022-05-13 21:06:06
 * @LastEditTime: 2022-06-10 21:06:47
 * @FilePath: /basic_library/src/common/src/rpc_client_manager.cpp
 * @Description: 
 * 
 * Copyright (c) 2022 by wuhanyi, All Rights Reserved. 
 */
// #include "rpc_client_manager.h"
// #include <iostream>
// #include <string>

// namespace why {
// std::shared_ptr<rpc::client> RpcClientManager::Get(const Config &config) {
//     bool found = false;
//     std::shared_ptr<rpc::client> rpc_client{};
//     auto thread_id = Utils::current_thread_id();
//     std::string rpc_uri = config.ip + "::" + std::to_string(config.port).c_str();
//     {
//         std::lock_guard<std::mutex> lock{m_mutex};
//         auto rpc_client_iter = m_clients.equal_range(rpc_uri);
//         if (rpc_client_iter.first != m_clients.end()) {
//             for (auto iter = rpc_client_iter.first; iter != rpc_client_iter.second;iter++) {
//                 if (iter->second.thread_id == thread_id) {
//                     rpc_client = iter->second.rpc_client;
//                     found = true;
//                     break;
//                 }
//             }
//         }
//         if (found == false) {
//             rpc_client = std::make_shared<rpc::client>(config.ip.c_str(), config.port);
//             if (config.keep_alive) {
//                 m_clients.emplace(rpc_uri, RpcClientWrapper{thread_id, rpc_client});
//             }
//         }
//     }
//     if (config.timeout > 0U) {
//         rpc_client->set_timeout(config.timeout);
//     }
//     return rpc_client;
// }

// void RpcClientManager::Erase(const std::string &ip, uint16_t port, bool ignore_thread_id) {
//     std::string rpc_uri = ip + "::" + std::to_string(port).c_str();

// }

// }