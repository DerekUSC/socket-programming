all : client.cpp aws.cpp
	g++ client.cpp -o client
	g++ aws.cpp -o aws.o
	g++ monitor.cpp -o monitor.o
	g++ serverA.cpp -o serverA.o
	g++ serverB.cpp -o serverB.o
	g++ serverC.cpp -o serverC.o

.PHONY: serverA
serverA : serverA
		./serverA.o

.PHONY: serverB
serverB : serverB
		./serverB.o
.PHONY: serverC
serverC : serverC
		./serverC.o
.PHONY: aws
aws : aws
	./aws.o
.PHONY: monitor
monitor : monitor
		./monitor.o

clean:
	rm -r client aws.o monitor.o serverA.o serverB.o serverC.o




