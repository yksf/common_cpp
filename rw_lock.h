#ifndef __WRITE_FIRST_RW_LOCK_H
#define __WRITE_FIRST_RW_LOCK_H
 /*
  * 写优先的读写锁
  */
#include <mutex>
#include <condition_variable>
 
class rw_lock
{
public:
	rw_lock() = default;
	~rw_lock() = default;

	void lock_read()
	{
		std::unique_lock<std::mutex> ulk(counter_mutex);
		cond_r.wait(ulk, [=]()->bool { return write_cnt == 0; });
		++read_cnt;
	}
	void lock_write()
	{
		std::unique_lock<std::mutex> ulk(counter_mutex);
		++write_cnt;
		cond_w.wait(ulk, [=]()->bool { return read_cnt == 0 && !is_writing_; });
		is_writing_ = true;
	}
	void unlock_read()
	{
		std::unique_lock<std::mutex> ulk(counter_mutex);
		if (--read_cnt == 0 && write_cnt > 0)
		{
			cond_w.notify_one();
		}
	}
	void unlock_write()
	{
		std::unique_lock<std::mutex> ulk(counter_mutex);
		if (--write_cnt == 0)
		{
			cond_r.notify_all();
		}
		else
		{
			cond_w.notify_one();
		}
		is_writing_ = false;
	}
private:
	rw_lock(const rw_lock&) = delete;
	rw_lock(rw_lock&&) = delete;
	rw_lock& operator=(const rw_lock&) = delete;
	rw_lock& operator=(rw_lock&&) = delete;
	
private:
	int read_cnt{ 0 };
	int write_cnt{ 0 };
	bool is_writing_{ false };
	std::mutex counter_mutex;
	std::condition_variable cond_w;
	std::condition_variable cond_r;
};
 
template <typename _rw_lockable>
class unique_wguard
{
public:
	explicit unique_wguard(_rw_lockable &rw_lck)
		: rw_lck_(rw_lck)
	{
		rw_lck_.lock_write();
	}
	~unique_wguard()
	{
		rw_lck_.unlock_write();
	}
private:
	unique_wguard(const unique_wguard&) = delete;
	unique_wguard(unique_wguard&&) = delete;
	unique_wguard& operator=(const unique_wguard&) = delete;
	unique_wguard& operator=(unique_wguard&&) = delete;
private:
	_rw_lockable &rw_lck_;
};
template <typename _rw_lockable>
class unique_rguard
{
public:
	explicit unique_rguard(_rw_lockable &rw_lck)
		: rw_lck_(rw_lck)
	{
		rw_lck_.lock_read();
	}
	~unique_rguard()
	{
		rw_lck_.unlock_read();
	}
private:
	unique_rguard(const unique_rguard&) = delete;
	unique_rguard(unique_rguard&&) = delete;
	unique_rguard& operator=(const unique_rguard&) = delete;
	unique_rguard& operator=(unique_rguard&&) = delete;
private:
	_rw_lockable &rw_lck_;
};
 
#endif