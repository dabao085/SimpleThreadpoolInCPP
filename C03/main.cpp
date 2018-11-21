#include "threadpool.h"
#include <string>
using namespace std;

class CMyTask
{
public:
    int run(int i, const char *str)
    {
        printf("%d : %s\n", i, str);
        return 0;
    }
};

int main(int argc, char **argv)
{
    int i;
    CMyTask task[20];
    CThreadpool pool(1);

    for(i = 0; i < 20; ++i){
        pool.add(std::bind(&CMyTask::run, &task[i], i, "hello world"));
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
    cout << endl;

    return 0;
}