#include "stdio.h"
#include "string.h"
#include "stdbool.h"
#include "stdlib.h"
#include "ctype.h"
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <netinet/in.h>

//-------------------variables--------------------
int text_alloc=0;
int flag_host=0;
int flag_path=0;
int request_alloc=0;
int flag_r=0;
int flag_p=0;
int flag_URL=0;
char* text =NULL;// come after p
char* host=NULL;
int  port=80;
char *path=NULL;
char *request=NULL;
int index_of_first_parameter_after_r=-1;
#define USAGE_ERR "Usage: client [-p n <text>] [-r n < pr1=value1 pr2=value2 …>] <URL>\n"
//-------------------variables--------------------

//-------------------functions--------------------
bool isnumber (char* str);
bool check_for_r_case(int argc, char **argv, int i);
bool check_for_p_case(int argc, char **argv, int i, int val);
void URL_case(int argc, char **argv, int i);
void make_request_function(int val, int ind_of_first_param_after_r ,char **argv);
void delete_memory();
//-------------------functions--------------------

/*will analyse the command ,
 * will create the socket file descriptor
 * and will make the connection.
*/
int main(int argc, char **argv){
    int val;
    int val_r=0;
    int i=1;
    while(i<argc ){

        if(argv[i][0] == '-' && ((argv[i][1] != 'p') && (argv[i][1] != 'r'))) {
            printf(USAGE_ERR);
            delete_memory();
            exit(0);
        }
        if(strcmp(argv[i], "-r")==0){
            if(flag_r==1) {
                printf(USAGE_ERR);
                delete_memory();
                exit(0);
            }
            flag_r=1;// I found the first r.
            i++;
            if(i>=argc || !isnumber(argv[i])) {//the next appearance in argv is not a number..
                printf(USAGE_ERR);
                delete_memory();
                exit(0);
            }
            val= (int)atoi(argv[i]);

            i++;
            int j=0;
            index_of_first_parameter_after_r=i;// index of the first parameter after r.
            if(val==0)
                index_of_first_parameter_after_r=0;
            for( ;  j<val && i<argc ;i++, j++){
                if( !check_for_r_case(argc, argv, i)){// something wrong with the parameters.
                    printf(USAGE_ERR);
                    delete_memory();
                    exit(0);
                }

            }
            if(j<val){// if there are less values than the expected.
                printf(USAGE_ERR);
                delete_memory();
                exit(0);
            }
            if((i+1)<=argc && check_for_r_case(argc, argv, i)){
                printf(USAGE_ERR);
                delete_memory();
                exit(0);
            }
            val_r=val;
            continue;
        }
        if(i>=argc || strcmp(argv[i], "-p")==0){
            if(flag_p==1) {
                printf(USAGE_ERR);
                delete_memory();
                exit(0);
            }
            flag_p=1;// I found the first r.
            i++;

            if(i>=argc || !isnumber(argv[i])) {//the next appearance in argv is not a number..
                printf(USAGE_ERR);
                delete_memory();
                exit(0);
            }

            val= (int)atoi(argv[i]);
            if(val==0){
                printf(USAGE_ERR);
                delete_memory();
                exit(0);
            }

            i++;
            if(i<argc)
                if(!check_for_p_case(argc, argv, i, val)) {// make text to point on the next text of the URL.
                    printf(USAGE_ERR);
                    delete_memory();
                    exit(0);
                }

            if(i>=argc|| (val>(int) strlen(argv[i]))){
                printf(USAGE_ERR);
                delete_memory();
                exit(0);
            }

            i++;
            continue;
        }
        if( argv[i][0] != '-' ){//this is the URL
            if(strncmp(argv[i],"http://" ,7) !=0 || flag_URL==1) {// not begin with "http://", so it is an error.
                printf(USAGE_ERR);
                delete_memory();
                exit(0);
            }
            flag_URL=1;
            //If we are here, so it is begin with "http://"
            URL_case(argc, argv, i);
            // now I have host, path and the port
        }
        i++;
    }
    if(port>65536 || port<0){
        printf(USAGE_ERR);
        delete_memory();
        exit(1);
    }
    if(flag_URL==0){// There is no host.
        printf(USAGE_ERR);
        delete_memory();
        exit(0);
    }

    //-------------------create_connection----------------
    int socket_fd;  //create an endpoint for communication
    if((socket_fd= socket( AF_INET, SOCK_STREAM , 0) )< 0){ //checking success
        perror("ERROR with create tour socket fd");
        delete_memory();
        exit(1);
    }
    struct hostent * hp;
    struct sockaddr_in peeraddr;

    if(host==NULL)
        host="";
    make_request_function(val_r,index_of_first_parameter_after_r, argv);
    printf("HTTP request =\n%s\nLEN = %d\n", request, (int)strlen(request));
    if((hp = gethostbyname(host))==NULL){//I got the host ip address.
        herror("");// So, the ip is NULL.
        close(socket_fd);
        delete_memory();
        exit(1);
    }

    peeraddr.sin_family = AF_INET;
    peeraddr.sin_addr.s_addr = ((struct in_addr*)(hp->h_addr))->s_addr;// fill the ip address in my struct
    bcopy((char *)hp->h_addr, (char *) &peeraddr.sin_addr.s_addr, hp->h_length);
    peeraddr.sin_port = htons(port);

    if(connect(socket_fd ,(const struct sockaddr*)&peeraddr, sizeof(peeraddr)) <0){// make a connection.
        perror("\n problem with make a connection!\n");
        delete_memory();
        exit(0);
    }
//    make_request_function(val_r,index_of_first_parameter_after_r, argv);
    if(request==NULL){
        printf("\nERROR with request\n");
        delete_memory();
        exit(1);
    }

    int sent;
    int size_read =0;
    char buff[20];
    if((sent= write(socket_fd, request, (int)strlen(request)+1))<0) {//send request to the server
        perror("\nERROR with the write function\n");
        delete_memory();
        exit(1);
    }
    while (true){
        if(sent==0)
            break;
        if((sent= read(socket_fd, buff, sizeof(buff)))<0) {
            perror("\nERROR with the read function\n");
            delete_memory();
            exit(1);
        }
        buff[sent] = 0;
        printf("%s", buff);
        size_read+=sent;
    }
    delete_memory();
    close(socket_fd);
    //-------------------create_connection----------------
    printf("\n Total received response bytes: %d\n",size_read);
    return 0;
}

//-------------------check_if_is_number----------------
// checks if string is a number.
bool isnumber (char* str){
    if(str==NULL)
        return false;
    for(int i = 0 ; i < (int)strlen(str) ; i++)//for each char, check if it is a digit or not.
        if(!isdigit(str[i]))
            return false;
    return true;
}
//-------------------check_if_is_number----------------

//-------------------r_p_case----------------
/*f the command contains  -r [number]
 * it will check some extremely cases.*/
bool check_for_r_case(int argc, char **argv, int i){
    /* if the parameter not contain "=",
     * or it is not kind of "a=b",
     * or the first/last char of it is "="
     * So there is a problem*/
    if(strchr(argv[i], '=') == NULL || (int)(strlen(argv[i])<3) || (argv[i][0] == '=') || (argv[i][(int)strlen(argv[i])-1]=='=') ) {
        return false;
    }
    return true;
}

/*if the command contains -p [number]
 * it will allocate the right text or will just point on it. */
bool check_for_p_case(int argc, char **argv, int i, int val){

    if((int)strlen(argv[i])<val || strchr(argv[i], '-') != NULL) {// if this place contain '-' there is a problem.
        return false;
    }

    if((int)strlen(argv[i])>val) { // take only the first characters of argv[i]
        text_alloc=1;
        text = (char *) malloc(sizeof(char) * (val + 1));
        if(text==NULL){
            perror("problem with allocate the text at function: check_for_p_case");
            delete_memory();
            exit(1);
        }
        int j=0;
        for(; j<val && j<= (int)strlen(argv[i]); j++ ) {
            if(!isalpha(argv[i][j])){
                printf(USAGE_ERR);
                delete_memory();
                exit(0);
            }

            text[j] = argv[i][j];
        }
        text[j]=0;
        return true;
    }

    if((int)strlen(argv[i])==val) {
        text = argv[i];

    }
    return true;
}
//-------------------r_p_case----------------

//-------------------build_URL----------------
/* will separate the url to path and host and if it is contains a port number,
 * so it will change the default one (80) to it.*/
void URL_case(int argc, char **argv, int i){
    int flag_port=0;

    int j=7;
    while (j< (int)strlen(argv[i])){
        if(strchr(argv[i]+7, ':')==NULL && strchr(argv[i]+7, '/')==NULL && flag_host==0){//make host
            flag_host=1;
            host = (char *) malloc(sizeof(char) * (int)strlen(argv[i])+1);
            if(host==NULL){
                perror("\n your allocate of port was failed : URL_case function\n");
                delete_memory();
                exit(1);
            }
            strncpy(host, argv[i]+7, (int)strlen(argv[i]));
            return;
        }
        if((argv[i][j]== '/' || argv[i][j]== ':') && flag_host==0) {
            if(flag_port==1 || flag_path==1){
                printf(USAGE_ERR);
                delete_memory();
                exit(1);
            }
            flag_host=1;
            host = (char *) malloc(sizeof(char) * (j - 7)+1);
            if(host==NULL){
                perror("\n your allocate of port was failed : URL_case function\n");
                delete_memory();
                exit(1);
            }
            strncpy(host, argv[i] + 7, j - 7);
            host[j - 7] = 0;
        }

        if(argv[i][j]==':' && flag_port==0){// so there is a port
            if(flag_host==0 || flag_path==1 ){
                printf(USAGE_ERR);
                delete_memory();
                exit(0);
            }
            flag_port=1;
            int k=j;
            for(;argv[i][k]!=0 && argv[i][k]!='/'; k++);
            j++;
            if(k-j==0){// case of just ':' without port
                printf(USAGE_ERR);
                delete_memory();
                exit(1);
            }
            char *port1=(char *) malloc(sizeof(char )* (j-7)+1);
            if(port1==NULL){
                printf(USAGE_ERR);

                delete_memory();
                exit(1);
            }
            strncpy(port1, argv[i]+j, k-j);
            port1[k-j] = 0;

            if(!isnumber(port1)){
                printf(USAGE_ERR);
                free(port1);
                port1=NULL;
                delete_memory();
                exit(1);
            }
            port= atoi(port1);
            free(port1);
            port1=NULL;


            j=k;
        }// so I found ":"

        if(argv[i][j]== '/' && flag_path==0){
            if(flag_host==0 ){
                printf(USAGE_ERR);
                delete_memory();
                exit(1);
            }
            flag_path=1;
            path=(char *) malloc(sizeof(char) * (strlen(argv[i]))-7 +1);
            if(path==NULL){
                perror("\n your allocate of port was failed : URL_case function\n");
                delete_memory();
                exit(1);
            }
            strncpy(path, argv[i] + j, strlen(argv[i])-7);
            path[strlen(argv[i])-7] = 0;
        }
        j++;
    }
}
//-------------------build_URL-------------------

//-------------------build_request----------------
void make_request_function(int val, int ind_of_first_param_after_r, char **argv ){
    int size_of_request=0;
    size_of_request+=(int) strlen("GET") /*+(int) strlen("Request:\r\n")*/;
    if(flag_p==1)
        size_of_request= (int) strlen("POST") ;
    size_of_request+=(int) strlen(" HTTP/1.0")+ (int) strlen("\r\n") + (int) strlen("\r\n\r\n");
    if(path!=NULL)
        size_of_request+=(int) strlen(path);
    if(host!=NULL)
        size_of_request+=(int) strlen(host);
    if(flag_r == 1){
        int j=index_of_first_parameter_after_r;
        for(int i=0 ; i<val && argv[j]!=0 ; i++) {
            size_of_request += (int) strlen(argv[j]);
            j++;
        }
    }
    if(text!=NULL)
        size_of_request+=(int) strlen(text);

    size_of_request+=100;// for safe...

    request= (char *)malloc(sizeof(char )* (size_of_request+1));
    request_alloc=1;
    if(request==NULL){
        perror("\nproblem with malloc the request input, from function: make_request_function\n");
        delete_memory();
        exit(1);
    }
    strcpy(request,"");
    if(flag_p==1)
        strcat(request,"POST ");
    if(flag_p==0)
        strcat(request,"GET ");
    if(flag_path==1) {
        strcat(request, path);
    }
    if(flag_path==0) {
        strcat(request, "/");
    }

    if(flag_r==1 && index_of_first_parameter_after_r!=0){
        strcat(request, "?");
        int j=ind_of_first_param_after_r;
        for(int i=0 ; i<val && argv[j]!=0 ; i++, j++){
            strcat(request, argv[j]);
            if(i+1!=val)
                strcat(request,"&");
        }
    }

    strcat(request, " HTTP/1.0\r\nHost: ");
    if(flag_host==1)
        strcat(request, host);

    if(text!=NULL){
        strcat(request, "\r\nContent-length:");
        char buffer[10];
        int value = (int)strlen(text);
        sprintf(buffer, "%d", value);
        strcat(request, buffer);

        strcat(request, "\r\n\r\n");
        strcat(request, text);
        strcat(request, "\r\n");
        return;
    }
    if(text==NULL)
        strcat(request, "\r\n\r\n");


}
//-------------------build_request----------------

void delete_memory(){
    if(flag_host==1) {
        free(host);
        host=NULL;
    }
    if(flag_path==1) {
        free(path);
        path=NULL;
    }
    if(text_alloc==1) {
        free(text);
        text=NULL;
    }
    if(request_alloc==1){
        free(request);
        request=NULL;
    }

}
