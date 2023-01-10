/*
 * @Author: wuhanyi
 * @Date: 2022-04-29 10:45:10
 * @LastEditTime: 2023-01-02 16:37:46
 * @FilePath: /cpp_basic_library/src/common/include/common/utils.h
 * @Description: 
 * 
 * Copyright (c) 2022 by wuhanyi, All Rights Reserved. 
 */

#ifndef __WHY_UTILS_H__
#define __WHY_UTILS_H__

#include <cxxabi.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <stdio.h>
#include <stdint.h>
#include <vector>
#include <string>
#include <iomanip>
#include <yaml-cpp/yaml.h>
#include <iostream>
#include <memory>

namespace why {

uint64_t GetCurrentSec();

uint64_t GetCurrentMS();

uint64_t GetCurrentUS();

uint64_t GetCurrentNS();
    
std::string ToUpper(const std::string& name);

std::string ToLower(const std::string& name);

class FSUtil {
public:
    static void ListAllFile(std::vector<std::string>& files
                            ,const std::string& path
                            ,const std::string& subfix);
    static bool Mkdir(const std::string& dirname);
    static bool IsRunningPidfile(const std::string& pidfile);
    static bool Rm(const std::string& path);
    static bool Mv(const std::string& from, const std::string& to);
    static bool Realpath(const std::string& path, std::string& rpath);
    static bool Symlink(const std::string& frm, const std::string& to);
    static bool Unlink(const std::string& filename, bool exist = false);
    static std::string Dirname(const std::string& filename);
    static std::string Basename(const std::string& filename);
    static bool OpenForRead(std::ifstream& ifs, const std::string& filename
                    ,std::ios_base::openmode mode);
    static bool OpenForWrite(std::ofstream& ofs, const std::string& filename
                    ,std::ios_base::openmode mode);
};

template<typename type, typename Deleter, typename ...Args>
std::shared_ptr<type> make_shared_with_deleter(Deleter&& func, Args&& ...args) {
    return std::shared_ptr<type>(new type(std::forward<Args>(args)...), std::forward<Deleter>(func));
}

template<class T>
const char* TypeToName() {
    static const char* s_name = abi::__cxa_demangle(typeid(T).name(), nullptr, nullptr, nullptr);
    return s_name;
}

#define LOCK_GUARD std::lock_guard<std::mutex>
#define UNIQUE_LOCK std::unique_lock<std::mutex>

}

#endif