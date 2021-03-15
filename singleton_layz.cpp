#include "singleton_layz.h"

std::unique_ptr<singleton_layz> singleton_layz::instance_;
std::mutex singleton_layz::ins_mtx_;
singleton_layz *singleton_layz::instance()
{
    // 这里使用了两个 if判断语句的技术称为双检锁; 好处是，只有判断指针为空的时候才加锁，
    // 避免每次调用 instance() 方法都加锁，锁的开销毕竟还是有点大的。
    if (!instance_)
    {
        std::unique_lock<std::mutex> lck(ins_mtx_); // 加锁
        if (!instance_)
        {
            instance_.reset(new (std::nothrow) singleton_layz);
        }
    }
    return instance_.get();
}

void singleton_layz::destory()
{
    if (instance_)
    {
        std::unique_lock<std::mutex> lck(ins_mtx_); // 加锁
        instance_.reset();
    }
}

singleton_layz::singleton_layz()
{
}

singleton_layz::~singleton_layz()
{
}
