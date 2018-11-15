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

/**
* @function CThreadpool
* @brief CThreadpool's constructor
* @param num Number of worker threads.
* @return 
*/
CThreadpool::CThreadpool(int num)
{
    assert(num > 0);
    threadNum_ = num;
    isRunning_ = true;
    threads_ = NULL;

    if(createThread() < 0){
        std::cerr << "createThread error!" << std::endl;
    }
}

/**
* @function ~CThreadpool
* @brief CThreadpool's destructor
* @return 
*/
CThreadpool::~CThreadpool()
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
int CThreadpool::createThread()
{
    int i;
    pthread_mutex_init(&lock_, NULL);
    pthread_cond_init(&notify_, NULL);

    threads_ = (pthread_t*)malloc(sizeof(pthread_t) * threadNum_);
    if(threads_ == NULL){
        std::cerr << "malloc error!" << std::endl;
        return -1;
    }

    for(i = 0; i < threadNum_; ++i){
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
const int CThreadpool::size()
{
    int size;
    pthread_mutex_lock(&lock_);
    size = queue_.size();
    pthread_mutex_unlock(&lock_);
    return size;
}

/**
* @function add
* @brief add task to task queue
* @param task pointer to task which is added to task queue
* @return 0 if succeed, -1 if failed
*/
int CThreadpool::add(CTask *task)
{
    //检查线程池是否已经停止
    pthread_mutex_lock(&lock_);
    if(!isRunning_){
        return -1;
    }

    //否则继续向线程池添加任务
    queue_.push_back(task);
    //发送消息
    pthread_cond_signal(&notify_);
    pthread_mutex_unlock(&lock_);

    return 0;
}

/**
* @function stop
* @brief stop the threadpool
*/
void CThreadpool::stop()
{
    int i;
    if(!isRunning_){
        return ;
    }

    isRunning_ = false;
    pthread_cond_broadcast(&notify_);
    //thread_join
    for(i = 0; i < threadNum_; ++i){
        pthread_join(threads_[i], NULL);
    }

    //free memory
    free(threads_);
    threads_ = NULL;

    pthread_mutex_destroy(&lock_);
    pthread_cond_destroy(&notify_);
}


/**
* @function take
* @brief take the task from threadpool
* @return the pointer to task
*/
CTask* CThreadpool::take()
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
void *CThreadpool::threadFunc(void * args)
{
    CThreadpool *pool = (CThreadpool*)args;
    while(pool->isRunning_){
        CTask *task = pool->take();
        if(!task){
            printf("thread %ld exit\n", pthread_self());
            break;
        }

        assert(task != NULL);
        task->run();
    }
}