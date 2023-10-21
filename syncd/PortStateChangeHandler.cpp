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

#include <string>

using namespace syncd;

PortStateChangeHandler::PortStateChangeHandler(
        _In_ std::shared_ptr<swss::SelectableEvent> portStateChangeEvent)
    : m_portStateChangeEvent(portStateChangeEvent)
{
    SWSS_LOG_ENTER();

    m_portStateChangeQueue = std::make_shared<portOperStatusNotificationQueue>(
            m_portStateChangeQueueSize);
}

PortStateChangeHandler::~PortStateChangeHandler()
{
    SWSS_LOG_ENTER();
}

void PortStateChangeHandler::handlePortStateChangeNotification(
        _In_ uint32_t count,
        _In_ const sai_port_oper_status_notification_t *data)
{
    SWSS_LOG_ENTER();

    if (m_portStateChangeEvent == nullptr)
    {
        SWSS_LOG_THROW("Unexpected error: port state change event is null.");
    }

    for (uint32_t idx = 0; idx < count; ++idx)
    {
        if (m_portStateChangeQueue->enqueue(data[idx]) == false)
        {
            SWSS_LOG_ERROR(
                    "Unexpected error: failed to enqueue the port state change "
                    "notification.");

            return;
        }
    }

    m_portStateChangeEvent->notify();
}
