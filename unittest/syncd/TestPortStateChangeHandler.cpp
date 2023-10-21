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

#include "PortStateChangeHandler.h"

#include <gtest/gtest.h>

using namespace syncd;

constexpr size_t portStateChangeQueueSize = 4096;

class PortStateChangeHandlerTest : public ::testing::Test
{
    protected:
        PortStateChangeHandlerTest()
            : m_portStateChangeHandler(std::make_shared<swss::SelectableEvent>())
        {
            SWSS_LOG_ENTER();
        }

        ~PortStateChangeHandlerTest() override
        {
            SWSS_LOG_ENTER();
        }

        PortStateChangeHandler m_portStateChangeHandler;
};

TEST_F(PortStateChangeHandlerTest, VerifyGetQueue)
{
    auto queue = m_portStateChangeHandler.getQueue();
    EXPECT_EQ(queue->size(), 0);
}

TEST_F(PortStateChangeHandlerTest,
       HandlePortStateChangeNotificationFailsOnEnqueuingData)
{
    auto queue = m_portStateChangeHandler.getQueue();
    EXPECT_EQ(queue->size(), 0);

    // Insert enough data in the queue so it reaches its capacity.
    sai_port_oper_status_notification_t operStatus[portStateChangeQueueSize];
    m_portStateChangeHandler.handlePortStateChangeNotification(
              portStateChangeQueueSize, &operStatus[0]);
    EXPECT_EQ(queue->size(), portStateChangeQueueSize);

    // Since queue is at its maximum capacity, adding a new element should cause
    // insert failure and new element should not get added.
    m_portStateChangeHandler.handlePortStateChangeNotification(/*count=*/1,
                                                               &operStatus[0]);
    EXPECT_EQ(queue->size(), portStateChangeQueueSize);
}

TEST_F(PortStateChangeHandlerTest, HandlePortStateChangeNotificationSucceeds)
{
    auto queue = m_portStateChangeHandler.getQueue();
    EXPECT_EQ(queue->size(), 0);

    sai_port_oper_status_notification_t operStatus;
    m_portStateChangeHandler.handlePortStateChangeNotification(/*count=*/1,
                                                               &operStatus);
    EXPECT_EQ(queue->size(), 1);
}
