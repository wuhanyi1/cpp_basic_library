/*
 * @Author: wuhanyi1 874864297@qq.com
 * @Date: 2023-01-01 18:03:08
 * @LastEditors: wuhanyi1 874864297@qq.com
 * @LastEditTime: 2023-01-08 15:47:34
 * @FilePath: /cpp_basic_library/src/config/config.h
 * @Description: 
 * 
 * Copyright (c) 2023 by wuhanyi1 874864297@qq.com, All Rights Reserved. 
 */
#ifndef __WHY_CONFIG_H__
#define __WHY_CONFIG_H__

#include <string>
#include <memory>
#include <boost/lexical_cast.hpp>
#include <yaml-cpp/yaml.h>
#include <mutex>
#include <algorithm>
#include <vector>
#include <list>
#include <map>
#include <unordered_map>
#include <set>
#include <unordered_set>
#include <utility>
#include <functional>
#include "common.h"
namespace why {



/**
 * @description: 从 From 类型转换至 To 类型,仅支持内置类型与 string 之间的转换
 */
template<typename From, typename To>
struct LexicalCast {
    To operator()(const From& val) {
        return boost::lexical_cast<To>(val);
    }
};    

template<typename T>
struct LexicalCast<std::string, std::vector<T>> {
    std::vector<T> operator()(const std::string& val) {
        YAML::Node node = YAML::Load(val);// 可能会抛异常
        std::vector<T> res{};
        std::stringstream ss{};
        for (size_t i = 0; i < node.size(); ++i) {
            // ss.clear();
            ss.str("");
            ss << node[i];
            res.push_back(LexicalCast<std::string, T>()(ss.str()));
        }
        return res;
    }
};

template<typename T>
struct LexicalCast<std::vector<T>, std::string> {
    std::string operator()(const std::vector<T>& vec) {
        YAML::Node node(YAML::NodeType::Sequence);
        std::stringstream ss{};
        for (auto &i : vec) {
            node.push_back(YAML::Load(LexicalCast<T, std::string>()(i)));
        }
        ss << node;
        return ss.str();
    }
};

template<typename T>
struct LexicalCast<std::string, std::list<T>> {
    std::list<T> operator()(const std::string& val) {
        YAML::Node node = YAML::Load(val);// 可能会抛异常
        std::list<T> res{};
        std::stringstream ss{};
        for (size_t i = 0; i < node.size(); ++i) {
            // ss.clear();
            ss.str("");
            ss << node[i];
            res.push_back(LexicalCast<std::string, T>()(ss.str()));
        }
        return res;
    }
};

template<typename T>
struct LexicalCast<std::list<T>, std::string> {
    std::string operator()(const std::list<T>& list) {
        YAML::Node node(YAML::NodeType::Sequence);
        std::stringstream ss{};
        for (auto iter = list.begin(); iter != list.end(); ++iter) {
            node.push_back(YAML::Load(LexicalCast<T, std::string>()(*iter)));
        }
        ss << node;
        return ss.str();
    }
};

template<class T>
class LexicalCast<std::string, std::set<T>> {
public:
    std::set<T> operator()(const std::string& val) {
        YAML::Node node = YAML::Load(val);
        std::set<T> res;
        std::stringstream ss;
        for(size_t i = 0; i < node.size(); ++i) {
            ss.str("");
            ss << node[i];
            res.insert(LexicalCast<std::string, T>()(ss.str()));
        }
        return res;
    }
};

template<class T>
class LexicalCast<std::set<T>, std::string> {
public:
    std::string operator()(const std::set<T>& val) {
        YAML::Node node(YAML::NodeType::Sequence);
        for(auto& i : val) {
            node.push_back(YAML::Load(LexicalCast<T, std::string>()(i)));
        }
        std::stringstream ss;
        ss << node;
        return ss.str();
    }
};

template<class T>
class LexicalCast<std::string, std::unordered_set<T>> {
public:
    std::unordered_set<T> operator()(const std::string& val) {
        YAML::Node node = YAML::Load(val);
        std::unordered_set<T> res;
        std::stringstream ss;
        for(size_t i = 0; i < node.size(); ++i) {
            ss.str("");
            ss << node[i];
            res.insert(LexicalCast<std::string, T>()(ss.str()));
        }
        return res;
    }
};

template<class T>
class LexicalCast<std::unordered_set<T>, std::string> {
public:
    std::string operator()(const std::unordered_set<T>& val) {
        YAML::Node node(YAML::NodeType::Sequence);
        for(auto& i : val) {
            node.push_back(YAML::Load(LexicalCast<T, std::string>()(i)));
        }
        std::stringstream ss;
        ss << node;
        return ss.str();
    }
};

template<class T>
class LexicalCast<std::string, std::map<std::string, T> > {
public:
    std::map<std::string, T> operator()(const std::string& v) {
        YAML::Node node = YAML::Load(v);
        std::map<std::string, T> res;
        std::stringstream ss;
        for(auto it = node.begin();
                it != node.end(); ++it) {
            ss.str("");
            ss << it->second;
            res.emplace(it->first.Scalar(),
                        LexicalCast<std::string, T>()(ss.str()));
        }
        return res;
    }
};

template<class T>
class LexicalCast<std::map<std::string, T>, std::string> {
public:
    std::string operator()(const std::map<std::string, T>& val) {
        YAML::Node node(YAML::NodeType::Map);
        for(auto& i : val) {
            node[i.first] = YAML::Load(LexicalCast<T, std::string>()(i.second));
        }
        std::stringstream ss;
        ss << node;
        return ss.str();
    }
};

template<class T>
class LexicalCast<std::string, std::unordered_map<std::string, T> > {
public:
    std::unordered_map<std::string, T> operator()(const std::string& v) {
        YAML::Node node = YAML::Load(v);
        std::unordered_map<std::string, T> res;
        std::stringstream ss;
        for(auto it = node.begin();
                it != node.end(); ++it) {
            ss.str("");
            ss << it->second;
            res.emplace(it->first.Scalar(),
                        LexicalCast<std::string, T>()(ss.str()));
        }
        return res;
    }
};

template<class T>
class LexicalCast<std::unordered_map<std::string, T>, std::string> {
public:
    std::string operator()(const std::unordered_map<std::string, T>& val) {
        YAML::Node node(YAML::NodeType::Map);
        for(auto& i : val) {
            node[i.first] = YAML::Load(LexicalCast<T, std::string>()(i.second));
        }
        std::stringstream ss;
        ss << node;
        return ss.str();
    }
};

/**
 * @description: 配置变量的基类
 */
class ConfigVarBase {
public:
    using ptr = std::shared_ptr<ConfigVarBase>;
    ConfigVarBase(const std::string& name, const std::string& desc = "") 
        : m_name(name), m_description(desc) {
            // 大写统一转换为小写
            std::transform(m_name.begin(), m_name.end(), m_name.begin(), ::tolower);
        }

    virtual ~ConfigVarBase() = default;

    const std::string& GetName() const { return m_name; }
    const std::string& GetDescription() const { return m_description; }

    /**
     * @description: 下面分别是将配置变量转换为字符串以及从字符串初始化为一个指定类型的配置变量
     * @description: 即序列化与反序列化
     */
    virtual std::string ToString() = 0;
    virtual void FromString(const std::string& val) = 0;

    virtual const char* GetTypeName() const = 0;

protected:
    std::string m_name;
    std::string m_description;

};

/**
 * @description: 管理[配置变量]的实体
 */
template<typename T, 
         typename FromStr = LexicalCast<std::string, T>, 
         typename ToStr = LexicalCast<T, std::string>>
class ConfigVar : public ConfigVarBase {
public:
    using ptr = std::shared_ptr<ConfigVar>;
    using OnChangeCb = std::function<void(const T& old_val, const T& new_val)>;
    ConfigVar(const std::string& name,
              const T& default_value,
              const std::string& description)
        : ConfigVarBase(name, description), m_val(default_value) {}

    ~ConfigVar() = default;

    std::string ToString() override {
        try {
            LOCK_GUARD lock(m_mtx);
            return ToStr()(m_val);
        } catch (std::exception& e) {
            CHECK_THROW(false, "ConfigVar::ToString exeption! Convert Type:%s to string, what:%s", 
                                TypeToName<T>(), e.what());
        }
        return "";
    }

    void FromString(const std::string& val) {
        try {
            // LOCK_GUARD lock(m_mtx);
            SetValue(FromStr()(val));
        } catch (const std::exception& e) {
            CHECK_THROW(false, "ConfigVal::FromString exception! Convert string:[%s] To Type:[%s], what:%s",
                                val.c_str(), TypeToName<T>(), e.what());
        }
    }

    const char* GetTypeName() const override { return TypeToName<T>();}

    const T& GetValue() { 
        LOCK_GUARD lock(m_mtx);
        return m_val;
    }

    void SetValue(const T& val) {
        LOCK_GUARD lock(m_mtx);
        if (val == m_val) {
            return;
        }

        for (auto &cb : m_cbs) {
            cb.second(m_val, val);
        }
        m_val = val;
    }

    uint64_t AddListener(const OnChangeCb& cb) {
        static uint64_t monotone_key = 0;
        if (cb == nullptr) {
            return 0;
        }
        LOCK_GUARD lock(m_mtx);
        
        m_cbs.emplace(++monotone_key, cb);
        return monotone_key;
    }

    void DelListener(uint64_t key) {
        LOCK_GUARD lock(m_mtx);
        m_cbs.erase(key);
    }

    void ClearListener() {
        LOCK_GUARD lock(m_mtx);
        m_cbs.clear();
    }

private:
    T m_val;// 特定类型的配置变量
    std::map<uint64_t, OnChangeCb> m_cbs;
    std::mutex m_mtx;
};

/**
 * @description: 配置模块,ConfigVar 的管理类,提供接口访问、管理 ConfigVar
 */
class ConfigVarManager : public Noncopyable {
public:
    using ConfigVarMap = std::unordered_map<std::string, ConfigVarBase::ptr>;
    
    ConfigVarManager() = delete;
    ~ConfigVarManager() = delete;
    
    /**
     * @description: 查找名称为 name 的配置变量，找不到则创建一个配置变量并插入
     * @return: 注意返回值是子类的智能指针
     */
    template<typename T>
    static typename ConfigVar<T>::ptr LookUp(const std::string& name, 
                                             const T& default_value,
                                             const std::string& desc = "") {
        UNIQUE_LOCK lock(m_mtx);
        auto iter = m_datas.find(name);
        if (iter != m_datas.end()) {
            lock.unlock();
            auto tmp = std::dynamic_pointer_cast<ConfigVar<T>>(iter->second);
            if (tmp == nullptr) {
                CHECK_THROW(false, "ConfigVar of name:%s exists, but realtype is:%s, cur type is:%s", 
                            name.c_str(), iter->second->GetTypeName(), TypeToName<T>());
                return nullptr;
            } else {
                return tmp;
            }
        }                                                
        
        if (name.find_first_not_of(kKeyRegularLetter) != std::string::npos) {
            lock.unlock();
            CHECK_THROW(false, "record ConfigVar:%s failed, cause it's name has invalid letter which not belong %s",
                      name.c_str(), kKeyRegularLetter);
            return nullptr;
        }

        const auto &ret = m_datas.emplace(name, std::make_shared<ConfigVar<T>>(name, default_value, desc));
        return std::dynamic_pointer_cast<ConfigVar<T>>(ret.first->second);
    }

    // /**
    //  * @description: 查找名称(name)对应的配置变量,找不到返回 nullptr,注意调用此接口时必须显示指定模板参数
    //  */
    // template<typename T>
    // static typename ConfigVar<T>::ptr LookUp(const std::string& name) {
    //     LOCK_GUARD lock(m_mtx);
    //     auto iter = m_datas.find(name);
    //     if (iter == m_datas.end()) {
    //         return nullptr;
    //     }
    //     return std::dynamic_pointer_cast<ConfigVar<T>>(iter->second);
    // }

    static ConfigVarBase::ptr LookUpBase(const std::string& name) {
        LOCK_GUARD lock(m_mtx);
        auto iter = m_datas.find(name);
        return iter == m_datas.end() ? nullptr : iter->second;
    }

    /**
     * @description: 使用 YAML::Node 初始化配置模块
     */
    static void LoadFromYaml(const YAML::Node& node);

private:
    /**
     * @description: 解析出 YAML::Node 所有可能的中间变量
     * @param {string&} prefix 配置变量名前缀
     * @param {Node&} node 
     * @param {vector<Node>& } output 传出参数
     * @return {*}
     */
    static void ParseAllNodes(const std::string& prefix, 
                              const YAML::Node& node, 
                              std::vector<std::pair<std::string, YAML::Node>>& output); 

private:
    static constexpr auto kKeyRegularLetter = "abcdefghijklmnopqrstuvwxyz0123456789._";
    static ConfigVarMap m_datas;
    static std::mutex m_mtx;
};

}

#endif