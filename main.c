#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#define KEY1 24601
#define KEY2 24602
#define SEG_SIZE 200

union semun {
  int              val;    /* Value for SETVAL */
  struct semid_ds *buf;    /* Buffer for IPC_STAT, IPC_SET */
  unsigned short  *array;  /* Array for GETALL, SETALL */
  struct seminfo  *__buf;  /* Buffer for IPC_INFO
                              (Linux-specific) */
};

int main(){
  int shmd,semd,fd,bytes;
  char lastAddition [SEG_SIZE];
  char buffer [SEG_SIZE];
  int *data;

  semd = semget(KEY1, 1, 0);
  if(semd == -1){
    printf("No Memory Exists");
    return -1;
  }
  struct sembuf sb;
  sb.sem_num = 0;
  sb.sem_op = -1;
  printf("trying to get in\n");
  semop(semd, &sb, 1);
  //Can now start aaccesing file info
  shmd = shmget(KEY2, SEG_SIZE, 0);
  data = shmat(shmd, 0, 0);
  bytes = data[0];
  //Can now start reading file
  fd = open("telephone",O_RDWR);
  lseek(fd, -1*bytes, SEEK_END);
  read(fd,lastAddition,bytes-1);
  //Not reading everything
  lseek(fd, 0, SEEK_END);
  printf("Last Addition: %s\n",lastAddition);
  //Can now start taking input
  printf("Your Addition: ");
  fgets(buffer, SEG_SIZE, stdin);
  data[0] = strlen(buffer);
  write(fd,buffer,data[0]);
  //Closing files
  close(fd);
  //Removing Variable
  shmdt(data);
  //Resetting Semaphores
  sb.sem_op = 1;
  semop(semd, &sb, 1);
}
