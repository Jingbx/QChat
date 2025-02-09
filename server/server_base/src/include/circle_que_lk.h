/**
 * @file    : /Users/jingbx/Qt.6.0.0/workspace/Qchat/server/src/src_base/circle_que_lk.h
 * @date    : 2024-12-14 12:30:25
 * @author  : Jingbx(18804328557@163.com)
 * @brief   : 模版有锁队列直接操作circle内存，不使用链表或list，仅队头队尾加锁
 *            私有继承 std::allocator<T> 是一种较为高级的内存管理设计方式，
 *            通常目的是通过继承分配器的功能，直接操作内存，同时隐蔽继承关系并对外部隐藏分配器接口。
 *            _max_size 设置为 cap + 1 来区分 队列满 和 队列空 的状态(达到环形)。根据代码的逻辑，
 *            当队列达到 _tail + 1 == _head 时被视为队列满。因此，为了存储固定数量 N 的有效元素，需要多一个空的额外空间。
 * -----
 * Last Modified: 2024-12-23 12:31:07
 * -----
 * @history :
 * ================================================================================
 *    Date   	 By 	(version) 	                    Comments
 * ----------	----	---------    ----------------------------------------------
 *   12.15      jingbx     1.0       cap必须传入编译时常量，所以yaml没法直接控制队列大小，无法动态开辟，需要修改代码
 * ================================================================================
 * Copyright (c) 2018 - 2024 All Right Reserved, JBX
 */
#ifndef __SERVER_CIRCLE_QUE_LK_H_
#define __SERVER_CIRCLE_QUE_LK_H_

#include <iostream>
#include <memory>
#include <mutex>

template <typename T, size_t cap>
class CircleQueLk : private std::allocator<T> {
public:
    CircleQueLk() 
        : _max_size(cap + 1),
          _data(std::allocator<T>::allocate(_max_size)),
          _head(0),
          _tail(0) {}
    
    ~CircleQueLk() {
        // note: destroy & deallocate
        while (_head != _tail) {
            // 原代码: std::allocator<T>::destroy(_data + _head);
            // 修改: 使用显式析构函数替代
            (_data + _head)->~T();  // 显式调用析构函数
            _head = (_head + 1) % _max_size;
        }
        std::allocator<T>::deallocate(_data, _max_size);
    }
    
    CircleQueLk(const CircleQueLk &) = delete;
    CircleQueLk &operator = (const CircleQueLk &que) = delete;
    CircleQueLk &operator = (const CircleQueLk &que) volatile = delete; // 编译器优化，可变的声明，从内存中取数据

    /**
     * @brief empty
     */
    bool empty() {
        std::lock_guard<std::mutex> lock(_mtx);
        return _head == _tail;
    }

    /**
     * @brief size 通过 _tail - _head得到
     */
    size_t size() {
        std::lock_guard<std::mutex> lock(_mtx);
        if (_tail >= _head) {
            return (_tail - _head);
        } else {
            return (_max_size - _head - _tail);
        }
    }
    
    /**
     * @brief base push
     */
    template <typename ...Args>
    bool emplace(Args &&...args) {
        std::lock_guard<std::mutex> lock(_mtx); // lock
        
        if ((_tail + 1) % _max_size == _head) {
            std::cout << "CircleQue is full!" << std::endl;
            return false;
        }
        
        // note:构造: constructor(T类型指针指向构造位置, T类型构造函数所需要的参数)
        // 原代码: std::allocator<T>::construct(_data + _tail, std::forward<Args>(args)...);
        // 修改: 使用 placement new 替代
        new (_data + _tail) T(std::forward<Args>(args)...);  // 调用 placement new
        _tail = (_tail + 1) % _max_size;
        return true;
    }

    /**
     * @brief important: T&&为万能引用；forward根据参数的实际类型（左值或右值）将其完美转发
     */
    bool push(const T &t) {
        return emplace(t);
    }
    bool push(T &&t) {
        return emplace(std::forward<T>(t));
    }

    /**
     * @brief pop 有返回值
     */
    T pop_return() {
        std::lock_guard<std::mutex> lock(_mtx); // lock
        assert(_head != _tail && "Queue is empty!");
        // 销毁对象
        auto t = std::move(_data[_head]);
        _head = (_head + 1) % _max_size;
        return t;   
    }
    
    /**
     * @brief pop 无返回值
     */
    bool pop() {
        std::lock_guard<std::mutex> lock(_mtx); // lock
        assert(_head != _tail && "Queue is empty!");
        // 销毁对象
        // 原代码: std::allocator<T>::destroy(_data + _head);
        // 修改: 使用显式析构函数替代
        (_data + _head)->~T();  // 显式调用析构函数
        _head = (_head + 1) % _max_size;
        return true;   
    }

private:
    std::mutex  _mtx;
    size_t      _max_size;
    T           *_data;
    size_t      _head;
    size_t      _tail;
};

#endif  // __SERVER_CIRCLE_QUE_LK_H_