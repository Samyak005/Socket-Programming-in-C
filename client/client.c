#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#define PORT 8080
#define token_buffersize 64
#define delimiters " \n\t"

void getFile(int sockconnection, char* getfilename)
{
    int count = 0;
    long long int pert=0;
    char my_progress[100];

    char* mybuffer = (char*)malloc(sizeof(char)*1025);

    long long int file_size;

    printf("Request for %s...\n", getfilename);
    send(sockconnection, getfilename, strlen(getfilename), 0); 
    
    int vlread = read( sockconnection , mybuffer, 1024); 
    mybuffer[vlread] = '\0';

    if(vlread<0){
        free(mybuffer);
        printf("%s Read failed\n", getfilename);
        perror("Read failed");
        exit(1);  
    }

    if(strcmp(mybuffer,"ERROR")==0){
        printf("%s wrong file requested, make another request for file\n", getfilename); 
        free(mybuffer);
        return;
    }

    //printf("mybuffer : %s\n", mybuffer);

    if(strncmp(mybuffer, "FILESIZE", 8)==0){
        char* token = strtok(mybuffer, "FILESIZE");
        //token= strtok(token, delimiters);
        file_size = atoll(token);
        printf("file size %lld\n", file_size);
    }

    vlread = read(sockconnection , mybuffer, 1024); 

    int fd = open(getfilename, O_WRONLY|O_CREAT|O_TRUNC, S_IRUSR|S_IWUSR|S_IXUSR); 
    while(1){

        count++;

        write(fd,mybuffer,vlread); 

        pert = count*1024;									// % calculation
		sprintf(my_progress, "Total File Size %lld Total Written %lld\r", file_size, pert);
        int writing_pert = write(1 , &my_progress, strlen(my_progress)); // writing to stdout(1)

        if ((vlread<1024) || (pert==file_size)){
            sprintf(my_progress, "Completed - Total File Size %lld Total Written %lld\r", file_size, file_size);
            int writing_pert = write(1 , &my_progress, strlen(my_progress)); // writing to stdout(1)
            fflush(stdout);
            break;
        }
        vlread = read( sockconnection , mybuffer, 1024);   

        if(vlread<0){
            free(mybuffer);
            close(fd);
            perror("Read failed");
            printf("Read error encountered in getFilename \n");
            exit(1);
        }
        mybuffer[vlread] = '\0';
    }
    close(fd);   
    free(mybuffer);
    printf("\n%s File download completed\n", getfilename);
    fflush(stdout);
    return;
}

char** split_line(char* input)
{
	int i=0,ch;

	int buffer_size = token_buffersize;
	
	char** buffer = (char**)malloc(buffer_size * sizeof(char*));
	
	if(!buffer)
	{
		fprintf(stderr, "Error: could not allocate memory\n");
		exit(0);
	}
	
	char* token = strtok(input, delimiters);
    while(token)
    {
        buffer[i] = token;
        //printf("%s\n", buffer[i]);
        i++;

        if(i >= buffer_size)
        {
            buffer_size += token_buffersize;
            buffer = realloc(buffer,buffer_size * sizeof(char*));
            if(!buffer)
            {
                fprintf(stderr, "Error: could not allocate memory\n");
                exit(0);
            }	
        }
        token = strtok(NULL,delimiters);	
    }
    
	buffer[i] = NULL;
	return buffer;
}

int main(int argc, char const *argv[])
{
    struct sockaddr_in address;
    char *msg;
    char mybuffer[1024] = {0};
    int sockconnection = 0;
    struct sockaddr_in serv_addrs;
    if ((sockconnection = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
    {
        printf("\n Socket creation failed error \n");
        return -1;
    }

    memset(&serv_addrs, '0', sizeof(serv_addrs)); 

    serv_addrs.sin_family = AF_INET;
    serv_addrs.sin_port = htons(PORT);

    if(inet_pton(AF_INET, "127.0.0.1", &serv_addrs.sin_addr)<=0)
    {
        printf("\n Please provide a valid address/ Address not supported \n");
        return -1;
    }

    if (connect(sockconnection, (struct sockaddr *)&serv_addrs, sizeof(serv_addrs)) < 0) 
    {
        printf("\nConnection Fail Error\n");
        return -1;
    }
    
    while(1){
    	printf(">>> ");

	    ssize_t mybuffer1 = 0;
	    msg = NULL;
	    if(getline(&msg,&mybuffer1,stdin)==-1){
		    printf("Cannot Read pls enter again.\n");
		}

        char ** args= split_line(msg);

        int i = 1;
    	if(strncmp(args[0],"get",3)==0 ){ 
            while (args[i]!=NULL)
            {
                getFile(sockconnection, args[i]); 
                i++;
            }
    	}

        if(strncmp(args[0],"exit",4)==0 ){ 
            send(sockconnection, "EXIT", 4, 0);   
            exit(1);    
    	}
    }
    return 0;
}
