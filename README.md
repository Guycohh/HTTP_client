# HTTP_client

HTTP client
Authored by Guy Cohen

#Description

HTTP stands for Hyper Text Transfer Protocol and is used for communication among web clients and servers. HTTP has a simple stateless client/server paradigm. The web client initiates a conversation by opening a connection to the server. Once a connection is set up, the client sends an HTTP request to the server. Upon receiving the HTTP request from the client, the server sends an HTTP response back to the client. An HTTP request consists of two parts: a header and a body (In this project, the basic HTTP request from a client doesn’t contain a body). 
this project will implement the following content: An HTTP client that constructs an HTTP request based on the user’s command line input, sends the request to a Web server, receives the reply from the server, and displays the reply message on the screen. 

#Program D.B.
The main databases of this program are dynamic arrays. the program will allocate the host, text, path. finally all of the allocations will create the request, that will be allocated too as allocated array.

#Functions
-int main(int argc, char **argv)—> will analyse the command ,will create the socket file descriptor and will make the connection. 

-bool isnumber (char* str)--> checks if string is a number.

-bool check_for_r_case(int argc, char **argv, int i)--> if the command contains  -r [number] it will check some extremely cases.

-bool check_for_p_case(int argc, char **argv, int i, int val)--> if the command contains -p [number] it will allocate the right text or will just point on it. 



-void URL_case(int argc, char **argv, int i)--> will separate the url to path and host and if it is contains a port number, so it will change the default one (80) to it.

-void make_request_function(int val, int ind_of_first_param_after_r, char **argv )--> this function organise the client request format.

void delete_memory()—> free the memory.

#Input
Command line usage: client [–p n <text>] [–r n <pr1=value1 pr2=value2 …>] <URL>. The flags and the URL can come in any order, the only limitation is that the parameters should come right after the flag –r and the text should come right after the flag –p. 

#Output
an HTTP request based on the options specified in the command line
connection to the server,  sending the HTTP request to the server, receiving  an HTTP response and displaying the response on the screen.

1.in any case of a failure in one of the system calls, it will be "perror(<sys_call>)" and exiting the program. for errors on gethostbyname it will be herror ).
2. In any case of wrong command usage, the program will print "Usage: client [-p n <text>] [-r n < pr1=value1 pr2=value2 …>] <URL>" 
  
#Program Files
client.c

#How to compile?
compile: gcc -Wall –o client client.c
run: ./client 





