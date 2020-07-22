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
public:
	void lock_read()
	{
		std::unique_lock<std::mutex> ulk(counter_mutex);
		cond_r.wait(ulk, [=]()->bool {return write_cnt == 0; });
		++read_cnt;
	}
	void lock_write()
	{
		std::unique_lock<std::mutex> ulk(counter_mutex);
		++write_cnt;
		cond_w.wait(ulk, [=]()->bool {return read_cnt == 0 && !inwriteflag; });
		inwriteflag = true;
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
		inwriteflag = false;
	}
	
private:
	volatile size_t read_cnt{ 0 };
	volatile size_t write_cnt{ 0 };
	volatile bool inwriteflag{ false };
	std::mutex counter_mutex;
	std::condition_variable cond_w;
	std::condition_variable cond_r;
};
 
template <typename _rw_lockable>
class unique_wguard
{
public:
	explicit unique_wguard(_rw_lockable &rw_lckable)
		: rw_lckable_(rw_lckable)
	{
		rw_lckable_.lock_write();
	}
	~unique_wguard()
	{
		rw_lckable_.unlock_write();
	}
private:
	unique_wguard() = delete;
	unique_wguard(const unique_wguard&) = delete;
	unique_wguard& operator=(const unique_wguard&) = delete;
private:
	_rw_lockable &rw_lckable_;
};
template <typename _rw_lockable>
class unique_rguard
{
public:
	explicit unique_rguard(_rw_lockable &rw_lckable)
		: rw_lckable_(rw_lckable)
	{
		rw_lckable_.lock_read();
	}
	~unique_rguard()
	{
		rw_lckable_.unlock_read();
	}
private:
	unique_rguard() = delete;
	unique_rguard(const unique_rguard&) = delete;
	unique_rguard& operator=(const unique_rguard&) = delete;
private:
	_rw_lockable &rw_lckable_;
};
 
#endif