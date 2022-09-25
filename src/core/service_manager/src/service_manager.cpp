#include "service_manager.h"
#include "log.h"

namespace why {

ServiceManager::ServiceManager(const Config &config) : m_config(config) {

}

void ServiceManager::Start() {
    std::lock_guard<std::mutex> lock(m_mutex);
    // 0 表示监听任意一个端口
    m_server = make_shared_with_deleter<rpc::server>([](rpc::server *server) {
        server->stop();
    }, 0);

    RegisterRpcService();
    // ..... so on
}

void ServiceManager::RegisterPublisher(const PublisherInfo &pub_info) {
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        auto iter = m_pubs.find(pub_info.topic_name);
        if (iter != m_pubs.end()) {
            LOG_WARN("publisher:%s register repeatitively", pub_info.topic_name.c_str());
            return;
        }
        m_pubs[pub_info.topic_name] = std::make_shared<PublisherInfo>(pub_info);
    }

    
}

void ServiceManager::RegisterSubscriber(const SubscriberInfo &sub_info) {

}

void ServiceManager::RegisterRpcService() {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    m_server->bind("RequestTopic", []() {

    });

    m_server->bind("NotifyChannelEstablished", []() {

    });
}

ServiceManager::ptr ServiceManager::Make(const Config& config) {
    return std::make_shared<ServiceManager>(config);
}


}