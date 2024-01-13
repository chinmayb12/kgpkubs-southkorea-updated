#include <signal.h>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <unistd.h>
#include <sys/select.h>
#include <iostream>

#include "mainheaders.hpp"
#include "qtDebugger/qtDebuggerMain.h"
#include <QtGui/QApplication>
#include "tester.hpp"
#include "worker.hpp"
using namespace std;
using namespace Strategy; 
// logger is to be used for logging throughout the project. Use it in other files using extern

/* This variable decides if the application should continue running or not.
 * It is true by default and becomes false by pressing Ctrl+C during the runtime.
 */
static bool run = true;
const bool test = false;

const bool qtdebug = true;

// Define the function to be called when Ctrl+C (SIGINT) signal is sent to process
void signal_callback_handler(int signum)
{
  NOT_USED(signum);
  run = false;
}

void main_constructor(void) __attribute__((constructor));

/* This function performs some sanity checks required for the project
 * to run properly besides other initializations.
 * It is executed before main()
 */
void main_constructor(void)
{
  srand(time(NULL));
  // Register signal and signal handler
  signal(SIGINT, signal_callback_handler);
}
int kbhit(void)
{
  struct timeval tv;
  fd_set read_fd;
  /* Do not wait at all, not even a microsecond */
  tv.tv_sec=0;
  tv.tv_usec=0;
  /* Must be done first to initialize read_fd */
  FD_ZERO(&read_fd);
  /* Makes select() ask if input is ready:
  * 0 is the file descriptor for stdin */
  FD_SET(0,&read_fd);
  /* The first parameter is the number of the
  * largest file descriptor totextEdit = new QPlainTextEdit;
    setCentralWidget(textEdit); check + 1. */
  if(select(1, &read_fd, NULL, /*No writes*/NULL, /*No exceptions*/&tv) == -1)
    return 0; /* An error occured */
  /* read_fd now holds a bit map of files that are
  * readable. We test the entry for the standard
  * input (file 0). */
  if(FD_ISSET(0,&read_fd))
    /* Character pending on stdin */
    return 1;

  /* no characters were pending */
  return 0;
}

bool FIELD_IS_INVERTED = false;
QApplication *a;

int startgame(bool usePositiveXSide, Simulator::TeamColor color) 
{
  std::cout<<"INSIDE START GAME"<<std::endl;
  // Set if the side used by us.
  FIELD_IS_INVERTED = usePositiveXSide;
  // Set the Team color and id so that the simulator and Strategy code are in coherence.
  switch(color) 
  {
    case Simulator::BLUE_TEAM:
      Strategy::HomeTeam::COLOR = Simulator::BLUE_TEAM;
      Strategy::AwayTeam::COLOR = Simulator::YELLOW_TEAM;
      Simulator::BlueTeam::ID   = Strategy::HOME_TEAM;
      Simulator::YellowTeam::ID = Strategy::AWAY_TEAM;
    break;
    case Simulator::YELLOW_TEAM:
      Strategy::HomeTeam::COLOR = Simulator::YELLOW_TEAM;
      Strategy::AwayTeam::COLOR = Simulator::BLUE_TEAM;
      Simulator::BlueTeam::ID   = Strategy::AWAY_TEAM;
      Simulator::YellowTeam::ID = Strategy::HOME_TEAM;
    break;
  }
  
  Strategy::BeliefState state;
  Util::CS writer_mutex;
  Util::CS writer_preference;
  Robot::writer_mutex      = &writer_mutex;
  Robot::writer_preference = &writer_preference;

  if(test) 
  {
    Tester t(run, state);
    if(qtdebug) 
    {
      t.start();
      QtDebugger qt_debug(a, &state);
      qt_debug.run();
    }
    else 
    {
      t.run();
    }
  }
  else 
  {
    Worker t(run, state, &writer_mutex, &writer_preference);
    if(qtdebug) 
    {
      t.start();
      QtDebugger qt_debug(a, &state);
      qt_debug.run();
    }
    else 
    {
      t.run();
    }
  }
  return 0;
}

int main(int argc, char* argv[])
{
  cout<<"Inside main"<<endl;
  if(qtdebug)
  {
    a = new QApplication(argc, argv);
  }
  
  return startgame(false, Simulator::BLUE_TEAM);
} // main
