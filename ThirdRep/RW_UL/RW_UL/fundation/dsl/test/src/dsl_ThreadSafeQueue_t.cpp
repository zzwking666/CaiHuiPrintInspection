#include"dsl_ThreadSafeQueue.hpp"

#include"gtest/gtest.h"

namespace dsl_ThreadSafeQueue
{
    using rw::dsl::ThreadSafeQueue;

    TEST(ThreadSafeQueueTest, BackReturnsLastElement)
    {
        ThreadSafeQueue<int> q;
        q.push(1);
        q.push(2);
        q.push(3);
        EXPECT_EQ(q.back(), 3);

        // 修改尾部元素，验证引用
        q.back() = 42;
        EXPECT_EQ(q.back(), 42);
    }

    TEST(ThreadSafeQueueTest, ConstBackReturnsLastElement)
    {
        ThreadSafeQueue<int> q;
        q.push(10);
        q.push(20);

        const ThreadSafeQueue<int>& cq = q;
        EXPECT_EQ(cq.back(), 20);
    }

    TEST(ThreadSafeQueueTest, BackThrowsOnEmptyQueue)
    {
        ThreadSafeQueue<int> q;
        EXPECT_THROW(q.back(), std::out_of_range);
        const ThreadSafeQueue<int>& cq = q;
        EXPECT_THROW(cq.back(), std::out_of_range);
    }
}