#pragma once
#include <deque>
#include <mutex>
#include <vector>

template<typename T>
class ThreadSafeQueue
{
public:
    void Push(T item)
    {
        std::lock_guard lock(mtx_);
        queue_.push_back(std::move(item));
    }

    bool TryPop(T& out)
    {
        std::lock_guard lock(mtx_);
        if (queue_.empty()) return false;
        out = std::move(queue_.front());
        queue_.pop_front();
        return true;
    }

    std::vector<T> Drain()
    {
        std::lock_guard lock(mtx_);
        std::vector<T> result;
        result.reserve(queue_.size());
        while (!queue_.empty())
        {
            result.push_back(std::move(queue_.front()));
            queue_.pop_front();
        }
        return result;
    }

    void Clear()
    {
        std::lock_guard lock(mtx_);
        queue_.clear();
    }

    size_t Size() const
    {
        std::lock_guard lock(mtx_);
        return queue_.size();
    }

private:
    mutable std::mutex mtx_;
    std::deque<T> queue_;
};