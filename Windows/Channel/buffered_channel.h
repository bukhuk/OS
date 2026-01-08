#ifndef BUFFERED_CHANNEL_H_
#define BUFFERED_CHANNEL_H_

#include <condition_variable>
#include <mutex>
#include <queue>
#include <stdexcept>
#include <utility>

template <class T>
class BufferedChannel {
public:
    explicit BufferedChannel(int size) : buffer_size_(size), is_closed_(false) {
        if (size <= 0) {
            throw std::invalid_argument("Size must be positive");
        }
    }

    void Send(T value) {
        std::unique_lock<std::mutex> lock(mutex_);

        send_cv_.wait(lock, [this]() {
            return queue_.size() < buffer_size_ || is_closed_;
        });

        if (is_closed_) {
            throw std::runtime_error("Can't send to closed channel");
        }

        queue_.push(std::move(value));
        recv_cv_.notify_one();
    }

    std::pair<T, bool> Recv() {
        std::unique_lock<std::mutex> lock(mutex_);

        recv_cv_.wait(lock, [this]() {
            return !queue_.empty() || (is_closed_ && queue_.empty());
        });

        if (!queue_.empty()) {
            T value = std::move(queue_.front());
            queue_.pop();
            send_cv_.notify_one();
            return {std::move(value), true};
        }

        return {T(), false};
    }

    void Close() {
        std::unique_lock<std::mutex> lock(mutex_);
        is_closed_ = true;
        send_cv_.notify_all();
        recv_cv_.notify_all();
    }

    ~BufferedChannel() {
        Close();
    }

private:
    std::queue<T> queue_;
    const size_t buffer_size_;
    bool is_closed_;

    std::mutex mutex_;
    std::condition_variable send_cv_;
    std::condition_variable recv_cv_;
};

#endif  // BUFFERED_CHANNEL_H_