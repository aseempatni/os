#ifndef BAR_HPP
#define BAR_HPP

# include <pthread.h>
# include <errno.h>
# include <string.h>
# include <stdio.h>

class Bar
  {
  private:
    pthread_barrier_t bar;

  public:
    inline Bar(int Tthread)
    {
      if (pthread_barrier_init(&bar, NULL,Tthread) != 0)
      {
        fprintf(stderr, "Failed to initialize bar: %s", strerror(errno));
      }
    }

    inline ~Bar()
    {
      if (pthread_barrier_destroy(&bar) != 0)
      {
        fprintf(stderr, "Failed to destroy bar: %s", strerror(errno));
      }
    }

    inline void wait()
    {
      int rc = pthread_barrier_wait(&bar);
      if(rc != 0 && rc != PTHREAD_BARRIER_SERIAL_THREAD)
      {
          fprintf(stderr,"Could not wait on bar: %s\n",strerror(errno));
          exit(-1);
      }
    }
  };

#endif