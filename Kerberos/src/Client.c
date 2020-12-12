// 客户端
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

// 设置的端口号
#define AS_PORT 8081
#define TGS_PORT 8082
#define SS_PORT 8083

int clientSocket; // 客户端Socket
int addr_len;
char ClientID[100]; // 客户ID
char userKey[100]; // 客户端密钥
char ServiceID[] = "Sun_Yat-sen_University"; // 服务ID

// 时间戳
time_t timestamp1;
time_t timestamp2;

// 消息
char A[100], B[100], C[100], D[100], E[100], F[100], G[100], H[100];
// 消息长度
int lenA, lenB, lenC, lenD, lenE, lenF, lenG, lenH;

// 会话密钥
char TGS_Session_Key[100];
char SS_Session_Key[100];

// 创建Socket
struct sockaddr_in* createSocket(int PORT){
    struct sockaddr_in *ret = (struct sockaddr_in*)malloc(addr_len);
    ret->sin_family = AF_INET;
    ret->sin_port = htons(PORT); // 对应特定端口号
    ret->sin_addr.s_addr = inet_addr("127.0.0.1"); // 都设置为本地连接
    return ret;
}

// 与AS服务其建立联系
void connectWithAS(){
    printf("-------- AS --------\n");
    struct sockaddr_in *AS_addr = createSocket(AS_PORT);
    int len; // 记录返回的字节长度

    printf("用户 %s 正在请求服务...\n", ClientID);
    printf("等待与AS服务器建立连接...\n");

    // 判断是否建立socket成功
    if ((clientSocket = socket(AF_INET, SOCK_STREAM, 0)) == -1){
        perror("Socket Error!\n");
        exit(1);
    }

    // 使用connect连接AS服务器，判断是否连接成功
    if (connect(clientSocket, (struct sockaddr*)AS_addr, addr_len) == -1){
        perror("Connect AS Error!\n");
        exit(1);
    }

    // 成功连接到AS服务器
    printf("成功与AS服务器建立连接! \n");
    // 发送自己的用户ID
    printf("正在向AS服务器发送用户ID...\n");
    send(clientSocket, ClientID, strlen(ClientID), 0);
    len = recv(clientSocket, A, 100, 0); 
    A[len] = 0;

    // 检测ClientID是否符合条件
    if (strcmp(A, "Wrong ClientID!") == 0){
        printf("ClientID 不存在! \n");
        exit(1);
    }

    printf("成功接收消息A! \n");
    printf("正在解密消息A...\n");

    // 用自己的密钥解密消息A
    Decrypt(userKey, A, len, TGS_Session_Key);
    printf("通过用户密钥解密得到的Client-TGS会话密钥为: %s\n", TGS_Session_Key);

    // 成功接受消息B
    len = recv(clientSocket, B, 100, 0); 
    B[len] = 0;

    printf("成功接收消息B! \n");

    // 关闭socket
    close(clientSocket);
    printf("-------- AS --------\n");
}

// 与TGS服务其建立联系
void connectWithTGS(){
    printf("-------- TGS --------\n");
    struct sockaddr_in *TGS_addr = createSocket(TGS_PORT);

    printf("用户 %s 正在请求服务...\n", ClientID);
    printf("等待与TGS服务器建立连接...\n");

    // 判断是否建立socket成功
    if ((clientSocket = socket(AF_INET, SOCK_STREAM, 0)) == -1){
        perror("Socket Error!\n");
        exit(1);
    }

    // 使用connect连接TGS服务器，判断是否连接成功
    if (connect(clientSocket, (struct sockaddr*)TGS_addr, addr_len) == -1){
        perror("Connect TGS Error!\n");
        exit(1);
    }

    // 成功连接到TGS服务器
    printf("成功与TGS服务器建立连接! \n");

    printf("客户端ServiceID为: %s\n", ServiceID);
    printf("正在向TGS服务器发送由ServiceID和消息B构成的消息C...\n");

    // 消息C是由ServiceID和消息B构成的
    sprintf(C, "%s,%s", ServiceID, B);
    
    // 发送消息C
    send(clientSocket, C, strlen(C), 0);
    // printf("C: %s\n", C);

    sleep(3); // 等待服务器端接收

    // 消息D是由ClientID和timestamp经过TGS_Session_Key加密后得到的
    sprintf(D, "<%s,%ld>", ClientID, timestamp1);
    printf("消息D为: %s\n", D);
    printf("正在进行加密发送给TGS服务器消息D...\n");
    lenD = Encrypt(TGS_Session_Key, D, strlen(D), D);

    // 发送消息D
    send(clientSocket, D, lenD, 0);
    // printf("D: %s\n", D);

    // 接收消息E
    lenE = recv(clientSocket, E, 100, 0);
    E[lenE] = 0;
    printf("成功接收消息E! \n");

    // 判断是否认证成功
    if(strcmp(E, "Different ID!") == 0){
        printf("客户ID不一致，认证失败! \n");
        exit(1);
    }
    if (strcmp(E, "Be Overdue!") == 0){
        printf("有效期已过，认证失败! \n");
        exit(1);
    }

    // 接收消息F
    lenF = recv(clientSocket, F, 100, 0);
    F[lenF] = 0;
    printf("成功接收消息F! \n");

    // 对F进行解密
    Decrypt(TGS_Session_Key, F, lenF, SS_Session_Key);
    printf("通过Client-TGS会话密钥解密得到的Client-SS会话密钥为: %s\n", SS_Session_Key);

    // 关闭socket
    close(clientSocket);
    printf("-------- TGS --------\n");
}

// 与SS服务其建立联系
void connectWithSS(){
    printf("-------- SS --------\n");
    struct sockaddr_in *SS_addr = createSocket(SS_PORT);

    printf("用户 %s 正在请求服务...\n", ClientID);
    printf("等待与SS服务器建立连接...\n");

    // 判断是否建立socket成功
    if ((clientSocket = socket(AF_INET, SOCK_STREAM, 0)) == -1){
        perror("Socket Error!\n");
        exit(1);
    }

    // 使用connect连接SS服务器，判断是否连接成功
    if (connect(clientSocket, (struct sockaddr*)SS_addr, addr_len) == -1){
        perror("Connect SS Server Error!\n");
        exit(1);
    }

    // 成功连接到SS服务器
    printf("成功与SS服务器建立连接! \n");
    
    // 发送消息E
    printf("正在向SS服务器发送消息E...\n");
    send(clientSocket, E, lenE, 0);

    sleep(3); // 等待服务器端接收

    // 发送消息加密后的G
    sprintf(G, "<%s,%ld>", ClientID, timestamp1);
    printf("由用户ID和时间戳构成的消息G为: %s\n", G);
    lenG = Encrypt(SS_Session_Key, G, strlen(G), G);
    printf("正在进行加密发送给SS服务器消息G...\n");
    send(clientSocket, G, lenG, 0);

    // 接收消息H，并且读取其中的数据
    lenH = recv(clientSocket, H, 100, 0);
    printf("成功接收消息H! \n");
    char ClientID_t[100];
    Decrypt(SS_Session_Key, H, lenH, H);
    printf("通过Client-SS会话密钥解密得到的消息H为: %s\n", H);
    sscanf(H, "%[^,],%ld", ClientID_t, &timestamp2);

    if (timestamp2 == timestamp1 + 1){
        printf("可以信赖，认证结束! \n");
    }else {
        printf("不可以信赖! \n");
        exit(1);
    }
    // 关闭socket
    close(clientSocket);
    printf("-------- SS --------\n");
}

int main(){
    srand(time(NULL) + 1);
    timestamp1 = time(NULL);
    addr_len = sizeof(struct sockaddr_in);

    printf("请输入用户账户: ");
    scanf("%s", ClientID);
    printf("请输入用户密码: ");
    scanf("%s", userKey);

    connectWithAS(); // 与AS服务其建立联系
    connectWithTGS(); // 与TGS服务其建立联系
    connectWithSS(); // 与SS服务其建立联系

    return 0;
}