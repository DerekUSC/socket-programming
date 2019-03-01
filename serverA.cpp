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

int main(){
	

		printf("The server A is up and running using UDP on port <21091>\n");
		int socket_serverA = socket(AF_INET, SOCK_DGRAM, 0);

		struct sockaddr_in serverA_address;
	    serverA_address.sin_family = AF_INET;
	    serverA_address.sin_addr.s_addr = inet_addr("127.0.0.1");
	    serverA_address.sin_port = htons(21091);

	    bind(socket_serverA,(struct sockaddr*) &serverA_address,sizeof(serverA_address));

	    //construct the address of socket AWS
	    struct sockaddr_in aws_address;
	    aws_address.sin_family = AF_INET;
	    aws_address.sin_addr.s_addr = inet_addr("127.0.0.1");
	    aws_address.sin_port = htons(24091);
	    
	    //receieve the link_id from aws
	    while(1) {
	    	
	    double receieved_message;
	    double link_id;

	    
	    recvfrom(socket_serverA,&receieved_message,sizeof(receieved_message),0,(struct sockaddr*) &aws_address,(socklen_t*) sizeof(aws_address));
		link_id = receieved_message; 
		printf("The server A receieved input <%.1f>\n",link_id);

		//search for certain values in database corresponding to link i
		FILE *databaseA = fopen("database_a.csv","r");
		int line = 0;
		char data[512];
		double bandwidth,length,velocity,noise_power;
		int m = 0;
		while(fgets(data,512,databaseA)!=NULL){
			char dlim = ',';
			char *a = strtok(data,&dlim);
			double databaseA_id = atof(a);
			if(databaseA_id == link_id){
				bandwidth = atof(strtok(NULL,","));
				length = atof(strtok(NULL,","));
				velocity = atof(strtok(NULL,","));
				noise_power = atof(strtok(NULL,","));
				m = 1;
				break;
			}
		}
		fclose(databaseA);

		//send message to aws
		printf("The Server A has found <%d> match\n",m);
		double send_message[] = {m,bandwidth,length,velocity,noise_power};
		int socket_aws = socket(AF_INET, SOCK_DGRAM, 0);
		
		for(int i=0;i<5;i++){
		 	sendto(socket_aws,&send_message[i],sizeof(send_message[i]),0,(struct sockaddr*) &aws_address,sizeof(aws_address));
		 }
		printf("The server A finished sending the output to AWS\n");

	 
	}
	return 0;
}