#ifndef SEM_H
#define SEM_H

#include <semaphore.h>

namespace Util
{
  class Sem
  {
  private:

    // disable copy constructor and assignment operator
    Sem (const Sem &);
    Sem & operator = (const Sem &);

#ifdef WIN32
    HANDLE theSemaphoreHandle;
    unsigned long theMaxCount;
#else
    sem_t* sem;
#endif // !WIN32

  public:
#ifdef WIN32
    Sem(const char * const inName = 0,
        const unsigned long inInitialCount = 0,
        const unsigned long inMaximumCount = 0);
#else
    Sem(sem_t* sem, unsigned int inInitialCount = 0);
#endif // !WIN32

    ~Sem ();

    // increments semaphore counter by inPostCount. inPostCount must not be 0
    void signal(const unsigned long inPostCount = 1);

    // Waits while semaphore counter is greater than 0 and decrements semaphore counter by 1
    void wait(const unsigned long inTimeout = 0);
  };
}

#endif // SEM_H
