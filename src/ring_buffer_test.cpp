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

/*
TEST(RingBufferTest, CapacityIsNotExceeded) {
    constexpr size_t cap = 5;
    RingBuffer<uint32_t> rb(cap);
    for (int i = 0; i < 8; ++i) {
        rb.push_back(i);
        EXPECT_EQ(rb.size(), std::min(cap, static_cast<size_t>(i + 1)));
    }

    EXPECT_THAT(rb, ::testing::ElementsAreArray({3, 4, 5, 6, 7});
}
*/