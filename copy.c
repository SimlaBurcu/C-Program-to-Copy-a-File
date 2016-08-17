#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#define BUF_SIZE 8192 //default buffer is optimal which is 8192
#define PERMS 0644 //permissions for creating a file

static void mymkdir(const char *dir);
static void cutstring(char *path);
void alarm_handler(int signum);

int main(int argc, char* argv[]) {
	int input_fd, output_fd; /* Input and output file descriptors */
	ssize_t bytes_read, bytes_write; /* Number of bytes returned by read() and write() */
	char buffer[BUF_SIZE]; /* Character buffer for read() and write() */
	char* path=malloc(sizeof(argv[2])); /* mymkdir fonksiyonunda, arguman olarak verilen pathte son '/'tan sonraki dosya, uzerine yazilacak olan text dosyasi oldugundan onun mkdir() ile degil create() ile olusturulmasi gerekiyor. path, argv[2]'den son '/'tan sonrasini cutstring fonksiyonu ile ayirdigimiz icin argv[2]'nin ilk halini koruyan degiskendir.*/
	strcpy(path,argv[2]);
 
	/* Yanlıs sekilde cagrildiysa */
	if(argc != 3){
		printf ("Kullanim: copy kaynak_dosya hedef_dosya\n");
		exit(EXIT_FAILURE);
	}
 	/* Open file to be read */
	input_fd = open (argv [1], O_RDONLY);
	if (input_fd == -1) {
		printf( "Error Value is : %s\n", strerror(errno) );
		exit(EXIT_FAILURE);
	}
 
	/* Open file to be written. Check if the given path exists, or if it has proper permissions, or if it can be openned else created.
If it has errors print to the console and exit. */
	if((access(argv[2],F_OK))==0){
		if((access(argv[2],W_OK))==0){
			output_fd = open(argv[2],O_WRONLY);
			if(output_fd == -1){
				printf( "Error Value is : %s\n", strerror(errno) );
			}
			
		}else{
			printf( "Error Value is : %s\n", strerror(errno) );
			exit(EXIT_FAILURE);
	}
	}else{
		output_fd =creat(argv[2], PERMS);
		if(output_fd == -1){
			cutstring(argv[2]);
			mymkdir(argv[2]);
			output_fd =creat(path, PERMS);
			if(output_fd ==-1){
				printf( "Error Value is : %s\n", strerror(errno) );
				exit(EXIT_FAILURE);
			}
		}
		
	}

	/* "Kopyalaniyor..." bildirimi icin sayac. */
	int sayac=1; 

	/* Set up alarm handler */
	signal(SIGALRM, alarm_handler);

	/* Schedule the first alarm */
	alarm(1);

	while(1){
		pause();
		if((bytes_read = read (input_fd, &buffer, BUF_SIZE)) <= 0)
			break;
		bytes_write = write (output_fd, &buffer, (ssize_t) bytes_read);
		if(bytes_write != bytes_read){
			printf( "Error Value is : %s\n", strerror(errno) );
			exit(EXIT_FAILURE);
		}
		if(sayac>2)
			printf("Kopyalaniyor...\n");
		sayac++;
	}

	/* Close file descriptors */
	close (input_fd);
	close (output_fd);

	printf("Toplam %zu byte kopyalandi.\n",bytes_write);
	exit(EXIT_SUCCESS);
}

/* Sets a new alarm for 1 second */
void alarm_handler(int signum){
	alarm(1);
}

/*Yukarida da aciklandigi gibi arguman olarak verilen path'teki dosyalar eger mevcut degilse onlari yaratmaya yarayan fonksiyon.*/
static void mymkdir(const char *dir) {
        char temp[256];
        char *ptr = NULL;
        size_t len;

        snprintf(temp, sizeof(temp),"%s",dir);
        len = strlen(temp);
        if(temp[len - 1] == '/')
                temp[len - 1] = 0;
        for(ptr = temp + 1; *ptr; ptr++)
                if(*ptr == '/') {
                        *ptr = 0;
                        mkdir(temp, S_IRWXU);
                        *ptr = '/';
                }
        mkdir(temp, S_IRWXU);
}


/* argv[2]'deki son '/'dan sonrasini atan fonksiyon. Nedeni yukarida anlatilmistir.*/
static void cutstring(char *path) {
	int i;
	for (i =  strlen(path); i>0; i--){
		if(path[i] =='/'){
			//file=path+i+1;
			path[i]='\0';
			break;
		}
	}
}
