// Server side C/C++ program to demonstrate Socket programming 
#include <unistd.h> 
#include <stdio.h> 
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include <string.h> 
#define PORT 8080 
int main(int argc, char const *argv[]) 
{ 
    int server_fd, new_socket, valread; 
    struct sockaddr_in address; 
    int opt = 1; 
    int addrlen = sizeof(address); 
    char buffer[1024] = {0};
    char *hello = "Hello from server"; 
       
    // Code for exec-ed process

    if(argc > 1)
    {
	   
	if(setuid(65534) < 0) {           // Dropping privilege
            perror("Privilege dropping failed");
            exit(EXIT_FAILURE);
        }      
	//reading the socket id sent
	server_fd = atoi(argv[1]);

	// exec-ed process processing the data & sending the message
	    valread = read( server_fd , buffer, 1024); 
	    printf("%s\n",buffer ); 
	    send(server_fd , hello , strlen(hello) , 0 ); 
	    printf("Hello message sent\n"); 
    }

    else {

    // Entering in parent process 

    // Creating socket file descriptor 
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) 
    { 
        perror("socket failed"); 
        exit(EXIT_FAILURE); 
    } 
       
    // Forcefully attaching socket to the port 8080 
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, 
                                                  &opt, sizeof(opt))) 
    { 
        perror("setsockopt"); 
        exit(EXIT_FAILURE); 
    } 
    address.sin_family = AF_INET; 
    address.sin_addr.s_addr = INADDR_ANY; 
    address.sin_port = htons( PORT ); 
       
    // Forcefully attaching socket to the port 8080 
    if (bind(server_fd, (struct sockaddr *)&address,  
                                 sizeof(address))<0) 
    { 
        perror("bind failed"); 
        exit(EXIT_FAILURE); 
    } 
    
   if (listen(server_fd, 3) < 0) 
	    { 
		perror("listen"); 
		exit(EXIT_FAILURE); 
	    } 
  	if ((new_socket = accept(server_fd, (struct sockaddr *)&address,  
		               (socklen_t*)&addrlen))<0) 
	    { 
		perror("accept"); 
		exit(EXIT_FAILURE); 
	    } 


	
    /* The separation should start from here as data processing does not    need any privilege.
    */
 
    //Child process creation for privilege separation
    pid_t child = fork();

    if (child < 0) {                        //Error handling 
        perror("Child process creation failed");
        exit(EXIT_FAILURE);
    }
    else if (child == 0) {
	
      
	
	//converting socket descriptor to string
	char server_fd_string[11];
	sprintf(server_fd_string, "%d", new_socket);

	//Passing arguments to new exec-ed process (Image process)

	char *args[] = {"./server", server_fd_string, NULL};

	//execvp : Takes file as an argument
	if (execvp(args[0], args) < 0) 
	{
        	perror("execvp execution failed!");
        	exit(EXIT_FAILURE);
    	}
	    
    }
   
    else {  
	      
        int stat = 0;
        while ((wait(&stat)) > 0);   //Parent Waiting for child exit
    }
}     
    return 0; 
} 
