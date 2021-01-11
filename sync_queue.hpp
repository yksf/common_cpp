#ifndef __SYNC_QUEUE_H__
#define __SYNC_QUEUE_H__

#include <list>
#include <condition_variable>

template<typename T>
class sync_queue
{
public:
  explicit sync_queue(size_t max_size = 5000) : queue_max_size_(max_size) {}
  ~sync_queue() {}

  static std::shared_ptr<sync_queue<T>> get_queue(size_t max_size = 5000)
  {
    if (nullptr == sync_queue_)
      sync_queue_.reset(new sync_queue<T>(max_size));
    return sync_queue_;
  }

  void push_back(const T& x)
  {
    append(x);
  }

  void push_back(T&& x)
  {
    append(std::forward<T>(x));
  }

  void push_back(T& x)
  {
    append(std::forward<T>(x));
  }

  T pop_front()
  {
    T tmp;
    std::unique_lock<std::mutex> lck(queue_mtx_);
    not_empty_cv_.wait(lck, [this] { return need_stop_ || not_empty(); });
    if (need_stop_) return tmp;
    
    tmp = std::move(queue_.front());
    queue_.pop_front();
    if (not_full()) not_full_cv_.notify_one();
    return std::move(tmp);
  }

  std::list<T> take_list()
  {
    std::list<T> tmp_queue;
    std::unique_lock<std::mutex> lck(queue_mtx_);
    not_empty_cv_.wait(lck, [this] { return need_stop_ || not_empty(); });
    if (need_stop_) return tmp_queue;
    tmp_queue = std::move(queue_);
    not_full_cv_.notify_one();
    return std::move(tmp_queue);
  }

  void stop()
  {
    {
      std::lock_guard<std::mutex> lck(queue_mtx_);
      need_stop_ = true;
    }

    not_full_cv_.notify_all();
    not_empty_cv_.notify_all();
  }

  bool empty()
  {
    std::lock_guard<std::mutex> lck(queue_mtx_);
    return queue_.empty();
  }

  bool full()
  {
    std::lock_guard<std::mutex> lck(queue_mtx_);
    return queue_.size() >= queue_max_size_;
  }

  size_t size()
  {
    std::lock_guard<std::mutex> lck(queue_mtx_);
    return queue_.size();
  }

private:
  bool not_full() const
  {
    return queue_.size() < queue_max_size_;
  }

  bool not_empty() const
  {
    return !queue_.empty();
  }

  void append(T&& x)
  {
    std::unique_lock<std::mutex> lck(queue_mtx_);
    not_full_cv_.wait(lck, [this] { return need_stop_ || not_full(); });
    if (need_stop_) return;
    queue_.emplace_back(std::forward<T>(x));
    not_empty_cv_.notify_one();
  }

private:
  bool need_stop_ = false;
  size_t queue_max_size_ = 5000;
  std::list<T> queue_;
  std::mutex queue_mtx_;
  std::condition_variable not_empty_cv_;
  std::condition_variable not_full_cv_;

  static std::shared_ptr<sync_queue<T>> sync_queue_;
};

template<typename T>
std::shared_ptr<sync_queue<T>> sync_queue<T>::sync_queue_ = nullptr;

#endif // !__SYNC_QUEUE_H__

