// AS服务器端
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <fcntl.h> // for open
#include <unistd.h> // for close
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <time.h>
#include "DES.h"

#define AS_PORT 8081

int serverSocket; // 服务端Socket

char ClientID[] = "hupf3"; // AS服务器存储的用户名
char ClientKey[] = "MYKey"; // AS服务器存储的密钥
char TGS_Session_Key[] = "Client-TGSSessionKey";
char TGS_Key[] = "TGSKey";

// AS服务器端进行通信
void serve(struct sockaddr_in *client_addr, int clientSocket){
    int len = -1; // 存储接受到的字节长度
    char rec[100]; // 接受到的数据
    char A[100], B[100];

    // 接收数据
    while (len < 0) len = recv(clientSocket, rec, 100, 0);
    rec[len] = '\0';

    // 断开连接
    if (strcmp(rec, "quit") == 0) return ;

    // 检查服务器中是否有ClientID，没有则须返回错误信息
    if (strcmp(rec, ClientID) != 0){
        send(clientSocket, "Wrong ClientID!", strlen("Wrong ClientID!"), 0);
        return ;
    }

    // 发送加密后的消息A，即Client/TGS会话密钥
    int lenA = Encrypt(ClientKey, TGS_Session_Key, strlen(TGS_Session_Key), A);
    send(clientSocket, A, lenA, 0);

    sleep(2); // 等待客户端接收

    // 发送消息B，即票据授权票据(TGT)
    char tmpB[100];
    sprintf(B, "<%s,%s,%ld,%s>", ClientID, inet_ntoa(client_addr->sin_addr), time(NULL)+1231232, TGS_Session_Key);
    int lenB = Encrypt(TGS_Key, B, strlen(B), B);
    send(clientSocket, B, lenB, 0);
}

int main(){
    srand(time(NULL) + 1);
    struct sockaddr_in AS_addr;
    int addr_len = sizeof(struct sockaddr_in);
    pid_t pid;

    // 设置socked
    if ((serverSocket = socket(AF_INET, SOCK_STREAM, 0)) == -1){
        perror("Socket Error!\n");
        exit(1);
    }

    // 记录本地信息
    bzero(&AS_addr, sizeof(AS_addr)); // 最后位置赋0，补位置
    AS_addr.sin_family = AF_INET; // IPV4
    AS_addr.sin_port = htons(AS_PORT); // 绑定端口，并将其转化为网络字节
    AS_addr.sin_addr.s_addr = htonl(INADDR_ANY); // 指定接收IP为任意（如果指定IP，则只接收指定IP）
    
    // 绑定socked
    if (bind(serverSocket, (struct sockaddr*)&AS_addr, sizeof(AS_addr)) < 0){
        perror("Bind Error!\n");
        exit(1);
    }

    // 开启监听,可连接客户端最大为10个
    if (listen(serverSocket, 10) == -1){
        perror("listen error!\n");
        exit(1);
    }
    // 持续保持监听
    while (1){
        struct sockaddr_in client_addr;
        int clientSocket = accept(serverSocket, (struct sockaddr *)&client_addr, (socklen_t *)&addr_len);
        
        pid = fork(); // 创建子进程
        if (pid == 0){
            serve(&client_addr, clientSocket);
            exit(1);
        }
    }
    
    // 关闭Socket
    close(serverSocket);
    return 0;
}