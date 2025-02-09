/*T
 * @File        singleton.h
 * @brief       单例模版
 *
 * @Author      Jingbx
 * @Data        2024/12/10
 * @History
 */
#ifndef SINGLETON_H
#define SINGLETON_H
#include "global.h"

template <typename T>
class Singleton {
protected:
    // protected 让子类可以调用父类构造实现单例模式
    Singleton() = default;
    Singleton(const Singleton<T> &) = delete;
    Singleton &operator = (const Singleton<T> &st) = delete;

    static std::shared_ptr<T> _instance;        // ⚠️ 静态成员变量 且 初始化线程安全

public:
    static std::shared_ptr<T> GetInstance() {
        /**  ⚠️ once_flag call_once配合 让初始化只进行一次
        *    避免懒汉2次确定条件，避免make_shared过程分配内存后，
        *    构造和赋值不按顺序执行，导致其他线程取到了先赋值但还没构造的单例
        */
        static std::once_flag s_flag;
        std::call_once(s_flag, [&] () {
            _instance = std::shared_ptr<T>(new T); // make_shared不能使用，构造函数是protected的
        });
        return _instance;
    }

    ~Singleton() {
        std::cout << "Singleton obj has been deleted" << std::endl;
    }

    void printAddress() {
        std::cout << "singleton instance address: " << _instance.get() << std::endl;
    }
};

/**
 * static 成员在非模版类中在cpp中定义
 *           在模版类中在.h下方定义
 */
template <typename T>
std::shared_ptr<T> Singleton<T>::_instance = nullptr;

#endif // SINGLETON_H
