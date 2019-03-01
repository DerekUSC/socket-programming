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
int main() {
	
		
		int socket_monitor = socket(AF_INET, SOCK_STREAM, 0);

	    printf("The monitor is up and running.\n");
	     //create the socket in monitor side
	    struct sockaddr_in server_address;
	    server_address.sin_family = AF_INET;
	    server_address.sin_addr.s_addr = inet_addr("127.0.0.1");
	    server_address.sin_port = htons(26091);


	   	 int connection_state = connect(socket_monitor, (struct sockaddr*)&server_address, sizeof(server_address));
	   	 if(connection_state == -1){
	    	perror("connect ");
		}
		
	   

	    while(1){

	    double receieved_message[3];
	    recv(socket_monitor,&receieved_message,sizeof(receieved_message),0);
	    double link_id = receieved_message[0];
	    double size = receieved_message[1];
	    double power = receieved_message[2];
	    printf("The monitor receieved ID=<%.1f>, size=<%.2f>, and power=<%.2f> from the AWS\n",link_id,size,power);


	    double receieved_message_T[4];
        recv(socket_monitor,&receieved_message_T,sizeof(receieved_message_T),0);

        double m = receieved_message_T[0];
        if(m == 1){
            double Tt = receieved_message_T[1];
            double Tp = receieved_message_T[2];
            double delay = receieved_message_T[3];
            printf("The result for link <%.1f>:\n Tt = <%.2f>ms, \n Tp = <%.2f>ms, \n Delay = <%.2f>ms\n",link_id,Tt,Tp,delay);
        }
        else{
         printf("Found no matches for link <%.1f>\n", link_id);
        }

	    // close(socket_monitor);
	    }
  
  return(0);
}
