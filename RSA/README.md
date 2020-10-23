[toc]

# RSA 算法实验报告

## 前言

本次实验是基于 C 语言实现的，大整数的运算我是使用了第三方库 `GMP` 实现的，`GMP` 的下载和环境配置过程，下方我会进行简单的描述。

## 代码运行说明

直接进入 RSA 目录下使用下列的命令生成可执行文件 `main`

`gcc ./src/*.c -lgmp -Iinclude -o main`

输入以上指令会生成可执行文件 `main`，然后继续输入命令

`./main`

即可执行本程序

如果 TA 电脑中没有 gmp 库相关文件，我直接将我代码的可执行文件放到目录中，直接打开即可

## Github 地址

由于代码文件的打包，和操作系统的不同，可能压缩文件中代码的中文符号会出现乱码，所以在此放出本次项目的 github 地址，里面保存了本次实验的代码：

[传送门](https://github.com/hupf3/inf_security/tree/master/RSA)

### GMP 环境配置

[GMP 官网](https://gmplib.org/#DOWNLOAD)

首先到 GMP 官网中下载最新的库文件

<img src="https://img-blog.csdnimg.cn/20201023110414685.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L3FxXzQzMjY3Nzcz,size_16,color_FFFFFF,t_70#pic_center" alt="在这里插入图片描述" style="zoom:50%;" />

下载完压缩包后，放到本地进行压缩

解压后到在该文件夹下进行编译：

`./configure --enable-cxx`

`make`

如果电脑没有 m4 的话可以用以下命令进行安装：

`brew install m4` 或者 `sudo apt-get install m4`

编译后进行检测

`make check`

然后对库进行安装(默认路径为/usr/local)

`sudo make install`

### GMP 使用

库函数的使用说明文档：[传送门](https://gmplib.org/manual/Integer-Functions#Integer-Functions)

使用 gcc 命令时要进行一定的修改

`gcc main.c -lgmp`

## 实验环境

**操作系统**：MAC OS

## 原理描述

本实验的设计流程是根据老师给的算法白皮书和课件进行实现的，流程的详细说明如下

### 大素数 p 和 q 的选择

**选择流程**：

- 确定 RSA 所要求 $N$ 的位数 $k$。 $k = 1024、2048、3072、4096 ... $

- 随机选择一个位数为 $(k + 1)/2$ 的素数 $p$。
  - 即 $p \in [2^{(k+1)/2-1}, 2^{(k+1)/2}-1]$.

- 选择一个位数为$k - (k + 1)/2 = (k - 1)/2$ 的素数 $q$。

- 求 $|p - q|$;如果 $log|p - q|$ 过小，则返回 (2)，重新选取 $p$。

- 计算 $N = pq$，确认 N 的位数为 k ($N\in2^{k-1},2^k-1$);否则返回 (2)，重新选取 p。

- 计算 $N$ 的 NAF 权重;如果权重过小，则返回 (2)，重新选取 p。
- 计算 $\phi(N)$，选择公钥指数 $e，2^{16} < e < \phi(N)$ 且 $gcd(e,  \phi(N)) = 1$。
  - PKCS#1 建议选择素数 $ e = 2^{16}+1 = 65537$。
- 求 $e$ 的模  $\phi$(N) 逆元 $d$ 作为私钥指数;如果 $d$ 过小，则返回 (2)，重新选取 $p$。

- $p$ 和 $q$ 选择成功，销毁 $p、q$，返回参数$N, e, d$。

**选择标准**：同时满足 $log|p - q|> k/2-100$ 以及 $log|p - q|> k/3$。

### 密钥生成

- 选择两个不同的大素数 $p$ 和 $q$，计算 $N = pq$。

- 引用欧拉 $\phi$ 函数，小于 $N$ 且与 $N$ 互素的正整数个数为

  $\phi(N) = \phi(pq) = \phi(p)\phi(q) = (p-1)(q-1)$.

- 选择一个整数 $e，1< e < \phi(N)$ 且 $gcd(e, \phi(N)) =1$.

- 求一个满足下列同余式的 $d$: $ed \equiv 1 (mod \phi(N))$.

  - $d$ 是 $e$ 的模 $\phi(N)$ 逆元，可以应用扩展欧几里德算法得到。
  - $d$ 必须是一个足够大的正整数。

- 将 $p$ 和 $q$ 的记录销毁(当 $N$ 足够大时，寻找 $p, q$ 将极其困难)

- 以 $(e, N)$ 作为公钥，$(d, N)$ 作为私钥。

### 加密运算

**输入**：

- $(n,e)$作为接收方的 RSA 公钥 (k 表示合数模 n 的以八位组为计量单位的长度)
- M 待加密的信息，是一个长度为 mLen 的八位组串，其中 $mLen \leq k - 11$

**输出**：C 密文，是一个长度为 k 的八位组串

**出错提示**：“消息太长”

**步骤**：

- 长度检查：如果 mLen > k - 11，输出 “消息太长” 然后终止运算

- EME-PKCS1-v1_5 编码得到编码信息 EM，具体过程在下面介绍

- 将编码信息 EM 转换成一个整数消息代表 m ： m = OS2IP(EM)

  **OS2IP**

  - OS2IP 将一个八位组串转换成一个非负整数
  - OS2IP(X)，输入为 X 待转换的八位组串，输出为 x 相应的非负整数
  - 步骤：
    - 使 $X_1X2...X_{xLen}$ 分别为 X 的第一个至最后一个八位组，使 $x_{xLen-i}$ 的值为八位组 $X_i$ 的整数值，$1\leq i\leq xLen$
    - 让 $x=x_{xLen-1}256^{xLen-1}+x_{xLen-2}256^{xLen-2}+...+x_{1}256^+x_0$
    - 输出 x

- 将 RSA 公钥(n, e) 和消息代表 m 代入 RSAEP 加密原语，从而产生一个饿密文代表 c：c = RSAEP((n, e), m)

  **RSAEP((n, e), m)** 

  - 输入：(n, e) RSA 公钥；m 消息代表，是一个位于 0~n-1 之间的整数
  - 输出：c 密文代表，是一个位于 0~n-1 之间的整数
  - 错误提示：“消息代表超出范围”
  - 假设：RSA 公钥 (n, e) 有效
  - 步骤：
    - 如果消息代表 m 不再 0~n - 1之间，输出消息代表超出范围并终止
    - 让 $c=m^emodn$
    - 输出 c

- 将密文代表 c 转换成一个长度为 k 个八位组的密文代表 C ：C = I2OSP(c, k)

  **I2OSP(x, xLen)**

  - I2OSP把一个非负整数转换为一个长度指定的字节穿

  - 输入：x 待转换的非负整数，xLen 转换后的八位组串的期望长度

  - 输出： X 对应的长度为 xLen 的八位组串

  - 错误信息：“整数太大”

  - 步骤

    - 如果 $x\ge256^{xLen}$，输出“整数太大” 然后终止

    - 用以 256 为基数的 xLen 位数表示整数 x：

      $x=x_{xLen-1}256^{xLen-1}+x_{xLen-2}256^{xLen-2}+...+x_{1}256^+x_0$

      其中 $0\le x_i\le 256$（注意如果 x 小于 $256^{xLen-1}$，一个或多个高位将为零）

    - 使字节 $X_i$ 的整数值为 $s_{xLen-1}$，$1\le I \le xLen$。输出八位组串

      $X=X_1X_2...X_{xLen}$

- 输出密文 C

### 解密运算

**输入**：K 接收方的 RSA私钥；C 待解密的密文，是一个长度为 k 的八位组串，其中 k 是 RSA 合数模 n 的以八位组为计量单位的长度

**输出**： M 消息，是一个长度至少为 k - 11 的八位组串

**出错提示**：“解密出错”

**步骤**：

- 长度检查：如果密文 C 的长度不是 k 个八位组(或者如果 k < 11)，则输出解密错误并终止运算

- 将密文 C 转换成一个整数密文代表 c：c = OS2IP(C) 此函数的介绍在加密中有

- 将 RSA 私钥(n, d) 和密文代表 c 代入 RSADP 解密原语，进而产生一个整数消息代表 m：m = RSADP((n, d), c)。

  **RSADP((n, d), c)**

  - 输入： K RSA的私钥，其中 K 采用两种形式之一，本实验采用的方式是一对(n, d)；c 密文代表，是一个位于 0~n-1之间的整数
  - 输出： m 消息代表，是一个位于 0~n-1之间的整数
  - 出错提示：“密文代表超出范围”
  - 假设：RSA 私钥 K 有效
  - 步骤：
    - $m = c^dmodn$
    - 输出 m

  如果 RSADP 输出 “密文代表超出范围” （意思是 $c\ge n$），则输出“解密出	错”然后中止运算

- 将消息代表 m 转换成一个长度为 k 个八位组串的编码消息 EM：EM = I2OSP(m, k)，函数的介绍在加密中有提及

- EME-PKCS1_v1_5 解码，在解编码中会详细介绍

- 输出 M

### 编解码

#### EME-PKCS1-v1_5 编码

- 生成一个长度为 $k-mLen-3$且由伪随机生成的非零八位组构成的八位组串 PS。PS的长度至少为八个八位组。

- 连接 PS、消息 M 和其他填充，从而形成一个长度为 k 个八位组的编码消息 EM

  $EM=0x00||0x02||PS||0x00||M$

#### EME-PKCS1-v1_5 解码

- 将编码消息 EM 分离成一个八位组串 PS (由非零八位组构成) 和一个消息 M，使满足

  $EM=0x00||0x02||PS||0x00||M$

  如果 EM 的第一个八位组的十六进制值不为0x00，如果 EM 的第二个八位组的十六进制值不为0x02，如果没有十六进制值为0x00的八位组可以从 M 分离出 PS ,或者如果 PS 的长度小于8个八位组，输出“解密出错”并且中止运算。

## 实验文件结构

实验文件的结构如下所示：

<img src="https://img-blog.csdnimg.cn/20201023110430349.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L3FxXzQzMjY3Nzcz,size_16,color_FFFFFF,t_70#pic_center" alt="在这里插入图片描述" style="zoom:33%;" />

可以看到在 RSA 目录下有 include 目录用于包含所有的头文件的代码，src 目录用于包含所有 .c 文件的代码，main 是 gcc 命令生成的可执行文件，README.md 是本次实验的说明文档

## 数据结构设计

### GMP 库 mpz_t 结构

第一个用到的结构就是 GMP 库中的大整数结构 mpz_t，此结构可以很方便的对大整数进行操作，并且执行的时间也是非常的快，对本次实验的完成起到了很大的帮助，下面列举一些本次实验设计到的的函数

- `mpz_init(a)`：对 a 进行初始化
- `mpz_clear(a)`：对 a 进行销毁
- `mpz_set_ui(a, b)`：用 b 对 a 进行赋值
- `mpz_cmp(a, b)`：比较 a 和 b 的大小
- `mpz_pow_ui(a, b, c)`：将 $b^c$ 的结果赋予 a
- `mpz_sizeinbase(a, b)` ：求出 a 在 b 进制下的位数
- `mpz_sub_ui(a, b, c)`：将 b - c 的结果赋予 a
- `mpz_mul(a, b, c)`：将 b * c 的结果赋予 a
- `mpz_invert(a, b, c)`：a 是 b 的模 c 逆元
- `mpz_powm(a, b, c，d)` ：将 $b^cmod$ d的结果赋予 a

### Octetstring 结构体

对 Octetstring 结构体的设计，我放在了 `Octetstring.h` 和 `Octetstring.c `文件中。

此结构体是对字节串的定义，也就是为了实现 EME-PKCS1-v1_5 编解码的过程：

```c
// 字节串的定义
typedef struct Octetstring{
    int *os;
    int len;
}OS;
```

在这个结构体中，我设计了两个成员变量，一个是字符数组（设置成了指针类型），一个是记录该字符数组的长度。字符数组中的每个值用来记录每个八元组的值。（我最开始设置的是字符 char 类型数组，但是在生成 PS 的时候，进行添加操作，会产生添加负数的情况，可能是因为 char 类型的 8 位是从 -128 ～ 127）。

对于该结构体我定义了一些函数，方便对于字节串进行操作：

- `void OSinit(OS *str);`：此函数用来初始化结构体中的成员变量，对于 int 类型的指针数组需要进行 malloc 申请内存，对于 len 直接设置成 0 即可

  ```c
  // 初始化
  void OSinit(OS *str){
      str->len = 0;
      str->os = (int *) malloc(130 * sizeof(int));
  }
  ```

- `void add_int(OS *str, int t);`：在指针数组后面插入一个新的 int 类型的数值，代表插入了一个八元组，并且长度加一

  ```c
  // 通过 int 类型加入到字节组中
  void add_int(OS *str, int t){
      str->os[str->len] = t;
      str->len ++;
  }
  ```

- `void add_char(OS *str, char c);`：在指针数组后面插入一个 char 类型的数值，并不是插入一个字符，而是插入这个字符所代表的 int 类型的数值，此函数实现的意义是将明文 M 转化成数字插入到字节串中

  ```c
  // 通过 char 类型加入到字节组中
  void add_char(OS *str, char c){
      str->os[str->len] = (int)c;
      str->len ++;
  }
  ```

  

- `void OS_inverse(OS *str);`：反转整个字节串，因为后续有需要进行反转的操作，所以在此实现一个函数进行调用，该函数还需要调用一个 `swap` 函数，在该代码文件下也有定义：

  ```c
  void swap(int *a, int *b){
      int t = *a;
      *a = *b;
      *b = t;
  }
  // 反转
  void OS_inverse(OS *str){
      for (int i = 0, j = str->len - 1; i < (str->len) / 2; i++, j--){
          swap(&(str->os[i]), &(str->os[j]));
      }
  }
  ```

  

- `void OS_free(OS *str);`：由于是申请的内存，所以需要释放内存，这里既需要释放结构体本身，也需要释放结构体内的数组指针成员变量

  ```c
  // 释放空间
  void OS_free(OS *str){
      free(str->os);
      free(str);
  }
  ```

## 密钥生成

对于密钥的生成过程，我放在了 `GetKey.h` 头文件 `GetKey.c` 函数实现文件

头文件中我设计了如下几个函数：

- `void getPQ(mpz_t t, int n);`：此函数用来获取随机的大素数 P 和 Q，具体的实现方法使用了 `GMP` 库中的两个函数：`mpz_rrandomb` 用来生成随机数的函数，以及 `mpz_nextprime` 用来生成素数的函数：

  ```c
  // 获取大素数 p 和 q
  void getPQ(mpz_t t, int n){
      mpz_rrandomb(t, state, n);
      mpz_nextprime(t, t);
  }
  ```

- `int checkPQ(mpz_t p, mpz_t q, int k);`：该函数用来检验生成的两个大随机数是否符合条件，同时满足 $log|p - q|> k/2-100$ 以及 $log|p - q|> k/3$。如果满足的话返回1，不满足的话返回0

  ```c
  // 判断 p 和 q 是否符和标准
  int checkPQ(mpz_t p, mpz_t q, int k){
      mpz_t t; mpz_init(t);
      mpz_sub(t, p, q);
      mpz_abs(t, t);
  
      // log|p - q|> k/2-100
      mpz_t t1; mpz_init(t1);
      int e1 = k / 2 - 100;
      mpz_t b1; mpz_init(b1); mpz_set_ui(b1, 2);
      mpz_pow_ui(t1, b1, e1);
      if (mpz_cmp(t, t1) <= 0) return 0;
  
      // log|p - q|> k/3
      mpz_t t2; mpz_init(t2);
      int e2 = k / 3;
      mpz_t b2; mpz_init(b2); mpz_set_ui(b2, 2);
      mpz_pow_ui(t2, b2, e2);
      if (mpz_cmp(t, t2) <= 0) return 0;
  
      mpz_clear(t);
      mpz_clear(t1);
      mpz_clear(b1);
      mpz_clear(t2);
      mpz_clear(b2);
      return 1;
  }
  ```

- `int checkN(mpz_t N, int k);`：此函数用来检验，通过大素数求得的 N 是否符合位数等于 k，如果不满足需要重新选择大素数，函数的实现我使用了函数`mpz_sizeinbase` 此函数是用来计算大整数在某个进制下的位数

  ```c
  // 判断 N 的位数是否为 k
  int checkN(mpz_t N, int k){
      if (mpz_sizeinbase(N, 2) == k) return 1;
      return 0;
  }
  ```

- `void getKey(int k, mpz_t n, mpz_t d, mpz_t e);`：此函数用来获取加密，解密时需要的密钥 d 和 e，以及模底 n，实现的过程需要不断判断选择的 p 和 q 是否满足上面两个判断函数，如果不满足需要重新选取。（此函数的实现我进行了一定的修改，使 p 和 q 的位数进行了 1 位以内的调整，这样选取素数的时候速度会更快一些，如果严格按照 老师课件上的方法实现的话，程序运行的时间会比较长，我这样的改变也能符合 p、q的选择标准，在注释中我也写了课件中的实现方法便于检查）

  ```c
  // 获取密钥
  void getKey(int k, mpz_t n, mpz_t d, mpz_t e){
      // 获取随机值
      gmp_randinit_default(state);
      gmp_randseed_ui(state, time(NULL));
  
      mpz_t phiN; mpz_init(phiN);
      while (1){
          mpz_t p; mpz_init(p);
          mpz_t q; mpz_init(q);
          mpz_t N; mpz_init(N);
  
          getPQ(p, k / 2);
          getPQ(q, k / 2);
          // 按照课件上的方法取p，q，应该是下方注释的方法
          // 但是执行过程较长，所以选择上面的方法
          // 下面的方法也能求解，只不过运行的时候需要等待一定时间
  //      getPQ(p, (int)((k + 1) / 2));
  //      getPQ(q, (int)((k - 1) / 2));
          // mpz_set_str(p, "13407807929942597099574024998205846127479365820592393377723561443721764030073546973268180168610134768523436856700823243770649609554001901502226311745110547", 10);
          // mpz_set_str(q, "13407806331607339337786002530828064473378217277310144333258332203833400701883216701081876796570178855341571855188871186561982435582158525349439211686594501", 10);
          mpz_mul(N, p, q);
  
          if (checkPQ(p, q, k) && checkN(N, k)){
  
              mpz_sub_ui(p, p, 1); mpz_sub_ui(q, q, 1);
              mpz_mul(phiN, p, q);
              mpz_set(n, N);
  
              // gmp_printf("p 为：%Zd\n", p);
              // gmp_printf("q 为：%Zd\n", q);
              // 进行销毁
              mpz_clear(N);
              mpz_clear(p);
              mpz_clear(q);
              break;
          }
  
          // 进行销毁
          mpz_clear(N);
          mpz_clear(p);
          mpz_clear(q);
      }
  
      // PKCS#1 建议
      mpz_set_ui(e, 65537);
  
      // d 是 e 的模 phiN 逆元
      mpz_invert(d, e, phiN);
  
      mpz_clear(phiN);
  }
  ```

## 加密

加密函数主要的实现我放在了 `Encrypt.h` 和 `Encrypt.c` 中，这两个文件写了加密主要实现的过程，由于加密和解密中间的步骤用到的函数会有重叠，所以我将具体步骤实现的函数，放到了另外的文件 `Step.h` 和 `Step.c` 中进行实现。

加密过程中首先会根据编码调用 `getEM()` 得到 EM 字节串，字节串的前两段是字节串 0 和 2，中间字节串 PS 是伪随机串，之所以是伪的，是因为该随机串不能是0，且该随机串不能有重复的一样的数值，然后在 PS 和 M 之间插入0字节串：

```c
// EM = 0x00 || 0x02 || PS || 0x00 || M.
void getEM(OS *str, int PSLen, char *M, int mLen){
    add_int(str, 0); add_int(str, 2);

    // 生成 PS
    srand((unsigned)time(NULL));
    memset(vis, 0, sizeof(vis));
    for (int i = 0; i < PSLen; i++){
        int t = rand() % MAXN + 1; // 不能为0
        while (vis[t] == 1){
            t = rand() % MAXN + 1;
        }
        vis[t] = 1;
        add_int(str, t);
    }

    add_int(str, 0);

    // 加入明文 M
    for (int i = 0; i < mLen; i++) add_char(str, M[i]);
}
```

然后调用 `OS2IP()` 函数来得到整数消息代表 m，该函数就是实现，从字节串转化为10进制的整数消息代表m：

```c
void OS2IP(OS *str, mpz_t m){
    mpz_t base; mpz_init(base);
    mpz_set_ui(base, 256);
    mpz_t tmp; mpz_init(tmp);
    for (int i = str->len - 1, e = 0; i >= 0; i--, e++){
        mpz_pow_ui(tmp, base, e);
        mpz_mul_ui(tmp, tmp, str->os[i]);
        mpz_add(m, m, tmp);
    }

    mpz_clear(base);
    mpz_clear(tmp);
}
```

得到后需要进行判断，如果 $m\ge n$ 则会报错并提示：消息代表超出范围。

```c
 if (mpz_cmp(m, tmp) >= 0){
         printf("消息代表超出范围！\n");
         return 0;
     }
```

然后进行 `RSAEP()` 得到密文代表 c，此过程是通过指数取余的方法得到的密文c，应用了函数 `mpz_powm()`：

```c
void RSAEP(mpz_t n, mpz_t e, mpz_t m, mpz_t c){
    mpz_powm(c, m, e, n);
}
```

最后调用 `I2OSP()` 函数的到最终的密文 C 字节串，该函数用来生成我们最终需要的密文 C，在此之前需要判断一下 c 是否满足条件，如果不满足条件需要打印信息：整数太大；然后就可以利用整数 c 求得字节串 C，求的过程就是反复的利用除法进行求商求余的运算，该过程还需要进行反转密文 C，因为求解的过程是从低往高求的：

```c
int I2OSP(mpz_t c, int k, OS *C){
    mpz_t base; mpz_init(base); mpz_set_ui(base, 256);
    mpz_t tmp; mpz_init(tmp);
    mpz_pow_ui(tmp, base, k);
    if (mpz_cmp(c, tmp) >= 0){
        printf("整数太大！\n");

        mpz_clear(base);
        mpz_clear(tmp);
        return 0;
    }
    mpz_t pre; mpz_init(pre); mpz_set(pre, c);
    while (mpz_cmp_ui(pre, 0) != 0){
//      gmp_printf("%Zd ", base);
//      gmp_printf("%d ", e);
//      gmp_printf("%Zd ", pre);
//      gmp_printf("%Zd\n", tmp);
        mpz_mod(tmp, pre, base);
        mpz_div(pre, pre, base);
        add_int(C, mpz_get_ui(tmp));
//      printf("%d ", mpz_get_ui(tmp));
//      gmp_printf("%Zd ", q);
//      printf("%d ", (int)(q));
    }
    int pad_0 = k - C->len;
    for (int i = 0; i < pad_0; i++) add_int(C, 0);

    OS_inverse(C);
    // printf("\n");
    mpz_clear(base);
    mpz_clear(tmp);
    mpz_clear(pre);
    return 1;
}
```

至此加密过程结束，释放所有内存，销毁所有定义的变量。过程的代码如下：

```c
int Encrypt(mpz_t n, mpz_t e, char *M, int PSLen, int mLen, int k, OS *str){    
		mpz_t m; mpz_init(m); mpz_set_ui(m, 0);

    getEM(str, PSLen, M, mLen);
    printf("# 加密后得到的 EM 字节串为 #：");
    for (int i = 0; i < str->len; i++) printf("%d ", str->os[i]);
    printf("\n\n");
    OS2IP(str, m);
    printf("# 加密后得到的整数消息代表 m 为 #：");
    gmp_printf("%Zd\n", m);
    printf("\n");

    mpz_t tmp; mpz_init(tmp); mpz_set(tmp, n);
    // if (mpz_cmp(m, tmp) >= 0){
    //     printf("消息代表超出范围！\n");
    //     return 0;
    // }

    mpz_t c; mpz_init(c);
    RSAEP(n, e, m, c);
    printf("# 加密后得到的密文代表 c 为 #：");
    gmp_printf("%Zd\n", c);
    printf("\n");
    OS *C; C = (OS *)malloc(sizeof(OS));
    OSinit(C);
    if (I2OSP(c, k, C) == 0){
        OS_free(C);
        mpz_clear(m);
        mpz_clear(c);
        mpz_clear(tmp);
        return 0;
    }
    printf("# 加密后得到的密文 C 字节串为 #：");
    for (int i = 0; i < C->len; i++) printf("%d ", (int)C->os[i]);
    printf("\n");

    OS_free(C);
    mpz_clear(m);
    mpz_clear(c);
    mpz_clear(tmp);
    return 1;
}
```

## 解密

解密的过程我主要在 `Decrypt.h` 和 `Decrypt.c` 文件中进行实现，由于函数的实现和加密的过程会有重叠，所以实现起来比较简单。

解密的过程如下，首先根据密文字节串 C 根据函数 `OS2IP()` 得到密文代表 c，函数的实现在加密的过程中有说明；然后进行判断，如果 $c\ge n$ 则输出提示信息：密文代表超出范围，解密出错，并跳出函数：

```c
if (mpz_cmp(c, n) >= 0){
        printf("密文代表超出范围，解密出错！\n");
        return 0;
    }
```

然后得到了密文 c ，通过函数 `RSADP()` 可以得到整数消息代表 m，该函数也是通过指数取余的方法进行实现：

```c
void RSADP(mpz_t n, mpz_t d, mpz_t c, mpz_t m){
    mpz_powm(m, c, d, n);
}
```

然后需要调用 `I2OSP()` 进行解码，得到 EM 字节串，该函数的实现在加密过程中也有说明，然后取 EM 字节串中的后几个字节串就是需要的解密明文 M，最后将定义的变量空间进行释放。整体的过程如下：

```c
int Decrypt(mpz_t n, mpz_t d, OS *C, int k, int mLen){
    mpz_t c; mpz_init(c);
    OS2IP(C, c);
    // gmp_printf("c : %Zd\n", c);
    if (mpz_cmp(c, n) >= 0){
        printf("密文代表超出范围，解密出错！\n");
        mpz_clear(c);
        return 0;
    }

    mpz_t m; mpz_init(m); mpz_set(m, c);
    // RSADP(n, d, c, m);
    printf("# 解密后得到的整数消息代表 m 为 #：");
    gmp_printf("%Zd\n", m);
    printf("\n");

    OS *EM; EM = (OS *)malloc(sizeof(OS));
    OSinit(EM);
    I2OSP(m, k, EM);
//  printf("%d\n", EM->len);
    printf("# 解密后得到的编码信息 EM 字节串为 #：");
    for (int i = 0; i < EM->len; i++){
        printf("%d ", EM->os[i]);
    }
    printf("\n\n");
    if (EM->os[0] != 0 || EM->os[1] != 2 || EM->os[EM->len - mLen - 1] != 0){
        printf("解密出错！\n");
        mpz_clear(m);
        OS_free(EM);
        mpz_clear(c);
        return 0;
    }

    printf("# 解密后的明文 M 为 #：");
    for (int i = EM->len - mLen; i < EM->len; i++) printf("%c", (char)(EM->os[i]));
    printf("\n");

    OS_free(EM);
    mpz_clear(m);
    mpz_clear(c);
    return 1;
}
```

## 编解码

### EME-PKCS1-v1_5 编码

编码的过程主要是通过名文 M 生成字节串 EM，实现的函数为 `getEM()`，实现的方法在最开始说过，代码如下：

```c
// EM = 0x00 || 0x02 || PS || 0x00 || M.
void getEM(OS *str, int PSLen, char *M, int mLen){
    add_int(str, 0); add_int(str, 2);

    // 生成 PS
    srand((unsigned)time(NULL));
    memset(vis, 0, sizeof(vis));
    for (int i = 0; i < PSLen; i++){
        int t = rand() % MAXN + 1; // 不能为0
        while (vis[t] == 1){
            t = rand() % MAXN + 1;
        }
        vis[t] = 1;
        add_int(str, t);
    }

    add_int(str, 0);

    // 加入明文 M
    for (int i = 0; i < mLen; i++) add_char(str, M[i]);
}
```

### EME-PKCS1-v1_5 解码

解码的过程就是将 EM 进行分解，得到想要的名文 M 的过程，解码过程中需要进行判断解码条件，如果不正确，则代表解密出错：

```c
    if (EM->os[0] != 0 || EM->os[1] != 2 || EM->os[EM->len - mLen - 1] != 0){
        printf("解密出错！\n");
        mpz_clear(m);
        OS_free(EM);
        mpz_clear(c);
        return 0;
    }
```

## 代码运行过程

此函数在运行的过程中首先会提示你输入需要加密的明文，明文是字母和数字的组合，输入明文后，会进行加密操作，然后每一步得到的整数代表以及字节串会显示出来方便检验正确性：

![在这里插入图片描述](https://img-blog.csdnimg.cn/20201023110449256.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L3FxXzQzMjY3Nzcz,size_16,color_FFFFFF,t_70#pic_center)

该程序在加密后得到的字节串 C 会带入到解密的过程中作为输入，解密的过程也会打印出每步得到的整数代表和字节串，可以通过与加密过程得到的信息进行比对，来检验自己实现的是否正确，最后会得到最初输入的名文：

![在这里插入图片描述](https://img-blog.csdnimg.cn/2020102311050488.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L3FxXzQzMjY3Nzcz,size_16,color_FFFFFF,t_70#pic_center)

## 实验心得

本次实验较上次的实验来讲，实现的过程更加的复杂，需要根据算法的白皮书进行一步一步的实现，提高了自己阅读英文文献的能力。因为本次实验的实现是通过 c 来完成的，并且大整数的运算我是调用了第三方 `GMP` 库，让我了解到了，这个第三方库的方便，以及运算效率之高，通过几天的钻研也提升了自己的代码能力和对 RSA 算法的理解程度。