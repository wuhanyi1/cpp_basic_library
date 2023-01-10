#include "config.h"

namespace why {

ConfigVarManager::ConfigVarMap ConfigVarManager::m_datas{};
std::mutex ConfigVarManager::m_mtx{};

void ConfigVarManager::ParseAllNodes(const std::string& prefix, 
                              const YAML::Node& node, 
                              std::vector<std::pair<std::string, YAML::Node>>& output) {
    output.push_back(std::make_pair(prefix, node));
    if (node.IsMap()) {
        for (auto iter = node.begin(); iter != node.end(); iter++) {
            ParseAllNodes(prefix.empty() ? iter->first.Scalar() : prefix + "." + iter->first.Scalar(), iter->second, output);
        }
    }
}

void ConfigVarManager::LoadFromYaml(const YAML::Node& node) {
    std::vector<std::pair<std::string, YAML::Node>> output{};
    ParseAllNodes("", node, output);

    for (auto &i : output) {
        auto &key = i.first;
        if (key.empty()) {
            continue;
        }
        std::transform(key.begin(), key.end(), key.begin(), ::tolower);
        if (key.find_first_not_of(kKeyRegularLetter) != std::string::npos) {
            CHECK_THROW(false, "prefix:%s has invalid letter which not belong to:%s", 
                        key.c_str(), kKeyRegularLetter);                    
        }
        ConfigVarBase::ptr ptr = LookUpBase(key);
        if (ptr == nullptr) continue;
        if (i.second.IsScalar()) {
            ptr->FromString(i.second.Scalar());
        } else {
            std::stringstream ss;
            ss << i.second;
            ptr->FromString(ss.str());
        }
    }
}

}