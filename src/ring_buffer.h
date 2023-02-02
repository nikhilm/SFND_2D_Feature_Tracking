//
// Created by nikhil on 2/1/23.
//

#ifndef CAMERA_FUSION_RING_BUFFER_H
#define CAMERA_FUSION_RING_BUFFER_H


#include <cstddef>
#include <vector>

template<typename T>
class RingBuffer final {
public:
    explicit RingBuffer(size_t capacity) { buffer_.reserve(capacity); }

    void push_back(T item) {
        buffer_[tail_] = item;
        tail_++;
    }

    size_t capacity() const {
        return buffer_.capacity();
    }

    size_t size() const {
        assert(head_ <= tail_);
        return tail_ - head_;
    }

    T &operator[](size_t i) {
        return buffer_[index(i)];
    }

private:
    size_t index(size_t i) const {
        assert(head_ <= tail_);
        assert(i < size());
        return head_ + i;
    }

    std::vector<T> buffer_{};
    size_t head_{0};
    size_t tail_{0};
};


#endif //CAMERA_FUSION_RING_BUFFER_H
