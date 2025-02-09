/**
 * @file    : /Users/jingbx/Qt.6.0.0/workspace/Qchat/server/src/singleton.h
 * @date    : 2024-12-11 15:46:13
 * @author  : Jingbx(18804328557@163.com)
 * @brief   :
 * -----
 * Last Modified: 2024-12-14 17:52:32
 * -----
 * @history :
 * ================================================================================
 *    Date   	 By 	(version) 	                    Comments
 * ----------	----	---------    ----------------------------------------------
 * ================================================================================
 * Copyright (c) 2018 - 2024 All Right Reserved, JBX
*/
#ifndef __SERVER_SINGLETON_H_
#define __SERVER_SINGLETON_H_

#include <memory>
#include <mutex>
#include <iostream>
template <typename T>
class SingleTon {
protected:
    SingleTon() = default;
    SingleTon(const SingleTon<T> &) = delete;
    SingleTon &operator=(const SingleTon<T> &) = delete;

    static std::unique_ptr<T> _instance;

public:
    static T* GetInstance() {  // 返回裸指针，或者可以封装返回引用
        static std::once_flag s_flag;
        std::call_once(s_flag, [&] () {
            _instance.reset(new T);  // 创建 unique_ptr
        });
        return _instance.get();
    }

    ~SingleTon() {
        std::cout << "Singleton obj has been deleted" << std::endl;
    }
};

/** 模版类静态成员变量定义需要在.h/.tpp中 */
template <typename T>
std::unique_ptr<T> SingleTon<T>::_instance = nullptr;


#endif // __SERVER_SINGLETON_H_
