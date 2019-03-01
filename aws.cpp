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
        
 
        double bandwidth,length,velocity,noise_power;
    	int socket_aws_server = socket(AF_INET, SOCK_DGRAM, 0);

        printf("the AWS is up and running\n");

    	//the construct of sockets with different port numbers
    	struct sockaddr_in aws_client_address;
        aws_client_address.sin_family = AF_INET;
        aws_client_address.sin_addr.s_addr = inet_addr("127.0.0.1");
        aws_client_address.sin_port = htons(25091);

        struct sockaddr_in aws_monitor_address;
        aws_monitor_address.sin_family = AF_INET;
        aws_monitor_address.sin_addr.s_addr = inet_addr("127.0.0.1");
        aws_monitor_address.sin_port = htons(26091);

        struct sockaddr_in aws_server_address;
        aws_server_address.sin_family = AF_INET;
        aws_server_address.sin_addr.s_addr = inet_addr("127.0.0.1");
        aws_server_address.sin_port = htons(24091);

        bind(socket_aws_server,(struct sockaddr*) &aws_server_address,sizeof(aws_server_address));

        //the child socket for monitor
        int socket_aws_monitor = socket(AF_INET, SOCK_STREAM, 0);
        bind(socket_aws_monitor,(struct sockaddr*) &aws_monitor_address,sizeof(aws_monitor_address));
        listen(socket_aws_monitor,5);
        int socket_monitor = accept(socket_aws_monitor,NULL,NULL); 

        //the child socket for client
        int socket_aws_client = socket(AF_INET, SOCK_STREAM, 0);
        bind(socket_aws_client,(struct sockaddr*) &aws_client_address,sizeof(aws_client_address));
        
        
        

        while(1){
            

            listen(socket_aws_client,5);
        
            int socket_client = accept(socket_aws_client,NULL,NULL);    
            double receieved_message_client[5];
            recv(socket_client,&receieved_message_client,sizeof(receieved_message_client),0);
            double link_id = receieved_message_client[0];
            double size = receieved_message_client[1];
            double power = receieved_message_client[2];
            printf("The AWS receieved link ID=<%.1f>, size=<%.2f>, and power=<%.2f> from the client using TCP over port <25091>\n",link_id,size,power);

      	 //send message of link id,size,power to monitor


            double send_message_monitor[]={link_id,size,power};
          	for(int i=0;i<3;i++){
            send(socket_monitor,&send_message_monitor[i],sizeof(send_message_monitor[i]),0);
        	}


            printf("The AWS sent link ID=<%.1f>, size=<%.2f>, and power=<%.2f> to the monitor using TCP over port <26091>\n",link_id,size,power);
    	   
           //send message to server A and B

        	struct sockaddr_in serverA_address;
            serverA_address.sin_family = AF_INET;
            serverA_address.sin_addr.s_addr = inet_addr("127.0.0.1");
            serverA_address.sin_port = htons(21091);

            struct sockaddr_in serverB_address;
            serverB_address.sin_family = AF_INET;
            serverB_address.sin_addr.s_addr = inet_addr("127.0.0.1");
            serverB_address.sin_port = htons(22091);

            struct sockaddr_in serverC_address;
            serverC_address.sin_family = AF_INET;
            serverC_address.sin_addr.s_addr = inet_addr("127.0.0.1");
            serverC_address.sin_port = htons(23091);

        	double send_message_server = link_id;
        	int socket_server = socket(AF_INET, SOCK_DGRAM, 0);

        	sendto(socket_server,&send_message_server,sizeof(send_message_server),0,(struct sockaddr*) &serverA_address,sizeof(serverA_address));
            sendto(socket_server,&send_message_server,sizeof(send_message_server),0,(struct sockaddr*) &serverB_address,sizeof(serverB_address));
        	printf("The AWS sent link ID=<%.1f> to Backend-Server A using UDP over port <24091>\n",link_id);
            printf("The AWS sent link ID=<%.1f> to Backend-Server B using UDP over port <24091>\n",link_id);

    	//receive message from serverA
        	struct sockaddr_in serverwho_address;
            double receieved_message_A;
            double receieved_message_B;
            double values_from_server_A[5];
            double values_from_server_B[5];
            int i = 0;
        	while(1){ 
                recvfrom(socket_aws_server,&receieved_message_A,sizeof(receieved_message_A),0,(struct sockaddr*) &serverwho_address,(socklen_t*) sizeof(serverwho_address));
                values_from_server_A[i] = receieved_message_A;
                i++;
                if(i == 5){
                    i = 0;
                    break;
                }
        	} 

        
           int m_A = values_from_server_A[0];
           sendto(socket_server,&m_A,sizeof(m_A),0,(struct sockaddr*) &serverB_address,sizeof(serverB_address));
           if(m_A == 1){
                 bandwidth = values_from_server_A[1];
                 length = values_from_server_A[2];
                 velocity = values_from_server_A[3];
                 noise_power = values_from_server_A[4];

             }
            printf("The AWS received <%d> match from Backend-Server <A> using UDP over port <24091>\n",m_A);

            while(1){ 
                recvfrom(socket_aws_server,&receieved_message_B,sizeof(receieved_message_B),0,(struct sockaddr*) &serverwho_address,(socklen_t*) sizeof(serverwho_address));
                values_from_server_B[i] = receieved_message_B;
                i++;
                if(i == 5){
                    break;
                }
            }
             int m_B = values_from_server_B[0];
             if(m_B == 1){
                 bandwidth = values_from_server_B[1];
                 length = values_from_server_B[2];
                 velocity = values_from_server_B[3];
                 noise_power = values_from_server_B[4];
             }
            printf("The AWS received <%d> match from Backend-Server <B> using UDP over port <24091>\n",m_B);

             if(m_A == 0 && m_B == 0)
             {
                printf("The AWS sent No Match to the monitor and the client using TCP over ports <26091> and <25091>\n");
                double send_message_error = 0;
                send(socket_monitor,&send_message_error,sizeof(send_message_error),0);
                send(socket_client,&send_message_error,sizeof(send_message_error),0);
                sendto(socket_server,&send_message_error,sizeof(send_message_error),0,(struct sockaddr*) &serverC_address,sizeof(serverC_address));
            }
            else{
        
            
                double send_message_serverC[] = {link_id,size,power,bandwidth,length,velocity,noise_power};
                for(int j = 0;j<7;j++){
                    sendto(socket_server,&send_message_serverC[j],sizeof(send_message_serverC[j]),0,(struct sockaddr*) &serverC_address,sizeof(serverC_address));
                }
                printf("The AWS sent link ID=<%.1f>,size=<%.2f>,power=<%.2f>,and link information to Backend-Server C using UDP over port number <24091>\n",link_id,size,power);

                //receieve message from server C

                double receieved_message_C;
                double values_from_serverC[4];
                i = 0;
                while(1){ 
                    recvfrom(socket_aws_server,&receieved_message_C,sizeof(receieved_message_C),0,(struct sockaddr*) &serverC_address,(socklen_t*) sizeof(serverC_address));
                    
                    values_from_serverC[i] = receieved_message_C;
                    i++;

                    if(i == 3){
                        break;
                    }
                }
                printf("The AWS receieved outputs from Backend-Server C using UDP over <24091>\n");

                //send delay to client
                double delay =  values_from_serverC[2];
                double Tt = values_from_serverC[0];
                double Tp = values_from_serverC[1];

                send(socket_client,&delay,sizeof(delay),0);
                printf("The AWS sent delay=<%.2f>ms to the client using TCP over port<25091>\n",delay);

                double send_message_f[4]={1,Tt,Tp,delay};
                i = 0;
                for(i=0;i<4;i++){
                    int send_state = send(socket_monitor,&send_message_f[i],sizeof(send_message_f[i]),0);
        
                    }
                
         
                printf("The AWS sent detailed results to the monitor using TCP over port<26091>\n");
            }
        

            close(socket_client);

        
    } 

    return 0;

}