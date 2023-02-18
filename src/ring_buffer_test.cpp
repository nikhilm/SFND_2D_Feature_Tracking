#include "gtest/gtest.h"
#include "gtest/gtest-matchers.h"
#include "gmock/gmock.h"
#include "ring_buffer.h"

TEST(RingBufferTest, TestBelowCapacity) {
    RingBuffer<uint32_t> rb(5);
    EXPECT_EQ(rb.size(), 0);
    EXPECT_EQ(rb.capacity(), 5);
    for (int i = 0; i < 4; ++i) {
        rb.push_back(i);
        EXPECT_EQ(rb.size(), i + 1);
    }

    for (int i = 0; i < 4; ++i) {
        EXPECT_EQ(rb[i], i);
    }
}

TEST(RingBufferTest, SupportsEndIterator) {
    constexpr size_t cap = 5;
    RingBuffer<uint32_t> rb(cap);
    for (int i = 0; i < 8; ++i) {
        rb.push_back(i);
        EXPECT_EQ(rb.size(), std::min(cap, static_cast<size_t>(i + 1)));
        EXPECT_EQ(*(rb.end() - 1), i);
        if (rb.size() >= 2) {
            EXPECT_EQ(*(rb.end() - 2), i - 1);
        }
    }

}

TEST(RingBufferTest, CapacityIsNotExceeded) {
    constexpr size_t cap = 5;
    RingBuffer<uint32_t> rb(cap);
    for (int i = 0; i < 8; ++i) {
        rb.push_back(i);
        EXPECT_EQ(rb.size(), std::min(cap, static_cast<size_t>(i + 1)));
    }

    EXPECT_EQ(rb.size(), rb.capacity());
    uint32_t expected = 3;
    for (int i = 0; i < rb.capacity(); i++) {
        uint32_t item = rb.pop_front();
        EXPECT_EQ(item, expected);
        expected++;
    }
    rb.push_back(9);
    EXPECT_EQ(rb.size(), 1);
}