#pragma once

#include <type_traits>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <queue>

namespace mrpc
{

template<typename T>
    requires((std::is_move_constructible_v<T> && std::is_move_assignable_v<T>) ||
        (std::is_copy_constructible_v<T> && std::is_copy_assignable_v<T>))
class ThreadSafeQueue
{
public:
    ThreadSafeQueue() = default;
    ~ThreadSafeQueue() = default;

    ThreadSafeQueue(const ThreadSafeQueue&) = delete;
    ThreadSafeQueue& operator=(const ThreadSafeQueue&) = delete;

    void push(const T& value)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        queue_.push(value);
        cv_.notify_one();
    }

    void push(T&& value)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        queue_.push(std::move(value));
        cv_.notify_one();
    }

    bool try_pop(T& value)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (queue_.empty())
        {
            return false;
        }

        if constexpr (std::is_move_constructible_v<T> && std::is_move_assignable_v<T>)
        {
            value = std::move(queue_.front());
        }
        else if constexpr (std::is_copy_constructible_v<T> && std::is_copy_assignable_v<T>)
        {
            value = queue_.front();
        }
        else
        {
            static_assert(false);
        }
        queue_.pop();
        return true;
    }

    void wait_and_pop(T& value)
    {
        std::unique_lock<std::mutex> lock(mutex_);
        cv_.wait(lock, [this] { return !queue_.empty(); });

        if constexpr (std::is_move_constructible_v<T> && std::is_move_assignable_v<T>)
        {
            value = std::move(queue_.front());
        }
        else if constexpr (std::is_copy_constructible_v<T> && std::is_copy_assignable_v<T>)
        {
            value = queue_.front();
        }
        else
        {
            static_assert(false);
        }
        queue_.pop();
    }

    bool wait_and_pop(T& value, const std::chrono::milliseconds& timeout)
    {
        std::unique_lock<std::mutex> lock(mutex_);
        if (!cv_.wait_for(lock, timeout, [this] { return !queue_.empty(); }))
        {
            return false;
        }

        if constexpr (std::is_move_constructible_v<T> && std::is_move_assignable_v<T>)
        {
            value = std::move(queue_.front());
        }
        else if constexpr (std::is_copy_constructible_v<T> && std::is_copy_assignable_v<T>)
        {
            value = queue_.front();
        }
        else
        {
            static_assert(false);
        }
        queue_.pop();
        return true;
    }

    bool empty() const
    {
        std::lock_guard<std::mutex> lock(mutex_);
        return queue_.empty();
    }

    size_t size() const
    {
        std::lock_guard<std::mutex> lock(mutex_);
        return queue_.size();
    }

private:
    mutable std::mutex mutex_;
    std::condition_variable cv_;
    std::queue<T> queue_;
};

}
