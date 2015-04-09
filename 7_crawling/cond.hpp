#ifndef COND_HPP
#define COND_HPP

# include <pthread.h>
# include <errno.h>
# include <string.h>
# include <stdio.h>

class Cond
  {
  private:
    pthread_cond_t cond;
    Mutex *m;

  public:
    inline Cond(Mutex *m_):m(m_)
    {
      if (pthread_cond_init(&cond, NULL) != 0)
      {
        fprintf(stderr, "Failed to initialize cond: %s", strerror(errno));
      }
    }

    inline ~Cond()
    {
      if (pthread_cond_destroy(&cond) != 0)
      {
        fprintf(stderr, "Failed to destroy cond: %s", strerror(errno));
      }
    }

    inline void wait()
    {
      if (pthread_cond_wait(&cond,m->getPointer()) != 0)
      {
        fprintf(stderr, "Failed to lock cond: %s", strerror(errno));
      }
    }

    inline void signal()
    {
      if (pthread_cond_signal(&cond) != 0)
      {
        fprintf(stderr, "Failed to lock cond: %s", strerror(errno));
      }
    }

    inline void broadcast()
    {
      if (pthread_cond_broadcast(&cond) != 0)
      {
        fprintf(stderr, "Failed to lock cond: %s", strerror(errno));
      }
    }
  };

#endif