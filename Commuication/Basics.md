[TOC]

# The Basic acknownledge of wireless communication



----

##多址接入技术

​	当把多个用户接入到一个公共的传输媒介实现互相通信的时候，需要给每一个用户的信号赋予不同的特征，用以区分不同的用户，这种技术称为多址技术。与多路复用不同，多址接入技术不需要各路信息集中在一起，而是各自经过调制送到信道上去，以及各自从信道上取下经调制而得到的所需信息。

​	多址接入技术将信号维划分为不同的信道后分配给用户，一般按照时间轴、频率轴或者是码字轴将信号空间的维分割为正交或者是非正交的用户信道。基本类型有：[频分多址](https://baike.baidu.com/item/%E9%A2%91%E5%88%86%E5%A4%9A%E5%9D%80)方式、[时分多址](https://baike.baidu.com/item/%E6%97%B6%E5%88%86%E5%A4%9A%E5%9D%80)方式、[空分多址](https://baike.baidu.com/item/%E7%A9%BA%E5%88%86%E5%A4%9A%E5%9D%80)方式(利用天线阵列或者是其他方式产生的有向天线也能够使得信号空间增加一个角度维，利用这一维来划分信道)、[码分多址](https://baike.baidu.com/item/%E7%A0%81%E5%88%86%E5%A4%9A%E5%9D%80)方式等。

​	在[蜂窝通信](https://baike.baidu.com/item/%E8%9C%82%E7%AA%9D%E9%80%9A%E4%BF%A1)系统中，大部分的移动通信系统都有一个或者是几个基站和若干个移动台，移动台是通过基站和其它移动台进行通信的，基站要同时和许多的移动台进行通信，因而基站通常是多路的，有多个信道，而每一个移动台只供一个用户使用，是单路的。移动通信要依靠无线电波的传播来传输信号，具有大面积覆盖的特点，对网内一个用户发射信号而言，其他用户均可接收到所传播的电波，因此必须对移动台和基站的信息加以区别，使基站能区分是哪个移动台发来的信号，而各移动台又能识别出哪个信号是发给自己的。要解决这个问题，就必须给每个信号赋以不同的特征，这就是多址接入技术要解决的问题。

​	每一代的通信系统都有自己独特的多址接入技术：

- 第一代移动通信系统（1G）主要采用频分多址接入方式（FDMA）：频道划分，频带独享，时间共享。
- 第二代移动通信系统（2G）主要采用时分多址接入方式（TDMA）：时隙划分，时隙独享，频率共享。
- 第三代移动通信系统（3G）主要采用码分多址接入方式（CDMA）：码形划分，时隙、频率共享。
- 第四代通信系统（4G）主要采用正交频分复用多址接入方式（OFDMA）：
- 非正交多址接入方式（NOMA）是下一代移动通信系统（5G）一个热门的技术。

![](http://upload-images.jianshu.io/upload_images/6128001-35b58ee4a5ba6c21.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)

![](http://upload-images.jianshu.io/upload_images/6128001-fdeddf334bc01cac.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)



###频分多址(Frequency Division Multiple Access,FDMA)

​	频分多址利用不同的频带来区分用户，即用户的数据在不同的频带上传输，从而避免用户间信号的互相干扰。第一代移动通信系统采用FDMA作为多址方式。FDMA的原理如下图所示，其中User1，User2，User3，User4，User5和User6分别在频点f1，f2，f3，f4，f5和f6上传输数据。各个频点之间有相应的保护频带，保证每个用户的信号不被其他用户干扰。

![FDMA](http://upload-images.jianshu.io/upload_images/6128001-7aaab4243c7024f6.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)

![FDMA系统频谱管理](http://upload-images.jianshu.io/upload_images/6128001-cb3b9f2b5906d88f.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)





###时分多址(Time Division Multiple Access,TDMA)

​	时分多址利用不同的时隙来区分用户，即用户的数据在不同的时隙上传输，从而避免用户间信号的相互干扰。 第二代移动通系统主要采用TDMA作为多址方式。TDMA的原理如下图所示，其中User1，User2，User3，User4，User5和User6分别在时隙t1，t2，t3，t4，t5和t6上传输数据。各个时隙的时间不会相互重叠，保证每个用户的信号不被其他用户干扰。

![TDMA](http://upload-images.jianshu.io/upload_images/6128001-e9b12a82c47edd51.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)

![](http://upload-images.jianshu.io/upload_images/6128001-b6f44015b92ca53d.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)

GSM系统的帧长为4.6ms(每帧8个时隙)，每一个时隙分配给一个用户。

![](http://upload-images.jianshu.io/upload_images/6128001-856c660b3ae0a767.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)

每一个移动台在上行的帧内只能够按指定的时隙(时间同步严苛)向基站发送信号，为了保证在不同的传播时延情况下，各个移动台到达基站处的信号不会重叠，通常上行时隙内必须有保护间隔，在该间隔内不传送信号。基站按顺序安排在预定的时隙中向各个移动台发送消息。

![](http://upload-images.jianshu.io/upload_images/6128001-d0a01130c7c05ecc.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)

![](http://upload-images.jianshu.io/upload_images/6128001-c7944ed4c25b069d.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)

​	GSM手机是没有[双工器](https://baike.baidu.com/item/%E5%8F%8C%E5%B7%A5%E5%99%A8/4904530?fr=aladdin)的,GSM系统的上行传输和下行传输所用的帧号相同，但是上行帧相对于下行帧来说，在时间上推后三个时隙，这样安排，允许移动台在这3个时隙的时间里进行帧调整以及对收发信机的调谐和转换。



###码分多址(Code Division Multiple Access,CDMA)

​	码分多址利用不同的码字来区分用户，即用户的数据用不用的码字进行加扰，从而避免用户间信号的相互干扰。 单纯的CDMA既不划分频带又不划分时隙，而是让每一个用户使用系统所能够提供的全部频谱，因而CDMA采用扩频技术能够使多个用户在同一时间、同一载频以不同的码序列来实现多路通信。

> 扩频通信(Spread Spectrum:SS)是指扩展频谱的通信，发送端采用扩频码调制，使信号所占用的频带宽度远远大于所传输的信息的带宽，发送的信号通过扩频，将信号能量扩展到很宽的频带上，使得扩频通信的信号带宽与信息带宽之比高达100~1000，属于宽带通信。接收端采用与发送端完全相同的扩频码序列与收到的扩频信号进行相关解调和解扩，恢复所传输的信息数据。
>
> 扩频通信是以香农公式的理论发展起来的一种通信方式：$C=B\log_2(1+\frac{S}{N})=1.44B\ln(1+\frac{S}{N})$。其中C为信道容量，单位为b/s;B为传输的信号频带宽度，单位为Hz；S为信号平均功率，N为噪声平均功率。香农公式表明：频带B和信噪比可以互换；采用扩频信号进行通信的优越性在于用扩展频谱的方法可以换取信噪比上的好处。

**问：**在传统的通信系统当中，我们总是相方设法使得信号所占用的频谱尽量的窄，以充分提高十分宝贵的频率资源的利用率。那么为什么还要用宽频带信号来传输窄带信息呢？

**答：**利用扩频实现码分多址的方式。在传统的FDMA和TDMA之外又提供了一种新的多址方式，这种凡事不需要节约频率资源。

​	第三代移动通信主要采用CDMA作为多址方式。例如：A、B、C、D四个发射端分别采用四种码字，

- A:(-1,-1,-1,+1,+1,-1,+1,+1)
- B:(-1 -1 +1 -1 +1 +1 +1 -1)
- C:(-1 +1 -1 +1 +1 +1 -1 -1)
- D:(-1 +1 -1 -1 -1 -1 +1 -1)

假设A发送数据1，B发送数据0，C不发送数据，D发送数据1，则：

- A•1 =  (-1 -1 -1 +1 +1 -1 +1 +1) 
- B•(-1) =  (1 1 -1 +1 -1 -1 -1 +1)
- D•1  =  (-1 +1 -1 -1 -1 -1 +1 -1) 
- S  =  A•1 + B•(-1) + D•1  =  (-1 +1 -3 +1 -1 -3 +1 +1)   


接收端接收到码字序列S之后，依次与各个码字相乘提取出各个码字上承载的信号。

- S•A=（＋1－1＋3＋1－1＋3＋1＋1）／8=1，   A发送1 
- S•B=（＋1－1－3－1－1－3＋1－1）／8=－1， B发送0 
- S•C=（＋1＋1＋3＋1－1－3－1－1）／8=0，   C无发送 
- S•D=（＋1＋1＋3－1＋1＋3＋1－1）／8=1，   D发送1

在实际的使用中，CDMA经常会和TDMA、FDMA一起使用，这样系统就能够在同时、同频的无线资源上传输多个用户的数据，多个用户的数据依靠不同的码字序列进行区分。

#### 正交Walsh函数

- 同步时，Walsh码是完全正交码(自相关函数为1，互相关函数为0)
- 在非同步的情形下，Walsh码的自相关特性和互相关特性很差。
- Walsh码序列的功率谱分布彼此不均匀，所以不能单独承担扩频任务，通常采用Walsh码与Gold序列的结合。



### 正交频分复用(Orthogonal Frequency Division Multiplex Accsee,OFDMA)

​	正交频分复用是在频分服用的基础上进一步的压缩频带，提高频谱的利用率。如下图所示，用户之间的频带有所交叠，但是每一个用户频带功率最大的那个点其他用户的信号能量都为0，所以在每一个用户频带功率最大值点处，各个用户信号依旧是正交的。

![OFDM](http://upload-images.jianshu.io/upload_images/6128001-40f6df24b8b8b506.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)

​	OFDM相比FDMA提高了频谱利用率。下图即LTE系统的视频资源结构，时间上每个单位叫做一个OFDM符号，频域上每个单位叫做一个子载波。4G LTE系统可以同时利用时域和频域进行区分用户。

![OFDM](http://upload-images.jianshu.io/upload_images/6128001-3790cb4985d6dd62.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)



####OFDMA详解jiehe

​	参考[这篇](http://blog.csdn.net/madongchunqiu/article/details/18614233/)博客。



### 非正交多址(Non-orthogonal Multiple Access,NOMA)

​	NOMA，是5G的一个热门备选技术,和以往的多址接入技术不同，NOMA采用的是非正交的功率域来区分用户。所谓的非正交就是说用户之间的数据可以在同一个时隙，同一个频点上传输，而仅仅依靠功率的不同来区分用户。如下图所示，ser1和User2在同一频域/时域上传输数据，而依靠功率的不同来区分用户。User3和User4之间类似。 

![NOMA](http://upload-images.jianshu.io/upload_images/6128001-8d7fab9f6ce5b8b0.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)

#### NOMA 详解

​	下面我们以两个UE为例详细的介绍NOMA的技术原理。如图所示，UE1位于小区中心，信道条件较好；UE2位于小区边缘，信道条件较差。我们根据UE的信道条件来给UE分配不同的功率，信道条件差的分配更多功率，即UE2分配的功率比UE1多。 

![](http://upload-images.jianshu.io/upload_images/6128001-52ca280022e56dbb.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)

1. 发射端：

假设基站发送给U1的数据为X~1~，发送给UE2的数据为X~2~，功率分配因子为 a。则基站发送的信号为 ：$$S=\sqrt{ax_1}+\sqrt{1-ax_2}$$ 因为UE2位于小区的边缘，所以我们给UE2分配更多的功率，即$0<a<0.5$.

2. 接收端：

**UE2**: 收到的信号为：$$y_2=h_2s+n_2=h2(\sqrt{ax_1}+\sqrt{1-ax_2})+n_2$$.因为UE2的信号$X_2$分配的功率较多，所以UE2可以直接把UE1的信号当做是噪声，直接解调解码UE2的信号即可。

**UE1:** 收到的信号为：$$y_1=h_1s+n_1=h1(\sqrt{ax_1}+\sqrt{1-ax_2})+n_1$$.因为UE1的信号分配较少的功率，所以UE1不能够直接解调解码UE1自己的数据，相反，UE1需要先跟UE2一样解调解码UE2的数据$X_2$，解出$X_2$之后再用$y_1$减去归一化的$X_2$得到UE1自己的数据，$$\hat{y}_1=y_1-h_2\sqrt{1-ax_2}\;$$,最后在解调解码UE1自己的数据。

***以上的推导只是表征NOMA技术的基本原理，推导不是很严谨。***





## 系统容量

- 定义：
  - 在一定的频段内所能提供的最大信道数，或最大用户数，或系统流入话务量总和。
  - 与信道的载频间隔、每载频的时隙数、频率资源和频率复用方式、以及基站的设置方式有关。
- 无线容量m是衡量无线系统频谱效率的参数
  - 载波干扰功率比$C/I$和带宽$B_c$
  - $m=\frac{B_t}{B_cN}$ 。
  - $B_t$系统总频谱，$B_c$信道带宽，N簇的大小。

1. 例1：一个FDMA系统的总频带宽度为1.25$MHz$,频道间的间隔为25$kHz$，那么这个系统的信道总数为：$$\frac{1.25MHz}{25kHz}=50$$, 假设簇内的小区数为7，那么系统的容量为：$$m=\frac{50}{7}=7.1信道/小区$$。

2. 例2：一个TDMA系统的总频带宽度为1.25$MHz$,频道间的间隔为$200kHz$,每个载频的时隙为8，那么这个系统的信道总数为：$$\frac{1.25MHz}{200kHz}*8=50$$, 簇内小区数：4，那么系统的容量为：$$m=\frac{50}{4}=12.5信道/小区$$。

3. 由于CDMA是一个干扰受限的系统，它的频率复用因子可以为1，对其系统容量的分析更为复杂，再次进行简单的推导。

   - $$\frac{C}{I}=\frac{R_b*E_b}{N_0*W}=\frac{E_b}{N_0}*\sqrt{\frac{W}{R_b}}$$ 其中$N_0$为干扰(包括噪声)的功率谱密度；$E_b$为每比特信号能量；$R_b$为信息的比特速度；$W$为总频段宽度。
   - 在单小区时，考虑上行链路的容量，假设理想的功率控制(即在上行链路对所有的移动台的发射功率进行控制，使到达基站接收机的信号功率均相同)，则基站接收机的载波干扰比为：$$\frac{C}{I}=\frac{P_r}{(m-1)P_r}=\frac{1}{m-1}$$。其中$P_r$为基站接收到的每一个移动台的信号功率，m为同时工作的移动台数。那么此时$$\frac{1}{m-1}=\frac{R_b*E_b}{N_0*W}\Longrightarrow m=1+\frac{N_0*W}{R_b*E_b}$$。

   例3：以数字IS-95系统的CDMA为例，假设频道带宽为:$$1.25MHz$$，声码器速率为：$$9.6kbit/s$$，要求$E_b/B_0=7dB  (10^{0.7}=5.01)$；那么$C/I=0.032$，系统容量为：$1+\frac{N_0*W}{R_b*E_b}\Longrightarrow1+\frac{W/R_b}{E_b/N_0}=1+\frac{1.25MHz\div9.6kbit/s}{5.01}\approx26.99用户/小区$




----

##多路复用技术





## Polar Code

[Polar Code](http://marshallcomm.cn/2017/03/01/polar-code-1-summary/)



---

##以太网的传输协议：CSMA/CD 

​	CSMA/CD(Carrier Sense Multiple Access with Collision Detection)