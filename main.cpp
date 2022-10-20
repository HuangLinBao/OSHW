#include <iostream>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>
#include <math.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <errno.h>
#include <cstring>
#include <fcntl.h>
#include <fstream>

using namespace std;

int main(int argc, char **argv)
{
    int fd;
    int *sharedMemory;
    int i;

    if (argc != 1)
    {
        shm_unlink("/bolts");
        return EXIT_SUCCESS;
    }

    fd = shm_open("/bolts", O_RDWR | O_CREAT, 0777);
    if (fd == -1)
    {
        fprintf(stderr, "Open failed:%s\n",
                strerror(errno));
        return EXIT_FAILURE;
    }

    if (ftruncate(fd, sizeof(*sharedMemory)) == -1)
    {
        fprintf(stderr, "ftruncate: %s\n",
                strerror(errno));
        return EXIT_FAILURE;
    }

    sharedMemory = (int *)(mmap(0, sizeof *sharedMemory, PROT_READ | PROT_WRITE,
                                MAP_SHARED | MAP_ANONYMOUS, -1, 0));
    if (sharedMemory == MAP_FAILED)
    {
        fprintf(stderr, "mmap failed:%s\n", strerror(errno));
        return EXIT_FAILURE;
    }

    i = 0;
    *sharedMemory = 0;
    int n = 924; // 924
    struct timespec tim, tim2;
    // int PID;
    int time;

    while (i < n)
    {

        if (fork() == 0)
        {
            time = getpid() % 10;

            for (int i = 0; i < n; i++)
            {
                cout << "(Child): process #" << getpid() << " about to increment the counter, old value  was: " << *sharedMemory << endl;

                *sharedMemory = *sharedMemory + 1;

                cout << "(Child): process #" << getpid() << " is done incrementing this counter , new value  is: " << *sharedMemory << " bruv" << endl;

                cout << "(Child # " << getpid() << "): about to go sleep for like " << time << " nanoseconds you get me bruv" << endl;
                tim.tv_sec = 0;
                tim.tv_nsec = time;

                if (nanosleep(&tim, &tim2) < 0)
                {
                    printf("something went wrong \n");
                    return 0;
                }

                else
                {
                    cout << "(Child # " << getpid() << "): Done... all went smooth" << endl;
                }
            }
            close(fd);
            shm_unlink("/bolts");

            exit(0);
        }

        i++;
    }

    for (int i = 0; i < n; i++)
    {

        cout << "(Parent): waiting.........." << endl;
        wait(0);
    }

    cout << "(Parent): " << *sharedMemory << " is the final value of the counter, expected value is " << pow(n, 2) << endl;
    return 0;
}
