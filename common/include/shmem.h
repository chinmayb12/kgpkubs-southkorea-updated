#ifndef SHARED_MEMORY_H
#define SHARED_MEMORY_H

#ifdef WIN32
#include "winsock2.h"
#else
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#endif // !WIN32

#include "sem.h"

namespace Util
{
  class Shmem
  {
  private:
#ifdef WIN32
    HANDLE        hMapFile;
    LPCTSTR       pBuf;
    char          smName[200];
    char          spName[200];
#else
    key_t  key;    // key to be passed to shmget()
    int    shmid;  // return value from shmget()
    void*  pBuf;
    sem_t* sem;    // semaphore to be used
#endif // !WIN32

    Sem*    mutex;

    // Disable copy constructor and assignment operator
    Shmem(const Shmem &);
    Shmem & operator = (const Shmem &);

  public:
#ifdef WIN32
    Shmem(const char* smName, unsigned long size);
#else
    Shmem(key_t key, size_t size);
#endif

    ~Shmem();

    // Reads data from the shared memory into buffer
    int read(void *buffer, unsigned long size);

    // Writes data to the shared memory from buffer
    int write(const void* buffer, unsigned long size);
  }; // SharedMemory
}

#endif // SHARED_MEMORY_H
