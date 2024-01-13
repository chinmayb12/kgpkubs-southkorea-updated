#include <cstdio>
#include <cstdlib>
#include <string>
#include <stdexcept>
#include "logger.h"
#include "shmem.h"

using namespace std;

namespace Util
{
  Shmem::Shmem(key_t key, size_t size)
  {
    // Adding extra space for storing the semaphore
    if ((shmid = shmget(key, size + sizeof(sem_t), IPC_CREAT | SHM_R | SHM_W)) < 0)
    {
      Logger::abort("shmget failed: %s", strerror(errno));
    }

    pBuf = shmat(shmid, NULL, 0);
    if (pBuf == (void *) - 1)
    {
      Logger::abort("shmat failed: %s", strerror(errno));
    }

    sem = (sem_t *)((char *)pBuf + size);   // Semaphore is stored at the end of the shared memory block created

    mutex = new Sem(sem, 1);
  }

  Shmem::~Shmem()
  {
    if (shmdt(pBuf) != 0)
    {
      Logger::abort("shmdt failed: %s", strerror(errno));
    }
  }

  int Shmem::read(void *buffer, unsigned long size)
  {
    mutex->wait();
    memcpy(buffer, pBuf, size);
    mutex->signal();
    return 0;
  }

  int Shmem::write(const void *buffer, unsigned long size)
  {
    mutex->wait();
    memcpy(pBuf, buffer, size);
    mutex->signal();
    return 0;
  }
} // namespace Util
