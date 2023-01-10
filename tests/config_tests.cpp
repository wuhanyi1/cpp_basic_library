#include "config.h"
#include "log.h"
#include <yaml-cpp/yaml.h>

struct Person {
    int m_age{0};
    int m_height{0};
    std::string m_name{};
    bool operator==(const Person& p) const {
        return m_age == p.m_age && m_height == p.m_height && m_name == p.m_name;
    }
};

why::ConfigVar<int>::ptr int_config_val = 
        why::ConfigVarManager::LookUp("system.port", (int)8080, "system port");
// why::ConfigVar<float>::ptr int_config_val1 = 
//         why::ConfigVarManager::LookUp("system.port", (float)8080, "system port");        
why::ConfigVar<float>::ptr float_config_val = 
        why::ConfigVarManager::LookUp("system.value", (float)1.1f, "system value");
why::ConfigVar<std::vector<int>>::ptr int_vec_config_val = 
        why::ConfigVarManager::LookUp("system.array", (std::vector<int>){1, 2}, "system array");
why::ConfigVar<std::list<int>>::ptr int_list_config_val = 
        why::ConfigVarManager::LookUp("system.list", (std::list<int>){1, 2}, "system list");
why::ConfigVar<std::map<std::string, int>>::ptr int_map_config_val = 
        why::ConfigVarManager::LookUp("system.map", (std::map<std::string, int>){{"value1", 22}, {"value2", 33}}, "system map");

why::ConfigVar<std::vector<Person>>::ptr person_class_config_val = 
        why::ConfigVarManager::LookUp("system.class", (std::vector<Person>){(Person){18, 185, "Tom"}, (Person){19, 183, "Nick"}}, "system class");



template<>
struct why::LexicalCast<std::string, Person> {
    Person operator()(const std::string& val) {
        YAML::Node node = YAML::Load(val);
        Person res{};
        res.m_age = node["age"].as<int>();
        res.m_height = node["height"].as<int>();
        res.m_name = node["name"].as<std::string>();
        return res;
    }
};

template<>
struct why::LexicalCast<Person, std::string> {
    std::string operator()(const Person& val) {
        YAML::Node node(YAML::NodeType::Map);
        node["name"] = val.m_name;
        node["age"] = val.m_age;
        node["height"] = val.m_height;
        return (std::stringstream() << node).str();
    }
};

// template<>
// struct why::LexicalCast<std::string, std::vector<Person>> {
//     std::vector<Person> operator()(const std::string& val) {
//         YAML::Node node = YAML::Load(val);
//         std::vector<Person> res{};
//         std::stringstream ss{};
//         for (size_t i = 0; i < node.size(); ++i) {
//             ss.str("");
//             ss.clear();
//             ss << node[i];
//             res.push_back(why::LexicalCast<std::string, Person>()(ss.str()));
//         }
//         return res;
//     }
// };

// template<>
// struct why::LexicalCast<std::vector<Person>, std::string> {
//     std::string operator()(const std::vector<Person>& val) {
//         YAML::Node node(YAML::NodeType::Sequence);
//         YAML::Node sub_node(YAML::NodeType::Map);
//         for (size_t i = 0; i < val.size(); ++i) {
//             sub_node["name"] = val[i].m_name;
//             sub_node["age"] = val[i].m_age;
//             sub_node["height"] = val[i].m_height;
//             node.push_back(sub_node);
//         }
//         return (std::stringstream() << node).str();
//     }
// };


// YAML::Node 重载了 << 流运算符
void print_yaml(const YAML::Node& node, int level) {
    if(node.IsScalar()) {
        WHY_LOG_INFO_WITH_STREAM(LOG_ROOT()) << std::string(level * 4, ' ')
            << node.Scalar() << " - " << node.Type() << " - " << level << " is Scalar";
    } else if(node.IsNull()) {
        WHY_LOG_INFO_WITH_STREAM(LOG_ROOT()) << std::string(level * 4, ' ')
            << "NULL - " << node.Type() << " - " << level;
    } else if(node.IsMap()) {
        for(auto it = node.begin();
                it != node.end(); ++it) {
            WHY_LOG_INFO_WITH_STREAM(LOG_ROOT()) << std::string(level * 4, ' ')
                    << it->first << " - " << it->second.Type() << " - " << level;
            print_yaml(it->second, level + 1);
        }
    } else if(node.IsSequence()) {
        for(size_t i = 0; i < node.size(); ++i) {
            WHY_LOG_INFO_WITH_STREAM(LOG_ROOT()) << std::string(level * 4, ' ')
                << i << " - " << node[i].Type() << " - " << level;
            print_yaml(node[i], level + 1);
        }
    }
}

template<typename T>
void print_list(const std::list<T>& list, const std::string& str) {
    for (auto iter = list.begin(); iter != list.end(); ++iter) {
        WHY_LOG_INFO_WITH_STREAM(LOG_ROOT()) << str << *iter;
    }
}

template<typename T>
void print_map(const std::map<std::string, T>& mp, const std::string& str) {
    for (auto &i : mp) {
        WHY_LOG_INFO_WITH_STREAM(LOG_ROOT()) << "key:" << i.first << " " << str << i.second;
    }
}

void print_person(const std::vector<Person>& nums, const std::string& str) {
    for (auto &i : nums) {
        WHY_LOG_INFO_WITH_STREAM(LOG_ROOT())  << str 
                                              << " name: " << i.m_name 
                                              << " age:" << i.m_age
                                              << " height:" << i.m_height;
    }
}

void test_config() {
    WHY_LOG_INFO_WITH_STREAM(LOG_ROOT()) << "int_config_val before value is:" << int_config_val->GetValue();
    WHY_LOG_INFO_WITH_STREAM(LOG_ROOT()) << "float_config_val before value is:" << float_config_val->GetValue();

    for (auto &i : int_vec_config_val->GetValue()) {
        WHY_LOG_INFO_WITH_STREAM(LOG_ROOT()) << "int_vec_config_val before value is:" << i;
    }

    print_list(int_list_config_val->GetValue(), "int_list_config_val begin value is:");
    print_map(int_map_config_val->GetValue(), "int_map_config_val begin value is:");

    print_person(person_class_config_val->GetValue(), "person_class_config_val begin value is:");
    auto key = person_class_config_val->AddListener([](const std::vector<Person>& p1, const std::vector<Person>& p2) {
        print_person(p1, "person_class_config_val old_value is:");
        print_person(p2, "person_class_config_val new_value is:");
    });
    (void)key;

    YAML::Node root = YAML::LoadFile("../conf/test.yaml");
    why::ConfigVarManager::LoadFromYaml(root);
    WHY_LOG_INFO_WITH_STREAM(LOG_ROOT()) << "int_config_val after value is:" << int_config_val->GetValue();
    WHY_LOG_INFO_WITH_STREAM(LOG_ROOT()) << "float_config_val after value is:" << float_config_val->GetValue();
    for (auto &i : int_vec_config_val->GetValue()) {
        WHY_LOG_INFO_WITH_STREAM(LOG_ROOT()) << "int_vec_config_val after value is:" << i;
    }

    print_list(int_list_config_val->GetValue(), "int_list_config_val after value is:");
    print_map(int_map_config_val->GetValue(), "int_map_config_val after value is:");
    print_person(person_class_config_val->GetValue(), "person_class_config_val after value is:");
    // WHY_LOG_INFO_WITH_STREAM(LOG_ROOT()) << "--------------- " << root["aaa"].IsDefined();
}


void test_log_config() {
    YAML::Node root = YAML::LoadFile("../conf/log.yaml");
    why::ConfigVarManager::LoadFromYaml(root);
    // LOG_INFO(" %s", why::LoggerManager::LoggerMgr::Instance().ToYamlString().c_str());
}

void test_logger() {
    why::Logger::ptr sys_logger = LOG_NAME("system");
    // WHY_LOG_LEVEL_WITH_STREAM(sys_logger, why::LogLevel::INFO) << "system log test";
    WHY_LOG_LEVEL(sys_logger, why::LogLevel::ERROR, "system log test");
}

int main() {
    // LOG_INFO("ConfigVar:%s, value is:%d, string fmt is:%s", int_config_val->GetName().c_str(), int_config_val->GetValue(), int_config_val->ToString().c_str());
    // LOG_INFO("ConfigVar:%s, value is:%f, string fmt is:%s", float_config_val->GetName().c_str(), float_config_val->GetValue(), float_config_val->ToString().c_str());

    // YAML::Node root = YAML::LoadFile("../conf/log.yaml");
    // WHY_LOG_INFO_WITH_STREAM(LOG_ROOT()) << root.IsMap();
    // WHY_LOG_INFO_WITH_STREAM(LOG_ROOT()) << root["logs"];

    test_log_config();
    test_logger();

    // test_config();
    
    // WHY_LOG_INFO_WITH_STREAM(LOG_ROOT()) << root["logs"][0]["appenders"][0]["type"];
    // WHY_LOG_INFO_WITH_STREAM(LOG_ROOT()) << root.IsScalar() << " " << root.IsDefined() << " " << root.IsSequence() << " " << root.IsMap();
    // WHY_LOG_INFO_WITH_STREAM(LOG_ROOT()) << root.Type();
    // WHY_LOG_INFO_WITH_STREAM(LOG_ROOT()) << root;

    // print_yaml(root, 0);
}