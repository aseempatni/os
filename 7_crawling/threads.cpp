#ifndef THREAD_CC
#define THREAD_CC

#include <errno.h>
#include <unistd.h>
#include <iostream>
#include <cstring>
#include "threads.hpp"
#include <stdio.h>

using namespace std;

Thread::Thread() :
  handle(0)
{ }

Thread::~Thread()
{
  if (handle != 0)
  {
    stop();
  }
}

void Thread::start()
{
  if(pthread_create(&handle, NULL, threadFunc, this) != 0)
    cerr << "Already running\n";
}

uint Thread::id()
{
  return (uint)pthread_self();
}

void Thread::sleep(long ms)
{
  usleep(ms * 1000);
}

void Thread::stop()
{
  pthread_cancel(handle);

  handle = 0;
}

void Thread::wait()
{
  pthread_join(handle, NULL);
}

void* Thread::threadFunc(void* arg)
{
  reinterpret_cast<Thread * >(arg)->run();
  return 0;
}
/* void Thread::setPriority(int tp)
{

  assert(handle); // Thread object is null. Priority cannot be set
  int ret = SetThreadPriority(handle, tp);
  assert(ret); // Failed to set priority
}

void Thread::suspend()
{
  assert(handle); // Thread object is null
  int ret = SuspendThread(handle);
  assert(ret >= 0); // Failed to suspend thread
}

void Thread::resume()
{
  assert(handle); // Thread object is null
  int ret = ResumeThread(handle);
  assert(ret >= 0); // Failed to resume thread
}*/
#endif