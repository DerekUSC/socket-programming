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
	
		printf("The server B is up and running using UDP on port <22091>\n");
		int socket_serverB = socket(AF_INET, SOCK_DGRAM, 0);

		struct sockaddr_in serverB_address;
	    serverB_address.sin_family = AF_INET;
	    serverB_address.sin_addr.s_addr = inet_addr("127.0.0.1");
	    serverB_address.sin_port = htons(22091);

	    bind(socket_serverB,(struct sockaddr*) &serverB_address,sizeof(serverB_address));

	    //construct the address of socket AWS
	    struct sockaddr_in aws_address;
	    aws_address.sin_family = AF_INET;
	    aws_address.sin_addr.s_addr = inet_addr("127.0.0.1");
	    aws_address.sin_port = htons(24091);
	    
	    while(1){
	    //receieve the link_id from aws
		    double receieved_message;
		    double link_id;
		    double bandwidth,length,velocity,noise_power = 0;
		    int m_from_A;
		    
			recvfrom(socket_serverB,&receieved_message,sizeof(receieved_message),0,(struct sockaddr*) &aws_address,(socklen_t*) sizeof(aws_address));
		    recvfrom(socket_serverB,&m_from_A,sizeof(m_from_A),0,(struct sockaddr*) &aws_address,(socklen_t*) sizeof(aws_address));
		    // printf("m from A is: %d\n",m_from_A);
			link_id =  receieved_message; 
			printf("The server B receieved input <%.1f>\n",link_id);
			int m = 0;
			//search for certain values in database corresponding to link i
			if(m_from_A == 0){
				FILE *databaseB = fopen("database_b.csv","r");
				int line = 0;
				char data[512];
				
				
				while(fgets(data,512,databaseB)!=NULL){
					char dlim = ',';
					char *a = strtok(data,&dlim);
					double databaseB_id = atof(a);
					if(databaseB_id == link_id){
						bandwidth = atof(strtok(NULL,","));
						length = atof(strtok(NULL,","));
						velocity = atof(strtok(NULL,","));
						noise_power = atof(strtok(NULL,","));
						m = 1;
						break;
					}
				}
				fclose(databaseB);
			}

			if(m_from_A == 1){
				m = 0;
			}
			//send message to aws
			printf("The Server B has found <%d> match\n",m);
			double send_message[] = {m,bandwidth,length,velocity,noise_power};
			int socket_aws = socket(AF_INET, SOCK_DGRAM, 0);
			//send link information to aws
			for(int i=0;i<5;i++){
				 sendto(socket_aws,&send_message[i],sizeof(send_message[i]),0,(struct sockaddr*) &aws_address,sizeof(aws_address));
			}
				printf("The server B finished sending the output to AWS\n");
    		
		}
	return 0;
}