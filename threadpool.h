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
* @file threadpool.h
* @brief Threadpool header file
*/
#ifndef _THREADPOOL_H_
#define _THREADPOOL_H_

#include <iostream>
#include <deque>
#include <string>
#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <assert.h>
#include <sys/types.h>
#include <malloc.h>

//任务基类
class CTask
{
public:
    CTask(){}
    virtual ~CTask(){}
public:
    void setTaskName(void *taskName = NULL)
    {
        taskName_ = taskName;
    }
    virtual int run() = 0;    
protected:
    void *taskName_;                                //任务标记
};

//线程池类
class CThreadpool
{
public:
    CThreadpool(int num = 10);
    ~CThreadpool();
public:
    const int size();
    void stop();
    int add(CTask* task);
    CTask *take();
private:
    int createThread();
    //工作线程
    static void *threadFunc(void *);
private:
    CThreadpool &operator=(const CThreadpool &);    //Effective C++ Item 6
    CThreadpool(const CThreadpool &);               //Effective C++ Item 6
private:
    int threadNum_;                                 //工作线程数
    int isRunning_;                                 //线程池运行与停止状态
    pthread_t *threads_;                            //工作线程的pthread_t id
    std::deque<CTask*> queue_;                      //任务队列
    pthread_mutex_t lock_;                          //mutex
    pthread_cond_t notify_;                         //condition
};
#endif