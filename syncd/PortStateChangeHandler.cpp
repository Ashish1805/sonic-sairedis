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
