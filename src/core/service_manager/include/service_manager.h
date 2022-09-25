/*
 * @Author: wuhanyi
 * @Date: 2022-05-05 19:12:35
 * @LastEditTime: 2022-05-20 15:39:41
 * @FilePath: /basic_library/src/core/service_manager/include/service_manager.h
 * @Description: 
 * 
 * Copyright (c) 2022 by wuhanyi, All Rights Reserved. 
 */
#ifndef __WHY_SERVICE_MANAGER_H__
#define __WHY_SERVICE_MANAGER_H__

#include <memory>
#include <iostream>
#include <vector>
#include <unordered_map>
#include "common.h"
#include "rpc/server.h"
#include "rpc/client.h"

namespace why {

class ServiceManager {
public:
    using ptr = std::shared_ptr<ServiceManager>;

    struct Config {
        std::string master_uri;
        bool is_master{false};
    };
    struct PublisherInfo {
        std::string uri;
        std::string topic_name;    
    };
    struct SubscriberInfo {
        std::string uri;
        std::string topic_name;
    };
    
    struct PublisherUpdateInfo {
        // sub 的 topic_name
        std::string topic_name;
        std::vector<std::string> uris;
    };
    
    struct SubscriberUpdateInfo {
        // pub 的 topic_name
        std::string topic_name;
        std::vector<std::string> uris;
    };
    
    struct RequestTopicInfo {
        // 告诉 pub 通过哪个 topic
        std::string topic_name;
        EntityAddress address;
    };
    
    ServiceManager(const Config& config) : m_config(config) {}
    
    void Start();

    void Stop();

    void RegisterPublisher(const PublisherInfo&);
    void RegisterSubscriber(const SubscriberInfo&);
    
    void RequestTopic(const RequestTopicInfo&);

    void NotifyChannelEstablished();

    static ptr Make(const Config&);

private:
    /**
     * @description: 用于注册当前节点需要处理的 RPC 服务
     */
    void RegisterRpcService();  

private:
    std::mutex m_mutex;
    Config m_config;
    std::shared_ptr<rpc::server> m_server;
    std::unordered_map<std::string, std::shared_ptr<PublisherInfo>> m_pubs;
    std::unordered_map<std::string, std::shared_ptr<SubscriberInfo>> m_subs;
};

}

#endif