#ifndef SINGLETON_EAGER_H
#define SINGLETON_EAGER_H
/*
 * 饿汉模式，单例模式(线程安全)
 * 程序启动时分配内存，程序结束通过智能指针自动释放
 */
#include <memory>

class singleton_eager
{
public:
    // 析构函数需要public, unique_ptr管理释放
    ~singleton_eager();
    // 获取单实例
    static singleton_eager* instance();

private:
    // 将其构造成为私有的, 禁止外部构造
    explicit singleton_eager();

    // 禁止外部拷贝和赋值
    singleton_eager(const singleton_eager &ins) = delete;
    const singleton_eager &operator=(const singleton_eager &ins) = delete;

private:
    // 唯一单实例对象指针
    static std::unique_ptr<singleton_eager> instance_;
};

#endif // SINGLETON_EAGER_H
