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

#pragma once

extern "C" {
#include "saimetadata.h"
}

#include <memory>

#include "ConcurrentQueue.h"
#include "swss/logger.h"
#include "swss/selectableevent.h"

namespace syncd
{

    // Class to handle the port state change callback from SAI. This consists a
    // selectable event that will be used to send notification from producer thread
    // to consumer thread, and a mutex protected concurrent queue to share the port
    // state change notification data between producer and consumer threads.
    class PortStateChangeHandler
    {
        public:

            using portOperStatusNotificationQueue =
                    ConcurrentQueue<sai_port_oper_status_notification_t>;

            PortStateChangeHandler(
                    _In_ std::shared_ptr<swss::SelectableEvent> portStateChangeEvent);

            virtual ~PortStateChangeHandler();

            // Adds the port operational status notification data to a queue and generates a
            // notification event.
            void handlePortStateChangeNotification(
                    _In_ uint32_t count,
                    _In_ const sai_port_oper_status_notification_t *data);

            // Returns the shared pointer of the queue.
            std::shared_ptr<portOperStatusNotificationQueue> getQueue() const
            {
                SWSS_LOG_ENTER();

                return m_portStateChangeQueue;
            }

        private:

            // Choosing 4k max event queue size based on if we had 256 ports, it can
            // accommodate on average 16 port events per ports in worst case.
            static constexpr size_t m_portStateChangeQueueSize = 4096;

            // SelectableEvent for producer to generate the event and for consumer to
            // listen on.
            std::shared_ptr<swss::SelectableEvent> m_portStateChangeEvent;

            // Mutex protected queue to share the data between producer and consumer.
            std::shared_ptr<portOperStatusNotificationQueue> m_portStateChangeQueue;
    };
}  // namespace syncd
