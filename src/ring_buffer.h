//
// Created by nikhil on 2/1/23.
//

#ifndef CAMERA_FUSION_RING_BUFFER_H
#define CAMERA_FUSION_RING_BUFFER_H


#include <cstddef>
#include <vector>

// #define RING_BUFFER_PRINT_ENABLED

template<typename T>
class RingBuffer final {
public:
    explicit RingBuffer(size_t capacity) {
        /* Have to use resize instead of reserve. Otherwise, doing subscript access is UB until push_back has been
         * called, and we do subscript access to implement our push_back. */
        buffer_.resize(capacity);
    }

    void push_back(T item) {
        debug_print();
        buffer_[head_] = item;
        const auto cap = capacity();
        head_ = (head_ + 1) % cap;
        if (used_ == cap) {
            tail_ = head_;
        }
        used_ = std::min(used_ + 1, cap);
//        debug_print();
    }

    size_t capacity() const {
        return buffer_.capacity();
    }

    size_t size() const {
        return used_;
    }

    T pop_front() {
        assert(used_ > 0);
        T temp = front();
        tail_++;
        used_--;
        if (tail_ >= capacity()) {
            tail_ = 0;
        }
        std::cout << "After pop_front\n";
//        debug_print();
        return temp;
    }

    T &front() {
        return this->operator[](0);
    }

    T &operator[](size_t i) {
        return buffer_[index(i)];
    }

    class Iterator {
        using iterator_category = std::forward_iterator_tag;
        using difference_type = std::ptrdiff_t;
        using value_type = T;
        using pointer = T *;
        using reference = T &;
    public:
        Iterator(RingBuffer<T> *rb, size_t index) : rb_(rb), index_(index) {}

        // TODO(nikhilm): I don't get why direct subscript calls aren't working.
        reference operator*() const { return rb_->operator[](index_); }

        pointer operator->() { return &rb_->operator[](index_); }

        Iterator operator-(difference_type d) {
            // Yes, totally unsafe without bounds checking.
            index_ -= d;
            return *this;
        }

    private:
        RingBuffer<T> *rb_;
        size_t index_;
    };

    Iterator end() {
        debug_print();
        return Iterator(this, size());
    }

private:
    size_t index(size_t i) const {
        // Can't assert for end().
        // assert(i < size());
        return (tail_ + i) % capacity();
    }

    // Only works with printable types.
#ifdef RING_BUFFER_PRINT_ENABLED
    void debug_print() const {
        std::cout << "RingBuffer (capacity=" << buffer_.capacity() << ", used=" << used_ << ")\n";
        for (int i = 0; i < buffer_.capacity(); i++) {
            std::cout << buffer_[i] << " ";
        }
        std::cout << "\n";
        std::string fill(2 * (buffer_.capacity() + 1), ' ');
        if (head_ == tail_) {
            fill[2 * head_] = 'B';
        } else {
            fill[2 * head_] = 'H';
            fill[2 * tail_] = 'T';
        }
        std::cout << fill << "\n";
    }
#else

    void debug_print() const {}

#endif

    std::vector<T> buffer_{};
    size_t tail_{0};
    size_t head_{0};
    // Needed to disambiguate full vs empty.
    size_t used_{0};
};


#endif //CAMERA_FUSION_RING_BUFFER_H
