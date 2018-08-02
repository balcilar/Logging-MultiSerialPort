#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <termios.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#define BAUDRATE0 B230400
#define BAUDRATE1 B115200
char *portname0 = "/dev/ttySAC0";
char *portname1 = "/dev/ttySAC2";


void construct_filename(char *filename_prefix, char *filename)
{
    filename[0] = 0;
    char date_stamp[9] = "00000000";
    char time_stamp[7] = "000000";
    char filename_suffix[5] = ".bin";

    time_t ltime;
    struct tm result;

    ltime = time(NULL);
    localtime_r(&ltime, &result);

    if(result.tm_hour > 9) {
        snprintf(time_stamp + 0, 3, "%d", result.tm_hour);
    } else {
        snprintf(time_stamp + 1, 2, "%d", result.tm_hour);
    }

    if(result.tm_min > 9) {
        snprintf(time_stamp + 2, 3, "%d", result.tm_min);
    } else {
        time_stamp[2] = 0x30;
        snprintf(time_stamp + 3, 2, "%d", result.tm_min);
    }

    if(result.tm_sec > 9) {
        snprintf(time_stamp + 4, 3, "%d", result.tm_sec);
    } else {
        time_stamp[4] = 0x30;
        snprintf(time_stamp + 5, 2, "%d", result.tm_sec);
    }

    if(result.tm_mon + 1 > 9) {
        snprintf(date_stamp + 0, 3, "%d", result.tm_mon + 1);
    } else {
        snprintf(date_stamp + 1, 2, "%d", result.tm_mon + 1);
    }

    if(result.tm_mday > 9) {
        snprintf(date_stamp + 2, 3, "%d", result.tm_mday);
    } else {
        date_stamp[2] = 0x30;
        snprintf(date_stamp + 3, 2, "%d", result.tm_mday);
    }

    snprintf(date_stamp + 4, 5, "%d", result.tm_year + 1900);


    strcat(filename, filename_prefix);
    strcat(filename, date_stamp);
    strcat(filename, "-");
    strcat(filename, time_stamp);
    strcat(filename, filename_suffix);
}

void  *thread0(void *)
{
        char buf[8192];
        int fd, fd1, n;
        char filename_prefix[5] = "IMU-";
        char filename[26];
        chdir("/home/lidar/data");

        construct_filename(filename_prefix,filename);

        fd1 = open(filename, O_CREAT | O_WRONLY, S_IRUSR | S_IWUSR);
        fd = open(portname0, O_RDONLY | O_NOCTTY | O_NDELAY);

        printf("Saving IMU data to file: ");

        printf("%s \n",filename);
        putchar('\n');

        struct termios toptions;

        memset(&toptions, 0, sizeof(toptions));

        toptions.c_cflag = CS8 | CLOCAL | CREAD;
        toptions.c_iflag = IGNPAR;
        toptions.c_oflag = 0;
        toptions.c_lflag = 0;
        toptions.c_cc[VMIN] = 0;
        toptions.c_cc[VTIME] = 0;
            toptions.c_cflag &= ~CRTSCTS;
            toptions.c_iflag &= ~(IXON | IXOFF | IXANY);

        cfsetispeed(&toptions, BAUDRATE0);
        cfsetospeed(&toptions, BAUDRATE0);

        tcsetattr(fd, TCSANOW, &toptions);

        tcflush(fd, TCIFLUSH);

        while(1)
        {
            n = read(fd, buf, 8192);

            if(n == -1) {
                continue;
            }
            write(fd1, buf, n);
        }

        close(fd);
        close(fd1);
}

void  *thread1(void *)
{

        char buf[8192];
        int fd, fd1, n;
        char filename_prefix[8] = "GPSRAW-";
        char filename[26];

        chdir("/home/lidar/data");
        construct_filename(filename_prefix,filename);



        fd1 = open(filename, O_CREAT | O_WRONLY, S_IRUSR | S_IWUSR);
        fd = open(portname1, O_RDONLY | O_NOCTTY | O_NDELAY);

        printf("Saving GPSRAW data to file: ");
        printf("%s \n",filename);
        putchar('\n');

        struct termios toptions;

        memset(&toptions, 0, sizeof(toptions));

        toptions.c_cflag = CS8 | CLOCAL | CREAD;
        toptions.c_iflag = IGNPAR;
        toptions.c_oflag = 0;
        toptions.c_lflag = 0;
        toptions.c_cc[VMIN] = 0;
        toptions.c_cc[VTIME] = 0;
        toptions.c_cflag &= ~CRTSCTS;
        toptions.c_iflag &= ~(IXON | IXOFF | IXANY);

        cfsetispeed(&toptions, BAUDRATE1);
        cfsetospeed(&toptions, BAUDRATE1);

        tcsetattr(fd, TCSANOW, &toptions);

        tcflush(fd, TCIFLUSH);


        while(1)
        {

            n = read(fd, buf, 8192);
            if(n == -1) {
                continue;
            }
            write(fd1, buf, n);
        }


        close(fd);
        close(fd1);
}




int main(int argc, char *argv[])
{
        int status,delay;
        pthread_t tid0,tid1;
        status=0;

        if (argc<2)
            delay=0;
        else
            delay=atoi(argv[1]);

        sleep(delay);

        pthread_create(&tid0,NULL,thread0,NULL);
        pthread_create(&tid1,NULL,thread1,NULL);
        pthread_join(tid0,NULL);
        pthread_join(tid1,NULL);
        return 0;
}
