// SS服务器端
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

#define SS_PORT 8083

int serverSocket; // 服务端Socket
char SS_Key[] = "SSKey"; // 服务器端密钥

// SS服务器端进行通信
void serve(struct sockaddr_in *client_addr, int clientSocket){
    // 时间戳
    time_t timestamp1;
    time_t timestamp2;
    time_t timestamp3;

    char E[100], G[100], H[100]; // 消息
    int len = -1;
    char SS_Session_Key[100];  // 会话密钥
    char ClientID1[100]; char ClientID2[100]; 
    char client_net_addr[100];

    // 接收消息E
    int lenE = recv(clientSocket, E, 100, 0);
    E[lenE] = 0;
    printf("成功接收消息E! \n");

    // 读取消息E的数据
    char ST[100]; char ServiceID[100];
    sscanf(E, "%[^,],%s", ServiceID, ST);
    for (int i = strlen(ServiceID) + 1; i < lenE; ++i) ST[i - (strlen(ServiceID) + 1)] = E[i];
    ST[lenE - (strlen(ServiceID) + 1)] = 0;
    printf("读取消息E得到的ServiceID为: %s\n", ServiceID);
    
    // 解密ST，并且进行读取数据
    Decrypt(SS_Key, ST, (lenE - strlen(ServiceID) - 1), ST);
    printf("读取消息E并经过SS密钥解密得到的ST为: %s\n", ST);
    sscanf(ST, "<%[^,],%[^,],%ld,%[^>]>", ClientID1, client_net_addr, &timestamp1, SS_Session_Key);

    // 接收消息G
    int lenG = recv(clientSocket, G, 100, 0);
    printf("成功接收消息G! \n");
    G[lenG] = 0;

    // 解密消息G,并且进行读取数据
    Decrypt(SS_Session_Key, G, lenG, G);
    printf("通过Client-SS会话密钥解密得到的消息G为: %s\n", G);
    sscanf(G, "<%[^,],%ld>", ClientID2, &timestamp2);

    if ((strcmp(ClientID1,ClientID2) != 0)){
        printf("客户ID不一致，认证失败! \n");
        return ;
    }

    // 生成消息H,并且发送回客户端
    timestamp3 = timestamp2 + 1;
    sprintf(H, "%s,%ld", ClientID1, timestamp3);
    printf("由用户ID和时间戳构成的消息H为: %s\n", H);
    printf("正在向客户端发送经Client-SS会话密钥加密得到的消息H...\n");
    int lenH = Encrypt(SS_Session_Key, H, strlen(H), H);
    send(clientSocket, H, lenH, 0);
}

int main(){
    srand(time(NULL) + 1);
    struct sockaddr_in SS_addr;
    int addr_len = sizeof(struct sockaddr_in);
    pid_t pid;

    printf("等待与客户端建立连接...\n");

    // 设置socket
    if ((serverSocket = socket(AF_INET, SOCK_STREAM, 0)) == -1){
        perror("Socket Error!\n");
        exit(1);
    }

    // 记录本地信息
    bzero(&SS_addr, sizeof(SS_addr)); // 最后位置赋0，补位置
    SS_addr.sin_family = AF_INET; // IPV4
    SS_addr.sin_port = htons(SS_PORT); // 绑定端口，并将其转化为网络字节
    SS_addr.sin_addr.s_addr = htonl(INADDR_ANY); // 指定接收IP为任意（如果指定IP，则只接收指定IP）
    
    // 绑定socked
    if (bind(serverSocket, (struct sockaddr*)&SS_addr, sizeof(SS_addr)) < 0){
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