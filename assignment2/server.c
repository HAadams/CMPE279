// Server side C/C++ program to demonstrate Socket programming

#include <arpa/inet.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>
#include <pwd.h>

#define PORT 8080
int main(int argc, char const *argv[])
{
    int argc0 = argc;
    char **argv0 = argv;

    if(argc == 1)
        main_app(argc, argv);
    else if(argc == 3 && argv[1] == "-P" && argv[2] == "client_listener")
        printf("%s", "CHILD RUNNING");
        //client_listener(0);

    return 0;
}

void main_app(int argc, char const *argv[]) {

    int server_fd, new_socket, valread;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[102] = {0};
    char *hello = "Hello from server";
    int pipe_child[2];
    // Show ASLR
    printf("execve=0x%p\n", execve);

    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Attaching socket to port 80
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
                                                  &opt, sizeof(opt)))
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons( PORT );

    // Forcefully attaching socket to the port 80
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

    // create a child process to handle communications with client
    pid_t pid = fork();
    int status;
    // child process
    if(pid == 0) {
        
        client_listener(new_socket);
        char **child_args[3];
        child_args[0] = argv[0];
        child_args[1] = "-P";
        child_args[2] = "client_listener";

        execv(argv[0], child_args);

    // failed to create child process
    } else if(pid < 0) {
       fatal("Error creating child process!");
       return 0;

    // parent should wait for child
    } else {
        int ret_code = waitpid(pid, &status, 0);
    }


}
void client_listener(int new_socket) {

    int valread;
    char *hello = "Hello from server";
    char buffer[102] = {0};

    setuid(getpwnam("nobody")->pw_uid);

    valread = read( new_socket , buffer, 1024);
    printf("%s\n",buffer );
    send(new_socket , hello , strlen(hello) , 0 );
    printf("Hello message sent\n");

}
