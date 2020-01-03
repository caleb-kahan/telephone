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

void createMemory();
void deleteMemory();
void readFile();

int main(int argc, char *argv[]){
  if(argc <2){
    printf("No Command Line Argument Given\n");
    return 0;
  }
  if(! strcmp(argv[1],"-c")){
    createMemory();
    return 0;
  }
  else if (! strcmp(argv[1],"-r")){
    deleteMemory();
    return 0;
  }
  else if (! strcmp(argv[1],"-v")){
    readFile();
    return 0;
  }
  else{
    printf("Not Proper Command Line Argument Given\n");
    return 0;
  }
}
void createMemory(){
  int shmd,semd,fd;
  semd = semget(KEY1, 1, IPC_CREAT | IPC_EXCL | 0644);
  if (semd == -1){
    printf("Everything is already created ?\n");
    return;
  }
  printf("semaphore created\n");
  union semun us;
  us.val = 1;
  semctl(semd, 0, SETVAL, us);
  shmd = shmget(KEY2, SEG_SIZE, IPC_CREAT | 0644);
  printf("shared memory created\n");
  fd = open("telephone",O_WRONLY | O_CREAT | O_TRUNC,0644);
  printf("file created\n");
  close(fd);
}
void deleteMemory(){
  int shmd,semd,fd;
  char *buffer;

  semd = semget(KEY1, 1, 0);
  if(semd == -1){
    printf("No Memory Exists");
    return;
  }
  struct sembuf sb;
  sb.sem_num = 0;
  sb.sem_op = -1;
  printf("trying to get in\n");
  semop(semd, &sb, 1);
  //Program waits until accessing semaphore
  //Read file
  readFile();
  //Removing shared memory
  shmd = shmget(KEY2, SEG_SIZE, 0);
  shmctl(shmd, IPC_RMID, 0);
  printf("shared memory removed\n");
  //Removing file
  remove("telephone");
  printf("file removed\n");
  //Removing semaphore
  semctl(semd, IPC_RMID, 0);
  printf("semaphore removed\n");
}
void readFile(){
  int fd;
  char * story;
  printf("The story so far:\n");
  fd = open("telephone",O_RDONLY);
  int storySize = lseek(fd, 0, SEEK_END);
  lseek(fd, 0, SEEK_SET);
  story = malloc(storySize+1);
  read(fd,story,storySize);
  story[storySize] = 0;
  printf("%s",story);
  close(fd);
}
