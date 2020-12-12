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

// 保存用户ID和密码的数据结构
typedef struct Client{
    char *ID;
    char *key;
}Client;

Client c[5];

int serverSocket; // 服务端Socket

char *ClientID; // AS服务器存储的用户名
char *ClientKey; // AS服务器存储的密钥
char TGS_Session_Key[] = "Client-TGSSessionKey"; // 会话密钥
char TGS_Key[] = "TGSKey"; // 服务端密钥

// 创建数据库中的数据
void createData(){
    c[0].ID = "hupengfei"; c[0].key = "MYKey1";
    c[1].ID = "huran"; c[1].key = "MYKey2";
    c[2].ID = "chenxiaolin"; c[2].key = "MYKey3";
    c[3].ID = "huangrui"; c[3].key = "MYKey4";
    c[4].ID = "lizihao"; c[4].key = "MYKey5";
}

// 判断数据库中是否有该数据 
// 0 : 不存在   
// 1 : 存在
int isInclude(char *ss){
    for (int i = 0; i < 5; ++i)
        if (strcmp(ss, c[i].ID) == 0) return 1;
    return 0;
}

// 获取该用户信息
void getClient(char *ss){
    for (int i = 0; i < 5; ++i){
        if (strcmp(ss, c[i].ID) == 0){
            ClientID = c[i].ID;
            ClientKey = c[i].key;
            return ;
        }
    }
}

// AS服务器端进行通信
void serve(struct sockaddr_in *client_addr, int clientSocket){
    int len = -1; // 存储接受到的字节长度
    char rec[100]; // 接受到的数据
    char A[100], B[100]; // 消息

    // 接收数据
    while (len < 0) len = recv(clientSocket, rec, 100, 0);
    rec[len] = '\0';
    printf("成功接收数据，用户ID为: %s\n", rec);

    int flag = isInclude(rec);

    // 检查服务器中是否有ClientID，没有则须返回错误信息
    if (flag == 0){
        printf("本服务器数据库中无该用户ID! \n");
        send(clientSocket, "Wrong ClientID!", strlen("Wrong ClientID!"), 0);
        return ;
    }

    getClient(rec); // 获取该用户信息

    printf("服务器端存有该用户ID: %s, 且密钥为: %s\n", ClientID, ClientKey);

    printf("Client-TGS会话密钥为: %s\n", TGS_Session_Key);
    printf("正在进行加密发送给客户端消息A...\n");
    // 发送加密后的消息A，即Client/TGS会话密钥
    int lenA = Encrypt(ClientKey, TGS_Session_Key, strlen(TGS_Session_Key), A);
    send(clientSocket, A, lenA, 0);

    sleep(3); // 等待客户端接收

    // 发送消息B，即票据授权票据(TGT)
    char tmpB[100];
    sprintf(B, "<%s,%s,%ld,%s>", ClientID, inet_ntoa(client_addr->sin_addr), time(NULL) + 123456, TGS_Session_Key);
    printf("消息B为: %s\n", B);
    printf("正在进行加密发送给客户端消息B...\n");
    int lenB = Encrypt(TGS_Key, B, strlen(B), B);
    send(clientSocket, B, lenB, 0);
}

int main(){
    srand(time(NULL) + 1);
    struct sockaddr_in AS_addr;
    int addr_len = sizeof(struct sockaddr_in);
    pid_t pid;

    createData();

    printf("等待与客户端建立连接...\n");

    // 设置socket
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
        printf("成功与客户端建立连接! \n");

        pid = fork(); // 创建子进程
        if (pid == 0){
            serve(&client_addr, clientSocket);
            printf("当前服务结束! \n");
            printf("---------------------\n");
            printf("等待与客户端建立连接...\n");
            exit(1);
        }
    }
    
    // 关闭Socket
    close(serverSocket);
    return 0;
}