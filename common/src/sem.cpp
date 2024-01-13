#ifndef _STDEXCEPT_
# include <stdexcept>
#endif

#include <errno.h>
#include <cstdio>
#include <cstring>
#include "sem.h"
#include "logger.h"

#define NOT_USED(x)      ((void)(x))
 
using namespace std;

namespace Util
{
  Sem::Sem(sem_t* sem, unsigned int inInitialCount) :
    sem(sem)
  {
    if (sem_init(sem, 1, inInitialCount) != 0)
    {
      Logger::abort("Could not create semaphore: %s", strerror(errno));
    }
  }

  Sem::~Sem ()
  {
    if (sem_destroy(sem) != 0)
    {
      Logger::abort("Could not destroy semaphore: %s", strerror(errno));
    }
  }

  void Sem::signal(const unsigned long inPostCount)
  {
    NOT_USED(inPostCount);
    if (sem_post(sem) != 0)
    {
      Logger::abort("Semaphore Post failed: %s", strerror(errno));
    }
  }

  void Sem::wait(const unsigned long inTimeout)
  {
    NOT_USED(inTimeout);
    if (sem_wait(sem) != 0)
    {
      Logger::abort("Semaphore wait failed: %s", strerror(errno));
    }
  }
} // namespace Util
