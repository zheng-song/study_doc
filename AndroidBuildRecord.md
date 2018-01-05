[TOC]

# 以下内容基于Android 8.0

## [Android HIDL](http://blog.csdn.net/qidi_huang/article/details/76572404)

在Android8.0当中全面使用全新的HIDL(HAL interface definition language:硬件抽象层接口定义语言)定义,HIDL读作hide-l.在此之前Android有AIDL,架构在Android binder之上,用来定义Android基于Binder通信的Client与Service之间的接口.HIDL也是类似的作用,只不过定义的是Android Framework与Android HAL实现之间的接口. 这样Android可以在不重新编译HAL的情况下对Framework进行OTA升级.

在AIDL机制中Android会提供一系列的工具将用户定义的*.aidl文件编译生成Client端代码与Service段代码,用户需要做的工作是:

1. 在Service端实现所需要实现的接口
2. 在Client端调用相关的接口.

基于Binder机制,在Client端的调用会自动通过binder驱动跨进程到Service进程当中.



而在HIDL当中,与AIDL比较类似,底层也是基于binder机制.但是也有稍微不一样的地方.为了支持HIDL, Android对Binder做了一定的修改.

![各种binder](http://img.blog.csdn.net/20170907000324722?watermark/2/text/aHR0cDovL2Jsb2cuY3Nkbi5uZXQvbGpwMTIwNQ==/font/5a6L5L2T/fontsize/400/fill/I0JBQkFCMA==/dissolve/70/gravity/SouthEast)

![AIDL和HIDL](http://img.blog.csdn.net/20170907001300130?watermark/2/text/aHR0cDovL2Jsb2cuY3Nkbi5uZXQvbGpwMTIwNQ==/font/5a6L5L2T/fontsize/400/fill/I0JBQkFCMA==/dissolve/70/gravity/SouthEast)





HIDL与Android Vendor Test Suite(VTS)测试有着紧密的联系. 使用HIDL描述的HAL描述文件替换旧的用头文件描述的HAL文件的过程称为**HAL的binder化(binderization),所有运行Android O的设备都必须要支持binder化后的HAL模块**.



> **Framework:**  这一层是核心应用程序所使用的API框架，为应用层提供各种接口API,包括各种组件和服务来支持我们的安卓开发，包括ActivityManager,WindowManager,ViewSystem等我们讲到AndroidFramework时很经常要提到的东西。
>
> **OTA升级:** 
>
> **HAL(HardWare Abstract Level):** Android硬件抽象层,是对Linux内核的驱动程序的封装,向上提供接口,屏蔽底层的实现细节.
>
> **[VTS:](http://blog.csdn.net/qidi_huang/article/details/76653677)** 



## [Android Binder](http://blog.csdn.net/boyupeng/article/details/47011383)

Binder是Android系统IPC(Internet Process Connection)方式之一.Linux已经拥有的进程间通信IPC手段包括：Pipe、Signal、Trace、Socket、Message、Share Memory和Semaphore。

### 序

基于client-server的通信方式广泛的应用于从互联网和数据库访问到嵌入式手持设备内部通信等各个领域.智能手机平台特别是Android系统中,为了向应用的开发者提供丰富多样的功能,这种通信方式更是无处不在,诸如媒体播放, 视音频捕获, 各种让手机更智能的传感器(加速度, 方位, 温度, 光亮度等)都是由不同的server负责管理的,应用程序只需要作为Client与这些Server建立链接便可以使用这些服务,花很少的时间和精力就可以开发出令人炫目的功能. Client-Server方式的广泛采用对进程间通信（IPC）机制是一个挑战。

目前linux支持的IPC包括传统的管道，System V IPC，即消息队列/共享内存/信号量，以及socket中只有socket支持Client-Server的通信方式。当然也可以在这些底层机制上架设一套协议来实现Client-Server通信，但这样增加了系统的复杂性，在手机这种条件复杂，资源稀缺的环境下可靠性也难以保证。另一方面是传输性能。socket作为一款通用接口，其传输效率低，开销大，主要用在跨网络的进程间通信和本机上进程间的低速通信。消息队列和管道采用存储-转发方式，即数据先从发送方缓存区拷贝到内核开辟的缓存区中，然后再从内核缓存区拷贝到接收方缓存区，至少有两次拷贝过程。共享内存虽然无需拷贝，但控制复杂，难以使用。

还有一点是出于安全性考虑。终端用户不希望从网上下载的程序在不知情的情况下偷窥隐私数据，连接无线网络，长期操作底层设备导致电池很快耗尽等等。传统IPC没有任何安全措施，完全依赖上层协议来确保。首先传统IPC的接收方无法获得对方进程可靠的UID和PID（用户ID进程ID），从而无法鉴别对方身份。Android为每个安装好的应用程序分配了自己的UID，故进程的UID是鉴别进程身份的重要标志。使用传统IPC只能由用户在数据包里填入UID和PID，但这样不可靠，容易被恶意程序利用。可靠的身份标记只有由IPC机制本身在内核中添加。其次传统IPC访问接入点是开放的，无法建立私有通道。比如命名管道的名称，systemV的键值，socket的ip地址或文件名都是开放的，只要知道这些接入点的程序都可以和对端建立连接，不管怎样都无法阻止恶意程序通过猜测接收方地址获得连接。

基于以上原因，Android需要建立一套新的IPC机制来满足系统对通信方式，传输性能和安全性的要求，这就是Binder。**Binder基于Client-Server通信模式，传输过程只需一次拷贝，为发送发添加UID/PID身份，既支持实名Binder也支持匿名Binder，安全性高。**



### 面向对象的Binder IPC

- Binder使用Client-Server通信方式：一个进程作为Server提供诸如视频/音频解码，视频捕获，地址本查询，网络连接等服务；多个进程作为Client向Server发起服务请求，获得所需要的服务。要想实现Client-Server通信据必须实现以下两点：一是server必须有确定的访问接入点或者说地址来接受Client的请求，并且Client可以通过某种途径获知Server的地址；二是制定Command-Reply协议来传输数据。例如在网络通信中Server的访问接入点就是Server主机的IP地址+端口号，传输协议为TCP协议。**对Binder而言，Binder可以看成Server提供的实现某个特定服务的访问接入点， Client通过这个‘地址’向Server发送请求来使用该服务；对Client而言，Binder可以看成是通向Server的管道入口，要想和某个Server通信首先必须建立这个管道并获得管道入口。**

- 与其它IPC不同，Binder使用了面向对象的思想来描述作为访问接入点的Binder及其在Client中的入口：Binder是一个实体位于Server中的对象，该对象提供了一套方法用以实现对服务的请求，就象类的成员函数。遍布于client中的入口可以看成指向这个binder对象的‘指针’，一旦获得了这个‘指针’就可以调用该对象的方法访问server。在Client看来，通过Binder‘指针’调用其提供的方法和通过指针调用其它任何本地对象的方法并无区别，尽管前者的实体位于远端Server中，而后者实体位于本地内存中。‘指针’是C++的术语，而更通常的说法是引用，即Client通过Binder的引用访问Server。而软件领域另一个术语‘句柄’也可以用来表述Binder在Client中的存在方式。从通信的角度看，Client中的Binder也可以看作是Server Binder的‘代理’，在本地代表远端Server为Client提供服务。本文中会使用‘引用’或‘句柄’这个两广泛使用的术语。

- 面向对象思想的引入将进程间通信转化为通过对某个Binder对象的引用调用该对象的方法，而其独特之处在于Binder对象是一个可以跨进程引用的对象，它的实体位于一个进程中，而它的引用却遍布于系统的各个进程之中。最诱人的是，这个引用和java里引用一样既可以是强类型，也可以是弱类型，而且可以从一个进程传给其它进程，让大家都能访问同一Server，就象将一个对象或引用赋值给另一个引用一样。Binder模糊了进程边界，淡化了进程间通信过程，整个系统仿佛运行于同一个面向对象的程序之中。形形色色的Binder对象以及星罗棋布的引用仿佛粘接各个应用程序的胶水，这也是Binder在英文里的原意。

当然面向对象只是针对应用程序而言，对于Binder驱动和内核其它模块一样使用C语言实现，没有类和对象的概念。Binder驱动为面向对象的进程间通信提供底层支持。



## [Android  OTA升级原理和流程分析](http://blog.csdn.net/ylyuanlu/article/details/44457557)



##[Andorid CTS 测试](http://blog.csdn.net/demo_006/article/details/51167497)

CTS全称Compatibility Test Suite(兼容性测试),CTS的目的就是让各种Android设备(如手机)开发商能够开发出兼容性更好的Android设备, Google制定了CDD(Compatibility Defination Document)规范,为了达到验证CDD规范的目的,提供了一组CASE给不同平台厂商进行验证，Android设备只有满足CDD的规定并且通过CTS，才能获得Android的商标和享受Android Market的权限，才能使用Android Market ，其中包括了CTS,GTS和CtsVerifier三项测试。

CTS测试相关资料下载链接（需自备梯子）：<http://source.android.com/compatibility/downloads.html>



## [Android Vendor Test Suite (VTS) 的概念、作用及测试方法](http://blog.csdn.net/qidi_huang/article/details/76653677)

Android 8.0之前前有一个比较明显的缺点是设备升级到新的版本系统所要花费的时间太长,通常是在Google发布了新版本的AOSP之后,还需要SoC厂商对HAL进行升级,以及OEM厂商进行HAL和Framework进行升级之后,用户才能够在设备上收到OTA升级包的推送.低端一点的产品甚至在出厂后就不会再进行系统升级了。用户对此抱怨良多。

**为了解决这个问题，于是 Google 发起了 Project Treble 项目。**2017 年 5 月 12 日，官方在**Developers Blog**上向公众介绍了这一项目并宣布 Android 8.0 中将引入它，**Project Treble 中最重要的就是新增了 Vendor Interface 这一概念，以及相应的 Vendor Test Suite (VTS) 测试。** 



### VTS 的概念以及作用

VTS 全称是 Vendor Test Suite，官方在介绍它时将其与 CTS 进行了类比，原文是：

> Project Treble aims to do what CTS did for apps, for the Android OS framework. The core concept is to separate the vendor implementation---- the device-specific,lower-level software written in large part by silicon manufacturers----from the Android OS Framework.
>
> This is achieved by the introduction of a new vendor interface between the Android OS framework and the vendor implementation. The new vendor interface is validated by a Vendor Test Suite (VTS), analogous to the CTS, to ensure forward compatibility of the vendor implementation.

**VTS 类似 CTS，通过对 Vendor Interface 进行测试，确保同一个版本的 Android Framework 可以运行在不同 HAL 上，或不同 Android Framework 可以运行在 同一个 HAL 上。** 

**通过这样的 Framework / HAL 分离设计和接口一致性保证，也使得 8.0 版本之后的 Android 系统在进行升级时，可以直接对 Framework 进行升级而不用考虑 HAL 层的改动，从而缩短了用户手上设备得到系统升级 OTA 推送的时间。** 

![vts_framework](http://img.blog.csdn.net/20170804090849559?watermark/2/text/aHR0cDovL2Jsb2cuY3Nkbi5uZXQvUWlkaV9IdWFuZw==/font/5a6L5L2T/fontsize/400/fill/I0JBQkFCMA==/dissolve/70/gravity/SouthEast)

这样在Android8.0采用新的架构之后,在Android系统的升级过程是可以直接对Framework进行替换的,如下图:

![update_with_treble](http://img.blog.csdn.net/20170804090922379?watermark/2/text/aHR0cDovL2Jsb2cuY3Nkbi5uZXQvUWlkaV9IdWFuZw==/font/5a6L5L2T/fontsize/400/fill/I0JBQkFCMA==/dissolve/70/gravity/SouthEast)

### 对工程师的影响

这样的架构变动对于 Android 设备用户的影响是他们今后可以得到更及时的升级服务，对于 Android BSP 工程师来说就是要为实现这样的服务铺设好平台基础，主要是以下几方面工作：

1. 将现有 Android Framework 中耦合的 HAL 代码剥离出来
2. 使用 HIDL 描述的 HAL （.hal文件）替换旧的头文件描述的 HAL
3. 根据接口描述实现各模块 HAL
4. 在 makefile 中为 .hal 文件添加声明 


5. 添加相应的 SEpolicy 配置

根据文档《HIDLHALVersioningandExtensions.pdf》的描述，Android 8.0 及之后版本的系统仅支持经过 binder 化（binderized）的 HAL，因此老版本的 HAL 必须被全部替换掉。

庆幸的是，我们可以使用 `c2hal` 工具将那些在头文件中定义的老的 HAL 接口转换为使用 HIDL 描述的 `.hal` 文件，这会大大减少我们手动进行添加的工作量，甚至有时我们会发现这些基本工作有的已经由 Google 的工程师帮我们完成了，我们只需要根据这些 `.hal` 文件中的 Vendor Interface 接口定义和数据声明来实现这些接口就好。 

要使用 `c2hal` 工具，我们需要先执行下面的命令来生成她：

`make c2hal`

然后就可以使用这个工具进行自动转换了。比如我们要为 NFC HAL 生成相应的 `.hal` 文件，那么可以执行下方的命令：

```
c2hal -r android.hardware:hardware/interfaces -r android.
hidl:system/libhidl/transport -p android.hardware.nfc@1.0 hardware/libhardware/include/hardware/nfc.h
```





## Android HAL

HAL(Hardware Abstract Layer)即Android 硬件抽象层,是对Linux内核驱动程序的封装，向上提供接口，屏蔽低层的实现细节。Android把对硬件的支持分成了两层，一层放在用户空间（User Space），一层放在内核空间（Kernel Space），其中，硬件抽象层运行在用户空间，而Linux内核驱动程序运行在内核空间。

> **为什么要这样安排呢？把硬件抽象层和内核驱动整合在一起放在内核空间不可行吗？**
>
> 从技术实现的角度来看，是可以的，然而从商业的角度来看，把对硬件的支持逻辑都放在内核空间，可能会损害厂家的利益。我们知道，Linux内核源代码版权遵循GNU License，而Android源代码版权遵循Apache License，前者在发布产品时，必须公布源代码，而后者无须发布源代码。如果把对硬件支持的所有代码都放在Linux驱动层，那就意味着发布时要公开驱动程序的源代码，而公开源代码就意味着把硬件的相关参数和实现都公开了，在手机市场竞争激烈的今天，这对厂家来说，损害是非常大的。因此，Android才会想到把对硬件的支持分成硬件抽象层和内核驱动层，内核驱动层只提供简单的访问硬件逻辑，例如读写硬件寄存器的通道，至于从硬件中读到了什么值或者写了什么值到硬件中的逻辑，都放在硬件抽象层中去了，这样就可以把商业秘密隐藏起来了。也正是由于这个分层的原因，Android被踢出了Linux内核主线代码树中。大家想想，Android放在内核空间的驱动程序对硬件的支持是不完整的，把Linux内核移植到别的机器上去时，由于缺乏硬件抽象层的支持，硬件就完全不能用了，这也是为什么说Android是开放系统而不是开源系统的原因。



学习Android的HAL，对理解整个Android系统，都是极其有用的，因为它从下到上涉及到了Android系统的硬件驱动层、硬件抽象层、运行时库和应用程序框架层等等，下面这个图阐述了硬件抽象层在Android系统中的位置，以及它和其它层的关系：

![img](http://hi.csdn.net/attachment/201106/25/0_1308977488PkP8.gif)

在学习Android硬件抽象层的过程中，我们将会学习如何在内核空间编写硬件驱动程序、如何在硬件抽象层中添加接口支持访问硬件、如何在系统启动时提供硬件访问服务以及 如何编写JNI使得可以通过Java接口来访问硬件，而作为中间的一个小插曲，我们还将学习一下如何在Android系统中添加一个C可执行程序来访问硬件驱动程序。由于这是一个系统的学习过程，可以分成六篇文章来描述每一个学习过程，转载如下:

1. [在Android内核源代码工程中编写硬件驱动程序](http://blog.csdn.net/luoshengyang/article/details/6568411)。
2. [在Android系统中增加C可执行程序来访问硬件驱动程序](http://blog.csdn.net/luoshengyang/article/details/6571210)。
3. [在Android硬件抽象层增加接口模块访问硬件驱动程序](http://blog.csdn.net/luoshengyang/article/details/6573809)。
4. [在Android系统中编写JNI方法在应用程序框架层提供Java接口访问硬件](http://blog.csdn.net/luoshengyang/article/details/6575988)。
5. [在Android系统的应用程序框架层增加硬件服务接口](http://blog.csdn.net/luoshengyang/article/details/6578352)。
6. [在Android系统中编写APP通过应用程序框架层访问硬件服务](http://blog.csdn.net/luoshengyang/article/details/6580267)。



## Android 层次结构

![Android层次结构](http://img2.tgbusdata.cn/v2/thumb/jpg/MUU5NCw1ODAsMTAwLDQsMywxLC0xLDAscms1MCw2MS4xNTIuMjQyLjEx/u/android.tgbus.com/help/UploadFiles_4576/201108/2011081710043460.jpg)



###  应用程序层

这一层一般我们最为熟悉了，我们平常开发安卓用java编写的app都属于这一层，当然系统自带的程序比如时钟啊，email等都是属于这一层.



### 应用程序框架层Framework

这一层是核心应用程序所使用的API框架，为应用层提供各种接口API,包括各种组件和服务来支持我们的安卓开发，包括ActivityManager,WindowManager,ViewSystem等我们讲到AndroidFramework时很经常要提到的东西。

#### 服务端

1. ActivityManagerService(AMS): 管理所有应用程序中的Activity,他掌握所有的Activity的情况, 具有调度Activity生命周期的能力,简而言之,他就是管理和掌控所有的Activity
2. WindowManagerService(Wms):控制窗口的显示与隐藏以及窗口之间的层次关系,简言之,他是用来管理窗口的,可以知道大多数和View有关系的都要和他打交道.
3. KeyQ类: 它是Wms的一个内部类,一旦创建就会启动一个线程,这个线程会不断的接收读取用户的UI操作消息,并把这些消息放到消息队列QueueEvent中.
4. InputDispatcherThread类: 该类也是一旦创建就会启动一个线程,这个线程会不断的从上面的QueueEvent中取出用户的消息,进行一定的过滤,再将这些消息发送给当前活动的客户端程序当中.

#### 客户端

1. ActivityThread类: 主线程类,即UI线程类,我们程序的入口就是从他的`main()`函数进入的, 也是客户端与AMS交互的一个最主要的类:内部有ApplicationThread类(继承IBinder)负责与AMS跨进程通讯, 又有H类(继承自Handler类), 负责接收ApplicationThread发来的消息,实现把消息传到主线程.
2. ViewRoot类：很重要的一个类，负责客户端与WMS的交互：内部有W类，W类继承于Binder，所以他与ApplicationThread的角色是差不多的，只不过他对应的是Wms，当Wms想与客户端进行通信的时候，Wms就调用这个类。内部又有ViewRootHandler类继承于Handler，所以他能在W类接收到Wms的消息后，把这个消息传送到UI线程中。同时界面绘制的发起点也是在这里面：performTraversals()。
3. Activity类：这个我们再熟悉不过了，APK运行的最小单位。
4. PhoneWindow类：继承自Window类，它里面会放一个DecorView，它提供了一组统一窗口操作的API.
5. DecorView类：这是我们所能看到的View的所有，它继承自FrameLayout，我们写的布局view就是放在他这个里面。
6. ApplicationThread类:继承于Binder,当Ams想与客户端通信时（即调用客户端的方法），Ams调用的就是这个类。
7. Instrumentation类：负责直接与Ams对话，比如当客户端想与Ams进行通信时（即调用Ams服务里的方法），都是他去实现单项调用Ams，即他相当于于一个管家，所有想调Ams的操作都集中到他这儿，他负责单向调用Ams.

#### Manager 机制

服务端有很多各种各样的系统服务，当我们客户端每次想调用这些服务时（IPC）如果每次都是想要哪一个服务就直接去调用哪一个服务的话，显然显得比较杂乱且拓展性较差，所以安卓采用了这种Manager机制，即设置一个类似经理的东西，也就是Manager,他本身也是一个服务，他管理着所有其他的服务，也就是说我们需要哪个服务要先经过他，他负责为我们去调用这个服务，所以这样就只给我们暴露一个经理这个服务，其他的服务被他屏蔽了，对我们来说是透明的，这和java的封装很像。

#### Android Framework的三大核心功能

1. **View.java：  **View工作原理，实现包括绘制view、处理触摸、按键事件等。
2. **ActivityManagerService.java ：Ams  **管理所有应用程序的Activity 等 。
3. **WindowManagerService.java :Wms   **为所有应用程序分配窗口，并管理这些窗口。

因为每一个Binder都会对应一个线程，所以一个含Activity的程序**最少也有3个线程**，分别是：1.UI主线程   2.ApplicationThread(Binder)     3.ViewRoot.W(Binder).



### 系统运行库层

从上图可以看出,这一层有两个部分:

1. 程序库

Android 包含一些C/C++库，这些库能被Android系统中不同的组件使用。它们通过 Android 应用程序框架为开发者提供服务。以下是一些核心库：

- 系统 C 库 - 一个从 BSD 继承来的标准 C 系统函数库（ libc ）， 它是专门为基于 embedded linux 的设备定制的。
- 媒体库 - 基于 PacketVideo OpenCORE；该库支持多种常用的音频、视频格式回放和录制，同时支持静态图像文件。编码格式包括MPEG4, H.264, MP3, AAC, AMR, JPG, PNG 。
- Surface Manager - 对显示子系统的管理，并且为多个应用程序提 供了2D和3D图层的无缝融合。
- LibWebCore - 一个最新的web浏览器引擎用，支持Android浏览器和一个可嵌入的web视图。
- SGL - 底层的2D图形引擎
- 3D libraries - 基于OpenGL ES 1.0 APIs实现；该库可以使用硬件 3D加速（如果可用）或者使用高度优化的3D软加速。
- FreeType -位图（bitmap）和矢量（vector）字体显示。
- SQLite - 一个对于所有应用程序可用，功能强劲的轻型关系型数据库引擎。

2. Android 运行库(Android Runtime)

Android 包括了一个核心库，该核心库提供了JAVA编程语言核心库的大多数功能。

每一个Android应用程序都在它自己的进程中运行，都拥有一个独立的Dalvik虚拟机实例。Dalvik被设计成一个设备可以同时高效地运行多个虚拟系统。 Dalvik虚拟机执行（.dex）的Dalvik可执行文件，该格式文件针对小内存使用做了优化。同时虚拟机是基于寄存器的，所有的类都经由JAVA编译器编译，然后通过SDK中 的 "dx" 工具转化成.dex格式由虚拟机执行。Dalvik虚拟机依赖于linux内核的一些功能，比如线程机制和底层内存管理机制。



### Linux 内核层

安卓系统基于Linux内核这个我们都知道，Android 的核心系统服务依赖于Linux内核(具体使用哪个版本的Linux内核依android版本的不同而不同)，如安全性，内存管理，进程管理， 网络协议栈和驱动模型。 Linux 内核也同时作为硬件和软件栈之间的抽象层。



