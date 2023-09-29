// Copyright 2021 Google LLC
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <gtest/gtest.h>

#include "ConcurrentQueue.h"

using namespace syncd;

class ConcurrentQueueTest : public ::testing::Test
{};

TEST_F(ConcurrentQueueTest, QueueIsEmpty)
{
    constexpr size_t queueSize = 5;
    ConcurrentQueue<int> testQueue(queueSize);

    EXPECT_TRUE(testQueue.empty());
    EXPECT_EQ(testQueue.size(), 0);
}

TEST_F(ConcurrentQueueTest, EnqueueSucceeds)
{
    constexpr size_t queueSize = 5;
    ConcurrentQueue<int> testQueue(queueSize);

    EXPECT_TRUE(testQueue.empty());

    EXPECT_TRUE(testQueue.enqueue(1));
    EXPECT_FALSE(testQueue.empty());
    EXPECT_EQ(testQueue.size(), 1);
}

TEST_F(ConcurrentQueueTest, EnqueueFailsIfQueueSizeLimitIsReached)
{
    constexpr size_t queueSize = 5;
    ConcurrentQueue<int> testQueue(queueSize);

    EXPECT_TRUE(testQueue.empty());

    int data = 1;
    for (size_t idx = 0; idx < queueSize; ++idx)
    {
        SCOPED_TRACE(::testing::Message() << "Inserting data at index: " << idx);
        EXPECT_TRUE(testQueue.enqueue(data++));
        EXPECT_EQ(testQueue.size(), idx + 1);
    }

    EXPECT_EQ(testQueue.size(), queueSize);

    // Once queue is at maximum capacity, en-queuing next element will fail.
    EXPECT_FALSE(testQueue.enqueue(data));
    EXPECT_EQ(testQueue.size(), queueSize);
}

TEST_F(ConcurrentQueueTest, DequeueFailsIfQueueIsEmpty)
{
    constexpr size_t queueSize = 5;
    ConcurrentQueue<int> testQueue(queueSize);

    EXPECT_TRUE(testQueue.empty());

    int val;
    EXPECT_FALSE(testQueue.dequeue(&val));
}

TEST_F(ConcurrentQueueTest, DequeueSucceeds)
{
    ConcurrentQueue<int> testQueue;
    EXPECT_TRUE(testQueue.empty());

    constexpr int testValue = 56;
    EXPECT_TRUE(testQueue.enqueue(testValue));
    EXPECT_EQ(testQueue.size(), 1);

    int val;
    EXPECT_TRUE(testQueue.dequeue(&val));
    EXPECT_EQ(val, testValue);
    EXPECT_TRUE(testQueue.empty());
}
