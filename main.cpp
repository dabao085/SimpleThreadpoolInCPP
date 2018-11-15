#include "threadpool.h"
using namespace std;

class CMyTask: public CTask
{
public:
    CMyTask(void *inputParam = NULL, void *outputParam = NULL):inputParam_(inputParam), outputParam_(outputParam){}
public:
    virtual int run()
    {
        int *p = static_cast<int*>(inputParam_);
        *p = *p * 2;
        //outputParam_ = inputParam_;
        *(int*)outputParam_ = *p;
        printf("%ld hello world \n", pthread_self());
        return 0;
    }    
    void setParam(void *input, void *output)
    {
        inputParam_ = input;
        outputParam_ = output;
    }
private:
    void *inputParam_;
    void *outputParam_;
};

int main(int argc, char **argv)
{
    int i, input[20], output[20];
    CMyTask task[20];
    CThreadpool pool(1);

    for(i = 0; i < 20; ++i){
        input[i] = i;
        // CMyTask task((void*)&input[i], (void*)&output[i]);
        task[i].setParam(static_cast<void*>(&input[i]), static_cast<void*>(&output[i]));
        pool.add(&task[i]);
    }

    while(1){
        printf("%d task left\n", pool.size());
        sleep(2);
        if(pool.size() == 0){
            pool.stop();
            printf("exit from main...\n");
            break;
        }
    }
    
    for(i = 0;i < 20; ++i){
        printf("%d ", output[i]);
    }
    cout << endl;

    return 0;
}