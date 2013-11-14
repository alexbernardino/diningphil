#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>

using namespace std;

int state[5];
condition_variable cv[5];
mutex mut;

const int hungry=0;
const int eating=1;
const int thinking=2;

bool test(int i)
{ 
   if( (state[i]         == hungry) && 
       (state[(i+1)%5]   != eating) &&
       (state[(i-1+5)%5] != eating) )
   {
      state[i]=eating;
      cv[i].notify_one();
      return false;
   }
   else
      return true;
}

void put_fork(int i)
{
   unique_lock<mutex> lck(mut);
   state[i] = thinking;
   test((i+1)%5);
   test((i-1+5)%5);   
}

void take_fork(int i)
{
   unique_lock<mutex> lck(mut);
   state[i] = hungry;
   if(test(i))
      cv[i].wait(lck);   
}

void phil(int i)
{
   int count = 0;
   while(count < 100)
   {
      take_fork(i);
      cout << "Phil " << i << " is eating " << count << endl;
      this_thread::yield();
      put_fork(i);
      cout << "Phil " << i << " is thinking " << count << endl;
      count++;
   }
}

int main()
{
   thread threads[5];
   for(int i=0; i<5; i++)
      threads[i]=thread(phil,i);
   for(auto& th : threads) 
      th.join();
}
