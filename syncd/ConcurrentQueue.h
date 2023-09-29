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

#include <mutex>
#include <queue>

#include "swss/logger.h"
#include "swss/sal.h"

namespace syncd
{
    template <class T>
    class ConcurrentQueue
    {
        public:

            explicit ConcurrentQueue(
                    _In_ size_t queueSizeLimit = UNLIMITED);

            virtual ~ConcurrentQueue()  = default;

            bool enqueue(
                    _In_ const T& val);

            bool dequeue(
                    _Out_ T* valOut);

            size_t size();

            bool empty();

        private:

            // Queue size = 0 means there is no limit on queue size.
            static constexpr size_t UNLIMITED = 0;

            std::mutex m_mutex;
            std::queue<T> m_queue;
            size_t m_queueSizeLimit;

            ConcurrentQueue<T>(const ConcurrentQueue<T>&) = delete;
            ConcurrentQueue<T>& operator=(const ConcurrentQueue<T>&) = delete;
    };

    template <class T>
    ConcurrentQueue<T>::ConcurrentQueue(
              _In_ size_t queueSizeLimit)
        : m_queueSizeLimit(queueSizeLimit)
    {
        SWSS_LOG_ENTER();
    }

    template <class T>
    bool ConcurrentQueue<T>::enqueue(
            _In_ const T& val)
    {
        SWSS_LOG_ENTER();

        std::lock_guard<std::mutex> mutex_lock(m_mutex);

        // If the queue exceeds the limit, return false.
        if ((m_queueSizeLimit == UNLIMITED) || (m_queue.size() < m_queueSizeLimit))
        {
            m_queue.push(val);
            return true;
        }

        return false;
    }

    template <class T>
    bool ConcurrentQueue<T>::dequeue(
            _Out_ T* valOut)
    {
        SWSS_LOG_ENTER();

        std::lock_guard<std::mutex> mutex_lock(m_mutex);
        if (m_queue.empty())
        {
            return false;
        }

        *valOut = m_queue.front();
        m_queue.pop();

        return true;
    }

    template <class T>
    size_t ConcurrentQueue<T>::size()
    {
        SWSS_LOG_ENTER();

        std::lock_guard<std::mutex> mutex_lock(m_mutex);

        return m_queue.size();
    }

    template <class T>
    bool ConcurrentQueue<T>::empty()
    {
        SWSS_LOG_ENTER();

        std::lock_guard<std::mutex> mutex_lock(m_mutex);

        return m_queue.empty();
    }
} // namespace syncd
