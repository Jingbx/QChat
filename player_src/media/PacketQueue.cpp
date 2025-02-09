// #include "PacketQueue.h"

// template<typename T>
// size_t PacketQueue<T>::size() {

//     std::lock_guard<std::mutex> lock(mtx);
//     return std::list<T>::size();

// }

// template <typename T>
// bool PacketQueue<T>::isEmpty() {

//     std::lock_guard<std::mutex> lock(mtx);
//     return std::list<T>::empty();

// }

// template <typename T>
// void PacketQueue<T>::enque (const T &t) {

//     std::lock_guard<std::mutex> lock(mtx);
//     std::list<T>::push_back(t);

// }

// template <typename T>
// T PacketQueue<T>::deque() {

//     std::lock_guard<std::mutex> lock(mtx);
//     T t = nullptr;
//     if (!this->isEmpty()) {
//         T t = std::list<T>::front();
//         std::list<T>::pop_front();
//     }
//     return t;

// }
