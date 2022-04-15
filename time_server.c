#include<stdio.h>
#include<string.h>    // for strlen
#include<sys/socket.h>
#include<arpa/inet.h> // for inet_addr
#include<unistd.h>    // for write
#include<time.h> 

int main(int argc, char *argv[])
{
    int socket_desc , new_socket , c;
    struct sockaddr_in server , client;
    char *message;
    char error_message[20] = "INCORRECT REQUEST\n";
    char requests[20] = "";
    char function[] = "GET_DATE";
    char server_reply[2000] = "";
     
    // Create socket
    socket_desc = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_desc == -1)
    {
        puts("Could not create socket");
        return 1;
    }
     
    server.sin_family = AF_INET;  //IPv4 Internet protocols
    server.sin_addr.s_addr = INADDR_ANY; //IPv4 local host address
    server.sin_port = htons(8889); // server will listen to 8888 port
     
    // Bind
    if(bind(socket_desc, (struct sockaddr *)&server, sizeof(server)) < 0)
    {
        puts("Binding failed");
        return 1;
    }
    puts("Socket is binded");
     
    // Listen
    listen(socket_desc, 3);
     
    // Accept and incoming connection
    puts("Waiting for incoming connections...");
    
    c = sizeof(struct sockaddr_in);
    new_socket= accept(socket_desc,(struct sockaddr *)&client,(socklen_t*)&c);
    if (new_socket<0)
    {
        puts("Accept failed");
        return 1;
    }
     
    puts("Connection accepted");

    if( recv(new_socket, server_reply , 2000 , 0) < 0) // Take input
    {
        puts("recv failed");
    }
    else
    {
        if(strcmp(server_reply, "") == 13) // Check if the input is null
        {
            write(new_socket, error_message, strlen(error_message));
            close(socket_desc);
            close(new_socket); 
            return 0;
        }

        if (strcmp(server_reply, function) == 13) // Check if the input is only GET_DATE
        { 
            write(new_socket, error_message, strlen(error_message));
            close(socket_desc);
            close(new_socket); 
            return 0;
        }

        char getFunction[9];  // Take first part
        memcpy(getFunction,&server_reply[0],8);
        getFunction[8] = '\0';

        char requests[100]; // Take second part
        memcpy(requests,&server_reply[9], strlen(server_reply) - strlen(getFunction) - 3);
        requests[99] = '\0';

        if (strcmp(function, getFunction) == 0) // If the firs part is equals GET_DATE
        {
            int flag = 0;
            char currentStr[10];
            char percent_sign[5] = "%";
            for (int i = 0; i < strlen(requests); i++) // Loop for checking % character. If does not exist, wrong input
            {
                memcpy(currentStr,&requests[i],1);
                currentStr[9] = '\0';

                if (strcmp(currentStr, percent_sign) == 0) // if the character does equal %
                {
                    flag = 1; // true
                    break;
                }
            }
            if (flag != 1)
            {
                write(new_socket, error_message, strlen(error_message));
                close(socket_desc);
                close(new_socket); 
                return 0;
            }
            else // Get response of server
            {                
                time_t rawtime;
                struct tm *info;
                char buffer[80];
                time(&rawtime);
                info = localtime(&rawtime);

                strftime(buffer, 80, requests, info);

                int flag = 0;
                char currentStr[10];
                char percent_sign[5] = "%";
                for (int i = 0; i < strlen(buffer); i++) // Loop for checking % character. If exists, wrong input
                {
                    memcpy(currentStr,&buffer[i],1);
                    currentStr[9] = '\0';
                    if (strcmp(currentStr, percent_sign) == 0) // if the character does equal %
                    {
                        flag = 1; 
                        break;
                    }
                }
                
                if (flag == 1) // if it is wrong input
                {
                    write(new_socket, error_message, strlen(error_message));
                    close(socket_desc);
                    close(new_socket); 
                    return 0;
                }
                write(new_socket, buffer, strlen(buffer));
                printf("Formatted date & time : |%s|\n", buffer);
            }
        } 
        else
        {
            write(new_socket, error_message, strlen(error_message));
        }
    }

    // Reply to the client
    message = "\n\nConnection terminated.\n\n";
    write(new_socket, message, strlen(message));

    close(socket_desc);
    close(new_socket); 

    return 0;
}

