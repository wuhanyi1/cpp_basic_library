/*
 * @Author: wuhanyi
 * @Date: 2022-04-29 10:45:10
 * @LastEditTime: 2023-02-15 14:13:43
 * @FilePath: /cpp_basic_library/src/common/include/common/utils.h
 * @Description: 
 * 
 * Copyright (c) 2022 by wuhanyi, All Rights Reserved. 
 */

#ifndef __WHY_UTILS_H__
#define __WHY_UTILS_H__

#include <cxxabi.h>
 #include <execinfo.h>
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

/**
 * @description: 获取当前的调用栈
 * @param[out] bt 保存调用栈
 * @param[in] size 最多返回层数
 * @param[in] skip 跳过栈顶的层数
 */
void Backtrace(std::vector<std::string>& bt, int size = 64, int skip = 1);

/**
 * @description: 获取当前栈信息的字符串
 * @param[in] size 栈的最大层数
 * @param[in] skip 跳过栈顶的层数
 * @param[in] prefix 栈信息前输出的内容
 */
std::string BacktraceToString(int size = 64, int skip = 2, const std::string& prefix = "");

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

}

#endif