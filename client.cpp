#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/wait.h>


int main(int argc,char* argv[]){
    
    int socket_client = socket(AF_INET, SOCK_STREAM, 0); //create the socket in client side

    if(socket_client == -1)
    {
        perror("create socket: ");
    }

    printf("The client is up and running.\n");
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = inet_addr("127.0.0.1");
    server_address.sin_port = htons(25091);
    //create variables corresponding to link id, size and power
    int connection_state = connect(socket_client, (struct sockaddr*)&server_address, sizeof(server_address));
       if(connection_state == -1){
            perror("connect ");
        }
        // if(connection_state == 0){
        //     printf("connection succeed\n");
        // }
    double link_id = atof(argv[1]);
    double size = atof(argv[2]);
    double power = atof(argv[3]);
  
    //send message to server
    
    double send_message[] = {link_id,size,power};
    for(int i=0;i<3;i++){
    send(socket_client, &send_message[i], sizeof(send_message), 0);
	}
    printf("The client sent ID=<%.1f>, size=<%.2f>, and power=<%.2f> to AWS\n",link_id,size,power);

    //receive message from server
    double receieved_message;
    recv(socket_client,&receieved_message,sizeof(receieved_message),0);

    double delay = receieved_message;
    if (delay == 0){
    	printf("Found no matches for link<%.1f>\n",link_id);
    }
    else
    {
        double delay = receieved_message;
        printf("The delay for link <%.1f> is <%.2f> ms\n",link_id,delay);
    }
	close(socket_client);

    return 0;
}