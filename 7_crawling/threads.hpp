// This filec contains the Thread class

#ifndef THREAD_HH
#define THREAD_HH

# include <pthread.h>
#include "mutex.hpp"
class Thread
{
private:
  pthread_t    handle;
  static void* threadFunc(void* arg);

public:
  Thread();

  ~Thread();

  void start();

  uint id();

  virtual void run()=0;

  void sleep(long ms);

  void stop();

  void wait();
};
#endif // THREAD_HH