#ifndef SINGLETON_LAYZ_H
#define SINGLETON_LAYZ_H

/*
 * 懒汉模式，单例模式(线程安全)
 * 需要时分配内存（堆），程序结束通过智能指针自动释放（也可调用destory随时销毁内存）
 */
#include <mutex>
#include <memory>

class singleton_layz
{
public:
    // 析构函数需要public, unique_ptr管理释放
    ~singleton_layz();
    // 获取单实例
    static singleton_layz* instance();
    // 释放单实例，进程退出时调用
    static void destory();

private:
    // 将其构造成为私有的, 禁止外部构造
    explicit singleton_layz();

    // 禁止外部拷贝和赋值
    singleton_layz(const singleton_layz &ins) = delete;
    const singleton_layz &operator=(const singleton_layz &ins) = delete;

private:
    // 唯一单实例对象指针
    static std::unique_ptr<singleton_layz> instance_;
    static std::mutex ins_mtx_;
};

#endif // SINGLETON_LAYZ_H
