#include <stdio.h>
#include <sys/socket.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <sys/types.h>

#define PORT 8080
int main(int argc, char const *argv[])
{
    int server_fd, new_socket, vlread;
    struct sockaddr_in address;  
    int opt = 1, fdp;
    int addrlen = sizeof(address);
    char mybuffer[1025] = {0};
    char my_string[100];

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)  
    {
        perror("socket connection failed");
        exit(EXIT_FAILURE);
    }


    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR,
                                                  &opt, sizeof(opt))) 
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;   
    address.sin_addr.s_addr = INADDR_ANY;  
    address.sin_port = htons( PORT );    

    if (bind(server_fd, (struct sockaddr *)&address,
                                 sizeof(address))<0)
    {
        perror("bind connection failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 3) < 0) 
    {
        perror("listening");
        exit(EXIT_FAILURE);
    }

   
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address,
                       (socklen_t*)&addrlen))<0)
    {
        perror("accept");
        exit(EXIT_FAILURE);
    }
    char* sendfilename = (char*)calloc(1024, sizeof(char));   
    vlread = read(new_socket, sendfilename, 1024);    

    while(strcmp(sendfilename,"EXIT")!=0){ // if not an exit code
        
        int fdp=open(sendfilename,O_RDONLY);	

        if(fdp<0){            
            perror("An error was encountered");
            send(new_socket, "ERROR", 5, 0);
            vlread = read(new_socket, sendfilename, 1024);
            continue;
        }

        struct stat buf;
		fstat(fdp, &buf);
		long long int size = buf.st_size;	

        sprintf(my_string, "FILESIZE%lld", size);
        send(new_socket, my_string, strlen(my_string), 0);
        
        vlread = read(fdp, mybuffer, 1024);      
        mybuffer[vlread] = '\0';
        
        if(vlread<0)
        {
            close(fdp);
            perror("Read fail error");
            exit(1);
        }

        printf("Sending filename %s\n", sendfilename);
        while(1){                                    
            send(new_socket, mybuffer, strlen(mybuffer), 0);          
            vlread = read(fdp, mybuffer, 1024);                
            mybuffer[vlread] = '\0';
            if(vlread<0)
            {
                close(fdp);
                perror("Read failed");
                exit(1);
            }
            if(vlread==0) // if nothing is read break, file completed
                break;
        }

        printf("File sent\n");

        sendfilename = (char*)calloc(1024, sizeof(char));
        vlread = read(new_socket, sendfilename, 1024); // keep reading
        close(fdp);					
    }


    return 0;
}
