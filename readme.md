# 简单的C++线程池
分别用C98、C03、C11实现

1. C98实现
封装任务放入队列中,由工作线程取出,任务基类为CTask,具体任务继承自CTask,并实现int run();

