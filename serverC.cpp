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
#include <cmath>

int main(){
	
	
		printf("The server C is up and running using UDP on port <23091>\n");
		int socket_serverC = socket(AF_INET, SOCK_DGRAM, 0);
		struct sockaddr_in serverC_address;
	    serverC_address.sin_family = AF_INET;
	    serverC_address.sin_addr.s_addr = inet_addr("127.0.0.1");
	    serverC_address.sin_port = htons(23091);

	    bind(socket_serverC,(struct sockaddr*) &serverC_address,sizeof(serverC_address));

	    struct sockaddr_in aws_address;
	    aws_address.sin_family = AF_INET;
	    aws_address.sin_addr.s_addr = inet_addr("127.0.0.1");
	    aws_address.sin_port = htons(24091);

		while(1){
		    double receieved_message;
		    double values_from_aws[50];
		    int i = 0;
		    while(1){
			    recvfrom(socket_serverC,&receieved_message,sizeof(receieved_message),0,(struct sockaddr*) &aws_address,(socklen_t*) sizeof(aws_address));
				values_from_aws[i] = receieved_message;
				if(values_from_aws[0] == 0){
					break;
				}
				i++;
				if(i == 7)
				{
					break;
				}
			}
			double link_id = values_from_aws[0];
			double size = values_from_aws[1];
			double power = values_from_aws[2];
			double bandwidth = values_from_aws[3];
		    double length = values_from_aws[4];
		    double velocity = values_from_aws[5];
		    double noise_power = values_from_aws[6];

		    printf("The serverC received link information of link<%.1f>, file size <%.2f>, and the signal power <%.2f>\n",link_id,size,power);

		    //calculation
		    double B = bandwidth*pow(10,6);
		    double S = (pow(10,(double)power/(double)10))/1000;
		    double N = (pow(10,(double)noise_power/(double)10))/1000;
		    double C = B*(log(1+S/N)/log(2));
		    double Tt = ((double)size/C)*pow(10,3);
		    double Tp = (length/(velocity*pow(10,4)))*pow(10,3);
		    double delay = Tt + Tp;

		    printf("The server C finished the calculation for link <%.1f>\n",link_id);
		    //send message to aws
		    int socket_aws = socket(AF_INET, SOCK_DGRAM, 0);
		    double send_message[] = {Tt,Tp,delay};
		    for(int i=0;i<3;i++){
			 	sendto(socket_aws,&send_message[i],sizeof(send_message[i]),0,(struct sockaddr*) &aws_address,sizeof(aws_address));
			 }
			 printf("The server C finished sending the output to AWS\n");
		 
	
		}
		return 0;
}

