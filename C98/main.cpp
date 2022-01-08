#include "threadpool.h"
#include <assert.h>
#include <unistd.h>
#include <iostream>
#include <vector>

using namespace std;

class CMyTask: public CTask
{
public:
    CMyTask(void *inputParam = NULL, void *outputParam = NULL):inputParam_(inputParam), outputParam_(outputParam){}
public:
    // 将入参乘以2并经过出参传出
    virtual int run()
    {
        int *p = static_cast<int*>(inputParam_);
        *p = *p * 2;
        *(int*)outputParam_ = *p;   // TODO: 这里如何修改成C++中的转型？
        cout << pthread_self() << " hello world" << endl;
        return 0;
    }
    void setParam(void *input, void *output)
    {
        assert(input != NULL && output != NULL);
        inputParam_ = input;
        outputParam_ = output;
    }
private:
    void *inputParam_;
    void *outputParam_;
};

int main(int argc, char **argv)
{
    const int kSleepSecond = 2;
    const int kInputSize = 20;  // 入参个数
    vector<int> input(kInputSize), output(kInputSize);
    vector<CMyTask> task(kInputSize);
    CThreadPool pool(1);

    for(int i = 0; i < kInputSize; ++i){
        input[i] = i;
        // CMyTask task((void*)&input[i], (void*)&output[i]);
        task[i].setParam(static_cast<void*>(&input[i]), static_cast<void*>(&output[i]));
        pool.addTask(&task[i]);
    }

    while(1){
        cout << pool.size() << " task left" << endl;
        sleep(kSleepSecond);   // 主线程每隔kSleepSecond秒打印一次线程池处理的情况
        if(pool.size() == 0){
            pool.stop();
            cout << "exit from main thread..." << endl;
            break;
        }
    }
    
    for(int i = 0;i < kInputSize; ++i){
        cout << output[i] << " ";
    }
    cout << endl;

    return 0;
}