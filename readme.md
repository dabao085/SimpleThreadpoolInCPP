# 简单的C++线程池
分别用C98、C03、C11实现

1. C98实现
封装任务放入队列中,由工作线程取出,任务基类为CTask,具体任务继承自CTask,并实现int run();

2. C03实现
使用`std::function`做为回调对象,替换CTask,执行具体的任务。
   
3. C11实现
使用C++11的写法实现线程池。
   
4. 使用方法
进入各文件夹,比如C98,执行
```shell
cd SimpleThreadpoolInCPP
cd C98
make
./threadpool98
```