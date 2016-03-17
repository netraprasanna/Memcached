#include <mysql.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <my_global.h>
#include <my_sys.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h> 

my_bool mem_delete_init(UDF_INIT *initid, UDF_ARGS *args, char *message);
long long mem_delete(UDF_INIT *initid, UDF_ARGS *args, char *is_null, char *error);
void mem_delete_deinit(UDF_INIT *initid);

/*init the UDF*/
my_bool mem_delete_init(UDF_INIT *initid, UDF_ARGS *args, char *message)
{
return 0;
}

/*
Open a new socket connection on the port at which memcached is running and perform
the delete operation to invalidate entries from memcached
*/
long long mem_delete(UDF_INIT *initid,
                      UDF_ARGS *args,
                      __attribute__ ((unused)) char *is_null,
                      char *error)
{

    int sockfd = 0, n = 0;
    char recvBuff[1024];
    struct sockaddr_in serv_addr; 
    memset(recvBuff, '0',sizeof(recvBuff));
    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Error : Could not create socket \n");
        return (long long) 0;
    } 
    memset(&serv_addr, '0', sizeof(serv_addr)); 

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(11211); 

    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0)
    {
        printf("\n inet_pton error occured\n");
        return (long long) 0;
    } 

    if( connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
       printf("\n Error : Connect Failed \n");
       return (long long) 0;
    } 
char buf[512],buf1[255];
strcpy(buf1,"delete ");
strcat(buf1,args->args[0]);
strcat(buf1,"\r\n");
char *msg = "delete trust\r\n";
int bytes;
bytes=send(sockfd, buf1, strlen(buf1), 0);
bytes = recv(sockfd,buf,sizeof(buf),0);
return (long long) 1;
}

/*deinit the UDF*/
void mem_delete_deinit(UDF_INIT *initid)
{

}
