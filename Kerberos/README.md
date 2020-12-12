# Kerberos认证模型设计报告

## 个人信息

| 课程名称 | 信息安全技术 |   任课老师   |      蔡国扬      |
| :------: | :----------: | :----------: | :--------------: |
|   年级   |    2018级    | 专业（方向） |   软件工程专业   |
|   学号   |   18342025   |     姓名     |      胡鹏飞      |
|   电话   | 13944589695  |    Email     | 945554668@qq.com |

## 文件结构说明

本项目的文件结构如下图所示：

![](./img/7.png)

- `Makefile`：为本次项目构建和运行编写的 `Makefile` 文件
- `README.md`：本次项目的运行说明和设计报告
- `img`：存储 `README.md` 中需要的图片资源
- `include`：存储了本次项目所包括的 C 语言头文件
- `src`：存储了本次项目包括的 C 语言文件

## 代码运行说明

进入到 `Kerberos` 目录下执行以下命令

`make clean`

`make`

执行完上面的命令后本项目文件夹会出现两个新的目录 `build` 和 `bin` 

![](./img/8.png)

`build` 是用来存放 `.o` 中间文件，`bin` 目录存放本次项目的可执行文件

接下来打开四个终端，按照下面的顺序分别在不同的终端输入以下命令：

`./AS`

`./TGS`

`./SS`

`./Client`

按照以上步骤即可运行本程序，看到实验结果

**注意**：在运行前注意自己端口号为 8081，8082，8083的端口是否被占用，如果被占用可以修改代码中的端口号，重新运行本次项目。查看端口号是否被运行的命令如下所示，并且没有被占用的情况也如下：

![](./img/1.png)

## 项目地址

[Github](https://github.com/hupf3/inf_security/tree/master/Kerberos)

## 实验环境

**操作系统**：MacOS

**编辑器**：Visual Studio Code 

## 原理概述

### Kerberos 介绍

Kerberos 是种网络身份验证协议,最初设计是用来保护雅典娜工程的网络服务器。Kerberos 这个名字源于希腊神话，是一只三头犬的名字，它旨在通过使用密钥加密技术为 Client/Server 序提供强身份验证。可以用于防止窃听、防止重放攻击、保护数据完整性等场合，是一种应用对称密钥体制进行密钥管理的系统。Kerberos 的扩展产品也使用公开密钥加密方法进行认证。

### 名词解释

- **AS（Authentication Server）**：认证服务器
- **KDC（Key Distribution Center）**：密钥分发中心
- **TGT（Ticket Granting Ticket**）：票据授权票据，票据的票据
- **TGS（Ticket Granting Server）**：票据授权服务器
- **SS（Service Server）**：特定服务提供端
- **Principal**：被认证的个体
- **Ticket**：票据，客户端用来证明身份真实性。包含：用户名，IP，时间戳，有效期，会话密钥

### 参与的各方角色

- **Client**: Application Client 应用客户端
- **AS**: Authentication Server 用来认证用户身份
- **TGS**: Ticket-Granting Service 用来授权服务访问
- **SS**: Service Server 用户所请求的服务

### 认证步骤

本次 `Kerberos` 认证模型过程主要分为四个部分，下面将分别介绍这四个部分

#### 用户登录

用户在登录的时候需要输入**用户名**和**密码**两个信息。然后在客户端，接收到用户输入的**密码**信息，将密码通过一个单向的 `Hash` 函数生成一个 `Client` 密钥

#### 客户端与 AS 服务器通信

客户端向 AS 发送一个明文消息，申请基于该用户所应享有的服务，比如“用户Bob想请求服务“(Bob 是该用户的 ID)。

![在这里插入图片描述](./img/11.png)

**注意**：用户是不会向 AS 发送用户密钥的，AS 会从本地的数据库中查询到该申请用户的密码，并且通过上面用户登录相同的单向 `Hash` 函数生成一个密钥

AS 首先会检查该用户 ID 是否存在于本地的数据库中：

- 如果本地数据库中没有改用户 ID 则会向客户端发送信息：“该用户不存在”，然后认证失败
- 如果数据库中存在改用户，则返回给客户端两条信息：
  - 消息 A：**Client/TGS会话密钥(Client/TGS Session Key)**（该Session Key 用在将来 Client 与 TGS 的通信上），通过 **用户密钥(user's secret key)** 进行加密
  - 消息 B：**票据授权票据(TGT)**（TGT 包括：消息A中的“Client/TGS会话密钥”(Client/TGS Session Key)，用户ID，用户网址，TGT有效期），通过**TGS密钥(TGS's secret key)** 进行加密

客户端收到消息 A 和消息 B，就可以尝试用自己的用户密钥对消息 A 进行解密，如果用户输入的密码与 AS 服务器数据库的密码相符就能够成功解密，得到 Client/TGS 会话密钥。**注意**：用户不能够解密消息 B，因为用户并没有 TGS 密钥。

解密得到了 Client/TGS 会话密钥，就可以与 TGS 服务器进行通信并且认证了

![在这里插入图片描述](./img/10.png)

#### 客户端与 TGS 服务器通信

客户端需要申请特定服务，向 TGS 服务器发送以下两条消息：

- 消息 C：即消息B的内容（TGS's secret key 加密后的TGT），和想获取的服务的服务ID（**注意**：不是用户ID）
- 消息 D：**认证符(Authenticator)**（Authenticator包括：用户ID，时间戳），通过**Client/TGS会话密钥(Client/TGS Session Key)**进行加密

![在这里插入图片描述](./img/9.png)

TGS 服务器端收到消息 C 和消息 D 后，首先检查数据库中是否存在所需的服务，查找到之后，TGS 用自己的 “TGS密钥” (TGS's secret key)解密消息 C 中的消息 B（也就是TGT），从而得到之前生成的“Client/TGS会话密钥”(Client/TGS Session Key)。TGS 再用这个 Session Key 会话密钥解密消息 D 得到包含用户ID和时间戳的 Authenticator，并对 TGT 和 Authenticator 进行验证，验证通过之后返回2条消息：

- 消息 E：**client-server票据(client-to-server ticket)**（该 ticket 包括：Client/SS 会话密钥 (Client/Server Session Key），用户 ID，用户网址，有效期），通过提供该服务的**服务器密钥(service's secret key)** 进行加密
- 消息 F：**Client/SS会话密钥( Client/Server Session Key)** （该Session Key 用在将来 Client 与 Server Service 的通信（会话）上），通过**Client/TGS会话密钥(Client/TGS Session Key)** 进行加密

客户端收到这些消息后，用 Client/TGS会话密钥解密消息 F 得到“Client/SS会话密钥”(Client/Server Session Key)。（**注意**：Client 不能解密消息 E，因为 E 是用“服务器密钥”(service's secret key)加密的）。

![在这里插入图片描述](./img/12.png)

#### 客户端与 SS 服务器通信

当获得 “Client/SS会话密钥”(Client/Server Session Key) 之后，客户端就能够使用服务器提供的服务了。客户端向SS服务器发出2条消息：

- 消息 E：即上一步中的消息 E“client-server票据”(client-to-server ticket)，通过**服务器密钥(service's secret key)** 进行加密
- 消息 G：新的**Authenticator**（包括：用户ID，时间戳），通过**Client/SS会话密钥(Client/Server Session Key)** 进行加密

![在这里插入图片描述](./img/13.png)

SS 服务器用自己的密钥(service's secret key)解密消息 E 从而得到TGS提供的Client/SS会话密钥(Client/Server Session Key)。再用这个会话密钥解密消息 G。得到 Authenticator，（同TGS一样）对 Ticket 和 Authenticator 进行验证，验证通过则返回1条消息：

- 消息 H：**新时间戳**（新时间戳是：Client 发送的时间戳加1），通过**Client/SS会话密钥(Client/Server Session Key)** 进行加密

![在这里插入图片描述](./img/14.png)

客户端通过 Client/SS 会话密钥(Client/Server Session Key)解密消息 H，得到新时间戳并验证其是否正确。验证通过的话则客户端可以信赖服务器，并向 SS 服务器发送服务请求；否则验证失败。

SS 服务器向客户端提供相应的服务。至此整个 Kerberos 认证过程结束。

#### 加密/解密算法

本次认证过程中需要对消息进行加密/解密操作，这里我选择的算法是 DES 算法，也是将第一次作业的内容进行了部分的修改整理，并且封装好了两个函数，分别是 `Encrypt` 加密和 `Decrypt` 解密放在 `DES.h` 文件中，方便认证过程中进行调用。

## 总体结构设计

![](./img/15.png)

本次实验涉及到的代码文件如上图所示，下面分别介绍一下每个代码文件实现的作用：

- `include`：包含本次项目的头文件
  - `DES.h`：封装好的 DES 算法头文件，包括两个函数，分别是加密和解密
  - `Data.h`：包含 DES 算法所需要的表的信息
- `src`：包含本次项目的 C 语言文件
  - `Client.c`：本次认证模型的客户端
  - `DES.c`：实现 DES 算法的 C 文件，里面包含了代码实现的具体流程
  - `ServerAS.c`：AS 服务器与客户端通信的过程
  - `ServerTGS.c`：TGS 服务器与客户端通信的过程
  - `ServerSS.c`：SS 服务器与客户端通信的过程

在客户端中主要实现了三个函数，分别于三个不同的客户端建立了联系：

```c
connectWithAS(); // 与AS服务其建立联系
connectWithTGS(); // 与TGS服务其建立联系
connectWithSS(); // 与SS服务其建立联系
```

每个服务端都有如下的结构，在 `main` 函数中与客户端建立联系，然后通过 `serve` 函数与客户端进行通信。

## 数据结构设计

本次实验中并没有设计什么数据结构，就说一下本次实验中涉及到的一些变量的类型：

本次的消息我都是用的 `char` 数组进行存储的，以及相关的短期密钥，会话密钥，用户名，用户密码等信息：

```c
// 消息
char A[100], B[100], C[100], D[100], E[100], F[100], G[100], H[100];
// 消息长度
int lenA, lenB, lenC, lenD, lenE, lenF, lenG, lenH;

// 会话密钥
char TGS_Session_Key[100];
char SS_Session_Key[100];

// 服务器端密钥
char TGS_Key[] = "TGSKey";
char SS_Key[] = "SSKey";

char ClientID[100]; // AS服务器存储的用户名
char ClientKey[100]; // AS服务器存储的密钥
```

本实验中需要用到时间戳的概念，所以我选择了 `time.h` 头文件中的 `time_t` 变量进行存储该时间戳：

```c
// 时间戳
time_t timestamp1;
time_t timestamp2;
time_t timestamp3;
```

本次实验由于需要三个不同的服务器端与客户端进行相连所以需要，设置端口，端口的设置我直接用 `#define` 进行存储

```c
// 设置的端口号
#define AS_PORT 8081
#define TGS_PORT 8082
#define SS_PORT 8083
```

由于本次实验有由多种数据组成的票据，我都将他们存储为了字符串的形式，具体存储的格式我是按照老师的 PPT 进行规范的，存储的方法也是根据 `sprintf` 函数，读取票据中的数据用的是 `sscanf` 函数，下面举一个例子：

```c
// 生成票据
sprintf(B, "<%s,%s,%ld,%s>", ClientID, inet_ntoa(client_addr->sin_addr), time(NULL) + 123456, TGS_Session_Key);
```

增加了服务器端数据库的设计，就是通过结构体来存储用户 ID 和用户密钥：

```c
// 保存用户ID和密码的数据结构
typedef struct Client{
    char *ID;
    char *key;
}Client;
```

并且设计了创建数据库数据的函数，以及查找数据库数据的函数以及获取数据的函数：

```c
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
```

## 模块分解

本次项目的主要过程就是建立客户端和服务端，并且在二者之间进行通信，所以下面分别介绍，自己创建的客户端，三个服务端，以及它们之间通信的过程：

- **客户端输入账户密码**

  首先需要在客户端输入自己的账户密码：

  ```c
  printf("请输入用户账户: ");
  scanf("%s", ClientID);
  printf("请输入用户密码: ");
  scanf("%s", userKey);
  ```

- **客户端与服务器端的建立与通信**：

  - 客户端：首先需要定义与每个客户端连接的端口号：

    ```c
    // 设置的端口号
    #define AS_PORT 8081
    #define TGS_PORT 8082
    #define SS_PORT 8083
    ```

    然后建立 `Socket`, 设置 IP 地址，连接方式，对应的端口号，即可成功进行连接：

    ```c
    // 创建Socket
    struct sockaddr_in* createSocket(int PORT){
        struct sockaddr_in *ret = (struct sockaddr_in*)malloc(addr_len);
        ret->sin_family = AF_INET;
        ret->sin_port = htons(PORT); // 对应特定端口号
        ret->sin_addr.s_addr = inet_addr("127.0.0.1"); // 都设置为本地连接
        return ret;
    }
    
    struct sockaddr_in *AS_addr = createSocket(AS_PORT);
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
    ```

  - 服务器端：由于服务器端创建以及与客户端的通信都是相同的过程只是端口号不同，所以在此只举出了 AS 服务器端创建与通信的过程。

    首先需要设置 `socket`；

    ```c
    // 设置socket
    if ((serverSocket = socket(AF_INET, SOCK_STREAM, 0)) == -1){
      perror("Socket Error!\n");
      exit(1);
    }
    ```

    然后记录本地的信息，在地址的最后位置赋0，补位置；设置成 IPV4 协议，绑定端口，并将其转化为网络字节；指定接收任意的 IP 地址；

    ```c
    // 记录本地信息
    bzero(&AS_addr, sizeof(AS_addr)); // 最后位置赋0，补位置
    AS_addr.sin_family = AF_INET; // IPV4
    AS_addr.sin_port = htons(AS_PORT); // 绑定端口，并将其转化为网络字节
    AS_addr.sin_addr.s_addr = htonl(INADDR_ANY); // 指定接收IP为任意（如果指定IP，则只接收指定IP）
    
    ```

    绑定 `socket` ;

    ```c
    // 绑定socked
    if (bind(serverSocket, (struct sockaddr*)&AS_addr, sizeof(AS_addr)) < 0){
      perror("Bind Error!\n");
      exit(1);
    }
    ```

    开启监听，设置可连接的最大客户端数量；

    ```c
    // 开启监听,可连接客户端最大为10个
    if (listen(serverSocket, 10) == -1){
      perror("listen error!\n");
      exit(1);
    }
    ```

    由于服务器端需要持续监听客户端的请求，所以设置一个无限循环 `while(1)` ；成功建立连接后，可以创建子进程开始服务。

    ```c
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
    ```

- **客户端向 AS 服务端发送数据**：

  首先客户端需要发送刚刚用户输入的用户名给 AS 服务端，让其在数据库中进行查询该用户 ID

  ```c
  send(clientSocket, ClientID, strlen(ClientID), 0);
  ```

- **AS 服务端向客户端发送数据**：

  客户端接收到数据后在数据库中进行查找该用户 ID，如果不存在则会返回客户端一个错误信息：

  ```c
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
  ```

  如果存在用户 ID 的话，则会向客户端发送由用户密钥经过 DES 算法加密后的 Client/TGS 会话密钥构成的消息 A

  ```c
  int lenA = Encrypt(ClientKey, TGS_Session_Key, strlen(TGS_Session_Key), A);
  send(clientSocket, A, lenA, 0);
  ```

  等待客户端接收后，向客户端发送消息 B即票据授权票据(TGT)，该票据是由用户 ID，客户网络地址，票据有效期，Client/TGS 会话密钥构成的，此消息也需要经过 TGS 密钥进行加密：

  ```c
  sleep(3); // 等待客户端接收
  
  // 发送消息B，即票据授权票据(TGT)
  char tmpB[100];
  sprintf(B, "<%s,%s,%ld,%s>", ClientID, inet_ntoa(client_addr->sin_addr), time(NULL) + 123456, TGS_Session_Key);
  printf("消息B为: %s\n", B);
  printf("正在进行加密发送给客户端消息B...\n");
  int lenB = Encrypt(TGS_Key, B, strlen(B), B);
  send(clientSocket, B, lenB, 0);
  ```

- **客户端接收 AS 服务端发送的数据**：

  客户端首先判断从 AS 服务端接收到的数据是否为错误信息，如果是错误信息，则终止此次连接：

  ```c
  len = recv(clientSocket, A, 100, 0); 
  A[len] = 0;
  
  // 检测ClientID是否符合条件
  if (strcmp(A, "Wrong ClientID!") == 0){
    printf("ClientID 不存在! \n");
    exit(1);
  }
  ```

  然后客户端通过自己的密钥解密消息 A，得到 Client-TGS 会话密钥：

  ```c
  // 用自己的密钥解密消息A
  Decrypt(userKey, A, len, TGS_Session_Key);
  printf("通过用户密钥解密得到的Client-TGS会话密钥为: %s\n", TGS_Session_Key);
  ```

  然后会接收到消息 B 并存储起来，最后关闭与 AS 服务端的联系：

  ```c
  // 成功接受消息B
  len = recv(clientSocket, B, 100, 0); 
  B[len] = 0;
  
  printf("成功接收消息B! \n");
  
  // 关闭socket
  close(clientSocket);
  ```

- **客户端向 TGS 服务端发送数据**：

  首先需要在客户端生成消息 C，该消息是由 ServiceID 和消息 B 构成的，然后将此消息发送给 TGS 服务端：

  ```c
  // 消息C是由ServiceID和消息B构成的
  sprintf(C, "%s,%s", ServiceID, B);
  
  // 发送消息C
  send(clientSocket, C, strlen(C), 0);
  ```

  等待 TGS 服务端接收消息 C 后，发送由用户 ID 和时间戳构成的消息 D：

  ```c
   sleep(3); // 等待服务器端接收
  
  // 消息D是由ClientID和timestamp经过TGS_Session_Key加密后得到的
  printf("消息D为: %s\n", D);
  printf("正在进行加密发送给TGS服务器消息D...\n");
  sprintf(D, "<%s,%ld>", ClientID, timestamp1);
  lenD = Encrypt(TGS_Session_Key, D, strlen(D), D);
  
  // 发送消息D
  send(clientSocket, D, lenD, 0);
  ```

- **TGS 服务端向客户端发送数据**：

  TGS 服务端首先接收到消息 C，并且读取到其中的服务 ID 和消息 B，并通过服务端密钥对消息 B 进行解密：

  ```c
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
  ```

  然后读取消息 D，通过会话密钥进行解密，得到其中的数据用户 ID，以及时间戳，并且判断用户 ID是否与消息 B中读取到的用户 ID 是否一致，如果不一致也终端通信，并且返回错误信息；或者时间戳过期也需要返回错误信息：

  ```c
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
  ```

  接收完数据后，需要向客户端发送数据。首先发送的是由客户 ID、客户网络地址、票据有效期限、 Client-SS 会话密钥构成的服务票据 ST 和服务 ID 一起构成的消息 E，发送消息之前需要对 ST 进行加密操作：

  ```c
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
  ```

  等待客户端接收后，需要发送消息 F，该消息是由 Client-TGS 会话密钥加密后的 Client-SS 会话密钥：

  ```c
  sleep(3); // 等待客户端接收
  
  // 加密生成消息F,并且发送给客户端
  printf("Client-SS会话密钥为: %s\n", SS_Session_Key);
  int lenF = Encrypt(TGS_Session_Key, SS_Session_Key, strlen(SS_Session_Key), F);
  F[lenF] = 0;
  printf("正在向客户端发送由Client-TGS会话密钥加密得到的消息E...\n");
  send(clientSocket, F, lenF, 0);
  ```

- **客户端接收 TGS 服务端发送的数据**：

  接收消息，首先判断客户 ID 是否一致，以及有效期是否过期，如果二者中有一个不符合要求，则认证失败，中断通信：

  ```c
  // 判断是否认证成功
  if(strcmp(E, "Different ID!") == 0){
    printf("客户ID不一致，认证失败! \n");
    exit(1);
  }
  if (strcmp(E, "Be Overdue!") == 0){
    printf("有效期已过，认证失败! \n");
    exit(1);
  }
  ```

  接收消息 E 和消息 F，并且利用上一轮获取到的 Client-TGS 会话密钥对消息 F 进行解密，得到 Client-SS 会话密钥，然后终止与 TGS 服务端的通信：

  ```c
  // 接收消息E
  lenE = recv(clientSocket, E, 100, 0);
  E[lenE] = 0; 
  // 接收消息F
  lenF = recv(clientSocket, F, 100, 0);
  F[lenF] = 0;
  printf("成功接收消息F! \n");
  
  // 对F进行解密
  Decrypt(TGS_Session_Key, F, lenF, SS_Session_Key);
  ```

- **客户端向 SS 服务端发送数据**：

  首先需要向 SS 服务端发送消息 E，等待服务端接收：

  ```c
   // 发送消息E
  printf("正在向SS服务器发送消息E...\n");
  send(clientSocket, E, lenE, 0);
  
  sleep(3); // 等待服务器端接收
  ```

  然后发送由 Client-SS 会话密钥加密后的消息 G，消息 G 是由用户 ID 和时间戳构成的：

  ```c
  // 发送消息加密后的G
  sprintf(G, "<%s,%ld>", ClientID, timestamp1);
  printf("由用户ID和时间戳构成的消息G为: %s\n", G);
  lenG = Encrypt(SS_Session_Key, G, strlen(G), G);
  printf("正在进行加密发送给SS服务器消息G...\n");
  send(clientSocket, G, lenG, 0);
  ```

- **SS 服务端向客户端发送数据**：

  首先需要接收客户端发来的消息 E，并且读取器中的数据即服务 ID 和 ST：

  ```c
  // 接收消息E
  int lenE = recv(clientSocket, E, 100, 0);
  E[lenE] = 0;
  printf("成功接收消息E! \n");
  
  // 读取消息E的数据
  char ST[100]; char ServiceID[100];
  sscanf(E, "%[^,],%s", ServiceID, ST);
  for (int i = strlen(ServiceID) + 1; i < lenE; ++i) ST[i - (strlen(ServiceID) + 1)] = E[i];
  ST[lenE - (strlen(ServiceID) + 1)] = 0;
  ```

  获取到了 ST 后，需要用 SS 密钥对 ST 进行解密，并且获取到其中的数据，客户 ID、客户网络地址、票据有效期限、 Client-SS 会话密钥：

  ```c
  // 解密ST，并且进行读取数据
  Decrypt(SS_Key, ST, (lenE - strlen(ServiceID) - 1), ST);
  printf("读取消息E并经过SS密钥解密得到的ST为: %s\n", ST);
  sscanf(ST, "<%[^,],%[^,],%ld,%[^>]>", ClientID1, client_net_addr, &timestamp1, SS_Session_Key);
  ```

  然后要接收消息 G，然后需要用 Client-SS 会话密钥对其进行解密，得到其中的客户 ID 与时间戳，并且进行判断客户 ID 是否与之前的一只，如果不一致则会认证失败：

  ```c
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
  ```

  之后会将时间戳 + 1并且和用户 ID 保存到消息 H 中并且通过 Client-SS 会话密钥进行加密，发送给客户端：

  ```c
  // 生成消息H,并且发送回客户端
  timestamp3 = timestamp2 + 1;
  sprintf(H, "%s,%ld", ClientID1, timestamp3);
  printf("由用户ID和时间戳构成的消息H为: %s\n", H);
  printf("正在向客户端发送经Client-SS会话密钥加密得到的消息H...\n");
  int lenH = Encrypt(SS_Session_Key, H, strlen(H), H);
  send(clientSocket, H, lenH, 0);
  ```

- **客户端接收 SS 服务端发送的数据**：

  接收到 SS 服务端发来的消息 H，并且通过 Client-SS 会话密钥进行解密，获取到其中的用户 ID 和时间戳，并且进行判断当前获取到的时间戳和之前的时间戳相差是否为1，如果为1则可以信赖，认证结束；否则不可以信赖，终止认证：

  ```c
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
  ```

## C语言源代码

源代码在压缩包中有，并且文件过长，我就不贴出来了，具体模块讲解上面已经写的很详细了 ^_^

[Github地址](https://github.com/hupf3/inf_security/tree/master/Kerberos)

## 编译运行结果

在运行前，先检查8081，8082，8083端口是否被占用，如果没有被占用则可运行以下命令进行编译代码文件生成代码的可执行文件

`make clean`

`make`

执行后的结果如下所示：

![](./img/6.png)

生成的可执行文件在当前目录下的 `bin	` 路径中，分别存有 `AS`, `TGS`, `SS`,`Client` 可执行文件，分别打开四个终端窗口，并且按照以下的顺序在不同的终端中输入命令即可成功运行本次项目，查看结果：

`./AS`

`./TGS`

`./SS`

`./Client`

实验的运行需要输入用户名和密码，为了方便 TA 测试，我这边提供一组数据进行测试：

- 用户名：`hupengfei`
- 密码：`MYKey1`

所有的数据如下：

```c
c[0].ID = "hupengfei"; c[0].key = "MYKey1";
c[1].ID = "huran"; c[1].key = "MYKey2";
c[2].ID = "chenxiaolin"; c[2].key = "MYKey3";
c[3].ID = "huangrui"; c[3].key = "MYKey4";
c[4].ID = "lizihao"; c[4].key = "MYKey5";
```

运行结果如下所示：

![](./img/2.png)

![](./img/3.png)

![](./img/4.png)

![](./img/5.png)

通过上面的结果可以看出本次认证成功，项目的编写是正确的。

## 验证用例

成功的用例如下所示，只展示客户端的内容，即可知道认证成功：

![](./img/5.png)

失败的案例如输入的用户ID并没有在服务器的数据库中有相关的数据，得到的结果如下所示：

- `AS` 服务器端：

  ![](./img/16.png)

- 客户端：

  ![](./img/17.png)

## 实验总结

通过本次实验了解到了 kerberos 认证的整个过程，并且能够通过 C 语言将其实现出来。本次项目也用到了进程间通信，以及服务器端和客户端建立与沟通的过程，这也是对之前学习内容的一次回顾，也是更加熟悉了这方面的操作。并且在本次实验中了解到了 `sprintf` 和 `sscanf` 函数的用法，两个函数在封装数据和读取数据的应用上都比较方便，在今后的学习中也会多多用两个函数解决问题。