#include "threadpool.h"
using namespace std;

int main(int argc, char **argv)
{
    mutex mtx;
    try{
        CThreadpool pool;
        vector<future<int>> v1;
        vector<future<void>> v2;
        
        for(int i = 0; i < 10; ++i){
            auto ans = pool.add([](int answer){return answer;}, i);
            v1.push_back(std::move(ans));
        }
        for(int i = 0; i < 5; ++i){
            auto ans = pool.add([&mtx](const string& str1, const string& str2){
                lock_guard<mutex> lg(mtx);
                cout << str1 + str2 << endl;
                return ;
            }, "hello", "world");
            v2.push_back(std::move(ans));
        }

        for(size_t i = 0; i < v1.size(); ++i){
            lock_guard<mutex> lg(mtx);
            cout << v1[i].get() << endl;
        }
        for(size_t i = 0; i < v2.size(); ++i){
            v2[i].get();
        }
    }catch(exception& ex){
        cout << ex.what() << endl;
    }
    return 0;
}