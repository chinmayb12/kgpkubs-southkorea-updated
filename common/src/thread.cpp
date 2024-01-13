#include <errno.h>
#include <unistd.h>
#include <cstring>
#include "thread.h"
#include "logger.h"

namespace Util
{
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
    if (pthread_create(&handle, NULL, threadFunc, this) != 0)
    {
      Logger::abort("Could not create thread: %s", strerror(errno));
    }

    std::cout<<"start"<<std::endl;
  }

  void Thread::sleep(long ms)
  {
    usleep(ms * 1000);
  }

  void Thread::stop()
  {
    assert(handle);
    // BUG changing this from pthread_exit to pthread_cancel as sleep in vThread caused SIGABORT
    pthread_cancel(handle);
    handle = 0;
  }

  void* Thread::threadFunc(void* arg)
  {
    reinterpret_cast<Thread *>(arg)->run();
    return 0;
  }
} // namespace Util
