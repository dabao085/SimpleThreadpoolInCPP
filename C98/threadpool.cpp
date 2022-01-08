/*
* Copyright (c) 2018, Leonardo Cheng <chengxiang085@gmail.com>.
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are
* met:
*
*  1. Redistributions of source code must retain the above copyright
*     notice, this list of conditions and the following disclaimer.
*
*  2. Redistributions in binary form must reproduce the above copyright
*     notice, this list of conditions and the following disclaimer in the
*     documentation and/or other materials provided with the distribution.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
* "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
* LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
* A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
* HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
* SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
* LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
* DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
* THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
* OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

/**
* @file threadpool.cpp
* @brief Threadpool implementation file
*/

#include "threadpool.h"
#include <assert.h>
#include <sys/types.h>
#include <malloc.h>
#include <string>
#include <iostream>
#include <new>  // std::bad_alloc

/**
* @function CThreadPool
* @brief CThreadPool's constructor
* @param num Number of worker threads.
* @return 
*/
CThreadPool::CThreadPool(int num):isRunning_(true), threadNum_(num), threads_(NULL)
{
    assert(threadNum_ > 0);

    if(createThread() < 0){
        // 还要做别的错误处理吗？
        std::cerr << "createThread error!" << std::endl;
    }
}

/**
* @function ~CThreadPool
* @brief CThreadPool's destructor
* @return 
*/
CThreadPool::~CThreadPool()
{
    stop();
    std::deque<CTask*>::iterator iter = queue_.begin();
    for(; iter != queue_.end(); ++iter){
        delete *iter;
    }
    queue_.clear();
}

/**
* @function createThread
* @brief create threadNum_ worker threads
* @return 0 if succeed, -1 on failed
*/
int CThreadPool::createThread()
{
    pthread_mutex_init(&lock_, NULL);
    pthread_cond_init(&notify_, NULL);

    //threads_ = (pthread_t*)malloc(sizeof(pthread_t) * threadNum_);
    try{
        threads_ = new pthread_t[threadNum_];
    }catch(const std::bad_alloc& err){
        std::cerr << "malloc error! " << err.what() << std::endl;

        pthread_mutex_destroy(&lock_);
        pthread_cond_destroy(&notify_);

        return -1;
    }

    for(int i = 0; i < threadNum_; ++i){
        if(pthread_create(&threads_[i], NULL, threadFunc, this) != 0){
            std::cerr << "pthread_create error!" << std::endl;
            return -1;
        }
    }

    return 0;
}

/**
* @function size
* @brief return task queue's size
* @return task queue's size
*/
size_t CThreadPool::size()
{
    pthread_mutex_lock(&lock_);
    size_t size = queue_.size();
    pthread_mutex_unlock(&lock_);
    return size;
}

/**
* @function add
* @brief add task to task queue
* @param task pointer to task which is added to task queue
* @return 0 if succeed, -1 if failed
*/
int CThreadPool::addTask(CTask *task)
{
    //检查线程池是否已经停止
    pthread_mutex_lock(&lock_);
    if(!isRunning_){
        pthread_mutex_unlock(&lock_);
        return -1;
    }

    //否则继续向线程池添加任务
    queue_.push_back(task);
    //发送消息
    //TODO: 这里每次都要发送一个信号吗?效率是否受影响? unlock放在signal之前还是之后好？
    pthread_cond_signal(&notify_);
    pthread_mutex_unlock(&lock_);

    return 0;
}

/**
* @function stop
* @brief stop the threadpool
*/
void CThreadPool::stop()
{
    // TODO: 这里需要加锁来保护isRunning_吗？
    if(!isRunning_){
        return ;
    }

    isRunning_ = false;
    pthread_cond_broadcast(&notify_);
    //thread_join
    for(int i = 0; i < threadNum_; ++i){
        pthread_join(threads_[i], NULL);
    }

    //delete memory
    delete [] threads_;
    threads_ = NULL;

    pthread_mutex_destroy(&lock_);
    pthread_cond_destroy(&notify_);
}


/**
* @function take
* @brief take the task from threadpool
* @return the pointer to task
*/
CTask* CThreadPool::takeTask()
{
    CTask * task = NULL;
    while(!task){
        pthread_mutex_lock(&lock_);
        while(queue_.empty() && isRunning_){
            pthread_cond_wait(&notify_, &lock_);
        }

        if(!isRunning_){
            pthread_mutex_unlock(&lock_);
            break;
        }else if(queue_.empty()){
            pthread_mutex_unlock(&lock_);
            continue;
        }

        task = queue_.front();
        queue_.pop_front();
        assert(task != NULL);
        pthread_mutex_unlock(&lock_);
    }

    return task;
}

/**
* @function threadFunc
* @brief worker thread
* @param input param
* @return 
*/
void *CThreadPool::threadFunc(void * args)
{
    assert(args != NULL);
    CThreadPool *pool = static_cast<CThreadPool*>(args);
    // TODO: 这里pool->isRunning_是否需要加锁保护呢？
    while(pool->isRunning_){
        CTask *task = pool->takeTask();
        if(!task){
            std::cout << "thread " << pthread_self() << " exit" << std::endl;
            break;
        }

        assert(task != NULL);
        task->run();
    }

    return NULL;
}