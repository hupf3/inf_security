// TGS服务器端
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

#define TGS_PORT 8082

int serverSocket; // 服务端Socket

// 服务器端密钥
char TGS_Key[] = "TGSKey";
char SS_Key[] = "SSKey";

char SS_Session_Key[] = "Client-SSSessionKey"; // SS服务器对话密钥

// TGS服务器端进行通信
void serve(struct sockaddr_in *client_addr, int clientSocket){
    // 时间戳
    time_t timestamp1 = 0;
    time_t timestamp2 = 0;
    int len = -1;
    char B[100], C[100], D[100], E[100], F[100]; // 消息
    char ServiceID[100]; // 服务ID
    char ClientID1[100]; char ClientID2[100];
    char client_net_addr[100];
    char TGS_Session_Key[100]; // 会话密钥

    // 读取消息C
    int lenC = recv(clientSocket, C, 100, 0);
    printf("成功接收消息C! \n");
    C[lenC] = 0;

    // 读取消息C中的信息获取消息B
    sscanf(C, "%[^,],%s", ServiceID, B);
    for (int i = strlen(ServiceID) + 1; i < lenC; ++i) B[i - (strlen(ServiceID) + 1)] = C[i];
    B[lenC - (strlen(ServiceID) + 1)] = 0;
    printf("读取消息C中的ServiceID为: %s\n", ServiceID);

    // 解密消息B，并且读取B消息中的数据
    Decrypt(TGS_Key, B, (lenC - strlen(ServiceID) - 1), B);
    sscanf(B, "<%[^,],%[^,],%ld,%[^>]>", ClientID1, client_net_addr, &timestamp1, TGS_Session_Key);
    printf("通过TGS密钥解密得到的消息B为: %s\n", B);

    // 读取消息D
    int lenD = recv(clientSocket, D, 100, 0);
    printf("成功接收消息D! \n");
    D[lenD] = 0;

    // 解密消息D，并且读取消息D中的数据
    Decrypt(TGS_Session_Key, D, lenD, D);
    printf("通过Client-TGS会话密钥解密得到的消息D为: %s\n", D);
    sscanf(D,"<%[^,],%ld,>", ClientID2, &timestamp2);

    // 需要对消息中的信息进行认证
    // 用户ID不一致
    if (strcmp(ClientID1, ClientID2) != 0){
        send(clientSocket, "Different ID!", strlen("Different ID!"), 0);
        return;
    }
    // 时间戳过期
    if (timestamp2 > timestamp1){
        send(clientSocket, "Be Overdue!", strlen("Be Overdue!"), 0);
        return;
    }

    char ST[100]; // 服务票据
    sprintf(ST, "<%s,%s,%ld,%s>", ClientID1, client_net_addr, timestamp2, SS_Session_Key);
    // 对服务票据进行加密
    printf("TGS服务器生成的服务票据ST为: %s\n", ST);
    printf("正在用SS密钥对ST进行加密...\n");
    int lenST = Encrypt(SS_Key, ST, strlen(ST), ST);

    printf("正在向客户端发送由ServiceID和ST构成的消息E...\n");
    // 生成消息E，并且发送给客户端
    sprintf(E, "%s,%s", ServiceID, ST);
    int lenE = strlen(ServiceID) + 1 + lenST;
    E[lenE] = 0;
    send(clientSocket, E, lenE, 0);

    sleep(3); // 等待客户端接收

    // 加密生成消息F,并且发送给客户端
    printf("Client-SS会话密钥为: %s\n", SS_Session_Key);
    int lenF = Encrypt(TGS_Session_Key, SS_Session_Key, strlen(SS_Session_Key), F);
    F[lenF] = 0;
    printf("正在向客户端发送由Client-TGS会话密钥加密得到的消息E...\n");
    send(clientSocket, F, lenF, 0);
}

int main(){
    srand(time(NULL) + 1);
    struct sockaddr_in TGS_addr;
    int addr_len = sizeof(struct sockaddr_in);
    pid_t pid;

    printf("等待与客户端建立连接...\n");

    // 设置socket
    if ((serverSocket = socket(AF_INET, SOCK_STREAM, 0)) == -1){
        perror("Socket Error!\n");
        exit(1);
    }

    // 记录本地信息
    bzero(&TGS_addr, sizeof(TGS_addr)); // 最后位置赋0，补位置
    TGS_addr.sin_family = AF_INET; // IPV4
    TGS_addr.sin_port = htons(TGS_PORT); // 绑定端口，并将其转化为网络字节
    TGS_addr.sin_addr.s_addr = htonl(INADDR_ANY); // 指定接收IP为任意（如果指定IP，则只接收指定IP）
    
    // 绑定socked
    if (bind(serverSocket, (struct sockaddr*)&TGS_addr, sizeof(TGS_addr)) < 0){
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