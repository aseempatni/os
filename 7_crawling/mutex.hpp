#ifndef MUTEX_HPP
#define MUTEX_HPP

# include <pthread.h>
# include <errno.h>
# include <string.h>
# include <stdio.h>

class Mutex
  {
  private:
    pthread_mutex_t mutex;

  public:
    pthread_mutex_t* getPointer()
    {
      return &mutex;
    }
    inline Mutex()
    {
      if (pthread_mutex_init(&mutex, NULL) != 0)
      {
        fprintf(stderr, "Failed to initialize mutex: %s", strerror(errno));
      }
    }

    inline ~Mutex()
    {
      if (pthread_mutex_destroy(&mutex) != 0)
      {
        fprintf(stderr, "Failed to destroy mutex: %s", strerror(errno));
      }
    }

    inline void lock()
    {
      if (pthread_mutex_lock(&mutex) != 0)
      {
        fprintf(stderr, "Failed to lock mutex: %s", strerror(errno));
      }
    }

    inline void unlock()
    {
      if (pthread_mutex_unlock(&mutex) != 0)
      {
        fprintf(stderr, "Failed to unlock mutex: %s", strerror(errno));
      }
    }

    inline void tryLock()
    {
      if (pthread_mutex_trylock(&mutex) != 0)
      {
        fprintf(stderr, "Failed trying to lock mutex: %s", strerror(errno));
      }
    }
  };

#endif