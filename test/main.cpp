#include <stdio.h>
#include "serial.h"
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>
int main(int argc, char **argv)
{
	Serial s0, s1;
	if(!s0.Open("/dev/ttyUSB1", 38400))
	{
		perror("Could not Open Serial Port s0 :");
		exit(0);
	}
	if(!s1.Open("/dev/ttyUSB0", 38400))
	{
		perror("Could not Open Serial Port s1 :");
		exit(0);
	}
	struct timeval startTime, endTime;
	gettimeofday(&startTime, NULL);
	// now sending 1 byte
	s0.WriteByte('a');
	char c = s1.ReadByte();
	gettimeofday(&endTime, NULL);
	printf("value read = %c\n", c);
	long long int microseconds = (endTime.tv_sec - startTime.tv_sec) * 1000000 + ((int)endTime.tv_usec - (int)startTime.tv_usec);
	long long int milliseconds = microseconds / 1000;
	printf(">>>>>>>>>>>>>>Microseconds = %lld\n", microseconds);
	return 0;
}
