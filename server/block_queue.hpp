#pragma once
#include <vector>
#include <semaphore.h>

namespace server
{
//类模板
//约定 [head, tail)左闭右开
template <typename T>
class BlockQueue
{
public:
    BlockQueue(size_t s = 1024)
        : data_(s) //给vector开辟空间
        , head_(0)
        , tail_(0)
        , size_(0)
    {
        //初始化信号量
        sem_init(&sem_data_, 0, 0);
        sem_init(&sem_blank_, 0, s);//生产者关心的资源是“空格”,所以初始化为s

    }

    

    void PushBack(const T& value)
    {
        //在插入之前先去申请”空格资源“
        sem_wait(&sem_blank_);

        //这里不用判断队列是否已经满了，因为，如果满了，肯定申请不到空格资源，会挂起等待
        data_[tail_++] = value;
        if(tail_ >= data_.size())//基于环形队列
        {
            tail_ = 0;
        }

        ++size_;

        //插入一个数据，相当于空格资源-1，数据资源+1
        sem_post(&sem_data_);
    }

    void PopFront(T* value)//相当于将pop与top合二为一了
    {
        //数据资源-1
        sem_wait(&sem_data_);
        *value = data_[head_++];
        if(head_ >= data_.size())
        {
            head_ = 0;
        }
        --size_;
        //空格资源+1
        sem_post(&sem_blank_);
    }

private:
    std::vector<T> data_;
    sem_t sem_data_;
    sem_t sem_blank_;

    size_t head_;
    size_t tail_;
    size_t size_;
    //由于此时=处为单生产者单消费者，所以可以不加互斥锁
    sem_t sem_lock_;
};
} //end Server
