[TOC]

# 以下内容基于Android 8.0

## Project Treble

Project treble是Android O中引入的特性. 主要的目标是Android模块化，根据谷歌、SoC供应商和OEM的需求分离所有权和分配不同的模块。 Vendor 模块被分离到一个独立的可单独更新的image，从而改进了软件升级发布的过程 .

![对比](http://img.blog.csdn.net/20170921175046088?watermark/2/text/aHR0cDovL2Jsb2cuY3Nkbi5uZXQvY2g4NTMxOTk3Njk=/font/5a6L5L2T/fontsize/400/fill/I0JBQkFCMA==/dissolve/70/gravity/SouthEast)

Treble由以下的几个部分组成:

- Kernel modularization
- HAL re-architecture(binderization)
- Separation of the vendor and OEM modules from the system image



## [Android HIDL](http://blog.csdn.net/qidi_huang/article/details/76572404)

在Android8.0当中全面使用全新的HIDL(HAL interface definition language:硬件抽象层接口定义语言)定义.在此之前Android有AIDL,架构在Android binder之上,用来定义Android基于Binder通信的Client与Service之间的接口.HIDL也是类似的作用,只不过定义的是Android Framework与Android HAL实现之间的接口. 这样Android可以在不重新编译HAL的情况下对Framework进行OTA升级.HALs将由供应商或者是SOC制造商构建,并放在设备的vendor下的分区当中.

HIDL的出现是Google为进一步解决Android碎片化的一种努力,它能够进一步的解耦frameworks和HAL层,以后系统的升级无需对HAL重新打包,此为其出现的大背景. 另外它还附带有以下几点优势:

1. 效率更高.HIDL支持两种跨进程通信: **共享内存**和**Fast Message Queue**, 后者其实也是对共享内存的进一步封装
2. HIDL的出现可以让HAL层运行在独立的进程当中,不再以链接的形式被调用,使得线程的运行环境更为清晰明了.
3. 隐藏通信细节. C类型的数据结构向HAL层传递,由于已有的HAL实现大部分是C++,这其中不免需要转换,而HIDL已经将这种转换处理好了,开发者不用担心这种情况.HIDL打头的H正是hide的意思.
4. 用户定义的数据类型可以直接传递. AIDL传递的数据类型必须是parcelable的,但是HIDL可以直接传递用户自定义的struct以及union.



在AIDL机制中Android会提供一系列的工具将用户定义的*.aidl文件编译生成Client端代码与Service端代码,用户需要做的工作是:

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



- HAL interafce definition language(HIDL) 定义了HAL和调用者之间的接口. 允许指定的类型和方法调用, 这些都被集成到接口和包当中. 更广泛的讲, HIDL是一个在独立编译的库函数(codebases)之间进行通信的系统.
- HIDL被用于进程间通信(IPC), 进程间通信被Binder化. 对于必须要连接到一个进程的库, 可以使用直通模式(在JAVA中不支持).
- HIDL指定数据结构和方法签名(methods signatures), 在接口当中进行组织并被集成到包当中. HIDL的语法类似于C++和Java,但是关键字集不同. HIDL使用Java风格的注释(annotation).

**HIDL 的实现被注册为一个服务**

HIDL的接口服务能够通过`registerAsService("another_foo_service")`被注册为一个命名服务(named services)

**查找服务**

客户端的请求代码通过名字和版本来给出一个接口,以此来在想要的HAL类当中调用getService

`sp<V1_1::IFooService> alternateService = 1_1::IFooService::getService("another_foo_service")`;



Android 原本已经以HAL的形式定义了许多接口, 这些接口以C头文件的形式定义在hardware/libhardware当中. HIDL以稳定的, versioned的接口取代了这些HAL接口, 新接口可以是客户端或者是服务端的C++/Java接口

1. HIDL接口有客户端和服务端的实现
   - Client: 通过调用接口中的方法来使用这一方法的代码.
   - Server: 用来从客户端接收调用和返回结果(需要的话)的HIDL接口的一种实现.
2. 一个HIDL定义的.hdl文件包被连接到一个动态库. 这个动态库导出(exports)一个独立的头文件(single header: IFoo.h), 这个头文件可以被clients和servers包含如下:![Screenshot-2018-1-9 ](/home/zs/Downloads/Screenshot-2018-1-9 .png)



在Android O 当中framework和HALs之间使用binder通信. 由于这种方式急剧的增加了binder的数据量(traffic), Android O 使用一些优化来使得binder IPC更快.



#### Multiple binder domains

为了清晰的界定framework和vendor代码之间的流量(traffic), Android O引入了binder上下文的概念. 每个binder上下文有自己的设备节点(device node)和上下文管理器(context manager). 你只能够通过上下文所属的设备节点来获取上下文管理器![binder](/home/zs/Downloads/binder.png)

#### VndServiceManager & ServiceManger

- 之前, binder服务和ServiceManger一同被注册到能被其他进程获取的地方. 在Android O当中, ServiceManager只能被framework和用户进程所使用, 厂商进程(Vendor process)不能在获取到ServiceManager
- Vendor services 目前能够使用的是VndServiceManager, VndServiceManager是serviceManager的一个新的实例, 该实例使用的是/device/vndbinder而不是/device/binder. 当Vendor Process打开/device/vndbinder时, 查找服务自动转到VndServiceManager.



#### What is VNDK

The Vendor Native Development Kit(VNDK) is a set of libraries exclusively for vendors to implement  their HALs. The VNDK ships in **system.img** and is dynamically linked to vendor code at runtime.



#### Why VNDK

Android O enables framework-only updates in which the system partition can be upgraded to the lastest version  while vendor partitons are left unchanged. This implies that binaries built at different times must be able to work with each other;  VNDK covers API/ABI changes across Android releases.



#### VNDK Concept

In an ideal Android O world, framework processes do not load vendor shared libraries, all vendor processes load only vendor shared libraries(and a portion of framework shared libraries), and communications between framework process and vendor process are governed by HIDL and hardware binder.



such a world includes the possibility that stable, public APIs from framework shared libraries might not be sufficient for vendor module developers(although APIs can change between Android releases), requiring that some portion of framework shared libraries be accessible to vendor process. In addition, as performance requirements can lead to compromises, some resopnse-time-critical HALs must be treated differently.

![Screenshot-2018-1-10 【图文】Android Treble 简单介绍_百度文库](/home/zs/Downloads/Screenshot-2018-1-10 【图文】Android Treble 简单介绍_百度文库.png) 

![Screenshot-2018-1-10 【图文】Android Treble 简单介绍_百度文库(1)](/home/zs/Downloads/Screenshot-2018-1-10 【图文】Android Treble 简单介绍_百度文库(1).png)



## HAL(Hardware Abstract Level)

HAL 可定义一个标准接口以供硬件供应商实现，这可让 Android 忽略较低级别的驱动程序实现。借助 HAL，您可以顺利实现相关功能，而不会影响或更改更高级别的系统。**HAL 实现会被封装成模块，并由 Android 系统适时地加载。**

您必须为您的产品所提供的特定硬件实现相应的 HAL（和驱动程序）。HAL 实现通常会内置在共享库模块（`.so` 文件）中，但 Android 并不要求 HAL 实现与设备驱动程序之间进行标准交互，因此您可以视情况采取适当的做法。不过，要使 Android 系统能够与您的硬件正确互动，您**必须**遵守各个特定于硬件的 HAL 接口中定义的合同。

为了保证 HAL 具有可预测的结构，每个特定于硬件的 HAL 接口都要具有 `hardware/libhardware/include/hardware/hardware.h` 中定义的属性。这类接口可让 Android 系统以一致的方式加载 HAL 模块的正确版本。HAL 接口包含两个组件：模块(modules)和设备(devices)。

- modules:  代表你封装的HAL实现,以共享库的形式呈现(.so 文件).
- devices: 你的产品的硬件抽象. 例如, 一个audio模块可以包含一个主audio设备,一个USB audio设备,或者一个Bluetooth A2DP audio设备.









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

![Screenshot-2018-1-8 【图文】Android Treble 简单介绍_百度文库](/home/zs/Downloads/Screenshot-2018-1-8 【图文】Android Treble 简单介绍_百度文库.png)

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

HAL(Hardware Abstract Layer)即Android 硬件抽象层,是对Linux内核驱动程序的封装，向上提供接口，屏蔽底层硬件操作的实现细节。Android把对硬件的支持分成了两层，一层放在用户空间（User Space），一层放在内核空间（Kernel Space），其中，硬件抽象层运行在用户空间，而Linux内核驱动程序运行在内核空间。

> **为什么要这样安排呢？把硬件抽象层和内核驱动整合在一起放在内核空间不可行吗？**
>
> - 严格来讲,Android系统里完全可以没有HAL硬件抽象层,上层应用层可以通过API调用底层硬件，然而从商业的角度来看，把对硬件的支持逻辑都放在内核空间，可能会损害厂家的利益。我们知道，Linux内核源代码版权遵循GNU License，而Android源代码版权遵循Apache License，前者在发布产品时，必须公布源代码，而后者无须发布源代码。如果把对硬件支持的所有代码都放在Linux驱动层，那就意味着发布时要公开驱动程序的源代码，而公开源代码就意味着把硬件的相关参数和实现都公开了，在手机市场竞争激烈的今天，这对厂家来说，损害是非常大的,厂商不会希望自己的核心代码开源出来,而只是提供二进制代码。除此之外, Android系统里面使用的一些硬件设备接口可能不是使用Linux Kernel的统一接口,并且还有GPL版权的原因. 因此，Android才会想到把对硬件的支持分成硬件抽象层和内核驱动层，内核驱动层只提供简单的访问硬件逻辑，例如读写硬件寄存器的通道，至于从硬件中读到了什么值或者写了什么值到硬件中的逻辑，都放在硬件抽象层中去了，这样就可以把商业秘密隐藏起来了。也正是由于这个分层的原因，Android被踢出了Linux内核主线代码树中。大家想想，Android放在内核空间的驱动程序对硬件的支持是不完整的，把Linux内核移植到别的机器上去时，由于缺乏硬件抽象层的支持，硬件就完全不能用了，这也是为什么说Android是开放系统而不是开源系统的原因。

Android 层次结构

学习Android的HAL，对理解整个Android系统，都是极其有用的，因为它从下到上涉及到了Android系统的硬件驱动层、硬件抽象层、运行时库和应用程序框架层等等，下面这个图阐述了硬件抽象层在Android系统中的位置，以及它和其它层的关系：

![img](http://hi.csdn.net/attachment/201106/25/0_1308977488PkP8.gif)

HAL的两种架构变迁:

1. 旧的架构module
2. 新的架构modlestub



#### 1.1 module架构

旧的架构比较好理解,Android用户应用程序和框架层的代码由Java实现, Java运行在Dalvik虚拟机中,没有办法直接访问底层的硬件,只能够通过调用so本地库代码实现,在so本地代码库里有对底层硬件操作的代码,如下图所示:

![img](http://img.my.csdn.net/uploads/201210/14/1350193074_2326.png)

也就是说应用层或者是框架层的Java代码,通过JNI技术调用C/C++编写的so库代码,在so库代码中调用底层驱动,实现上层应用的硬件操作请求. 实现硬件操作的so库为: module.  流程如下:

![img](http://img.my.csdn.net/uploads/201210/14/1350193085_4774.png)

Java代码要访问硬件效率很低,但是Android系统在软件框架和硬件处理器上都在减少和C代码执行效率的差距,据国外测试的结果来看,基本上能够达到C代码效率的95%左右. 

这种module的架构虽然可以满足Java应用访问硬件的需要,但是,使得我们的代码上下层次间的耦合太高,用户程序或者是框架代码必须要去加载module库,如果底层硬件有变化,module需要重新编译,上层也要做相应的变化, 另外,如果多个应用程序同时访问硬件,都需要去加载module, 同一个module被多个进程映射多次,会有代码的重入问题. 因此,Google提出了新的HAL架构.![img](http://img.my.csdn.net/uploads/201211/22/1353553765_2968.png)

#### 1.2 新的HAL架构

![img](http://img.my.csdn.net/uploads/201210/14/1350193116_1922.png)

新的架构使用module stub的方式. stub是存根或桩的意思,其实指的是一个对象代表的意思. 由上面的框架可知上层的应用层或框架层加载so库代码,我们称so库代码为module, 在HAL层注册了每一个硬件对象的存根stub, 当上层需要访问硬件的时候, 就从当前注册的硬件对象stub里查找, 找到之后stub回想上层module提供该硬件对象的operations interface(操作接口), 该操作接口就保存在了module中,上层应用或者框架再通过这个module操作接口来访问硬件. 如下图的LED示例:

![img](http://img.my.csdn.net/uploads/201210/14/1350193180_8172.png)

Led App为Android应用程序,Led App里面的Java代码不能够操作硬件, 将硬件操作工作交给本地的module库led_runtime.so. 他从当前的系统当中查找Led Stub, 查找到之后,Led Stub将硬件驱动的操作返回给module, Led App操作硬件时,通过保存在module中的操作接口间接访问底层硬件. 

新的框架虽然也要加载module库, 但是这个module已经不包含操作底层硬件驱动的功能了,它里面保存的只是底层Stub提供的操作接口, 底层Stub扮演了**接口提供者**的角色. 当Stub第一次被使用时加载到内存,后续在使用时仅仅返回硬件对象操作接口,不会存在设备的多次打开问题,并且由于多进程访问时返回的只是函数指针,代码并没有重入的问题. 	

在学习Android硬件抽象层的过程中，我们将会学习如何在内核空间编写硬件驱动程序、如何在硬件抽象层中添加接口支持访问硬件、如何在系统启动时提供硬件访问服务以及 如何编写JNI使得可以通过Java接口来访问硬件，而作为中间的一个小插曲，我们还将学习一下如何在Android系统中添加一个C可执行程序来访问硬件驱动程序。由于这是一个系统的学习过程，可以分成六篇文章来描述每一个学习过程，转载如下:

1. [在Android内核源代码工程中编写硬件驱动程序](http://blog.csdn.net/luoshengyang/article/details/6568411)。
2. [在Android系统中增加C可执行程序来访问硬件驱动程序](http://blog.csdn.net/luoshengyang/article/details/6571210)。
3. [在Android硬件抽象层增加接口模块访问硬件驱动程序](http://blog.csdn.net/luoshengyang/article/details/6573809)。
4. [在Android系统中编写JNI方法在应用程序框架层提供Java接口访问硬件](http://blog.csdn.net/luoshengyang/article/details/6575988)。
5. [在Android系统的应用程序框架层增加硬件服务接口](http://blog.csdn.net/luoshengyang/article/details/6578352)。
6. [在Android系统中编写APP通过应用程序框架层访问硬件服务](http://blog.csdn.net/luoshengyang/article/details/6580267)。



## Android 层次结构

![img](http://img.my.csdn.net/uploads/201210/14/1350193019_9972.png)



![ape_fwk_all](/home/zs/图片/ape_fwk_all.png)

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





## Android NDK

***Android NDK相关内容来自[极客学院](http://wiki.jikexueyuan.com/project/jni-ndk-developer-guide/ndk.html)***



Android 平台从诞生起，就已经支持 C、C++ 开发。Android 的 SDK 基于 Java 实现，这意味着基于Android SDK 进行开发的第三方应用都必须使用 Java 语言。但这并不等同于“第三方应用只能使用Java”。在Android SDK 首次发布时，Google 就宣称其虚拟机 Dalvik 支持 JNI 编程方式，也就是第三方应用完全可以通过 JNI 调用自己的 C 动态库，即在 Android 平台上，“Java+C”的编程方式是一直都可以实现的。

不过，Google 也表示，使用原生 SDK 编程相比 Dalvik  虚拟机也有一些劣势，Android SDK 文档里，找不到任何 JNI 方面的帮助。即使第三方应用开发者使用 JNI 完成了自己的 C 动态链接库（so）开发，但是 so 如何和应用程序一起打包成 apk 并发布？这里面也存在技术障碍。比如程序更加复杂，兼容性难以保障，无法访问Framework API，Debug 难度更大等。开发者需要自行斟酌使用。

于是 NDK 就应运而生了。NDK 全称是 Native Development Kit。

NDK 的发布，使“Java+C”的开发方式终于转正，成为官方支持的开发方式。NDK 将是 Android 平台支持 C 开发的开端。

### 为什么使用 NDK

- 代码的保护。由于 apk 的 java 层代码很容易被反编译，而 C/C++ 库反汇难度较大。
- 可以方便地使用现存的开源库。大部分现存的开源库都是用 C/C++ 代码编写的。
- 提高程序的执行效率。将要求高性能的应用逻辑使用 C 开发，从而提高应用程序的执行效率。
- 便于移植。用 C/C++ 写得库可以方便在其他的嵌入式平台上再次使用。

NDK 提供了一系列的工具，帮助开发者快速开发 C（或C++）的动态库，并能自动将 so 和 java 应用一起打包成 apk。这些工具对开发者的帮助是巨大的。

NDK 集成了交叉编译器，并提供了相应的 mk 文件隔离 CPU、平台、ABI 等差异，开发人员只需要简单修改 mk 文件（指出“哪些文件需要编译”、“编译特性要求”等），就可以创建出 so。

NDK 可以自动地将 so 和 Java 应用一起打包，极大地减轻了开发人员的打包工作。

### Android NDK 开发环境

1. [下载安装Android NDK](http://blog.sina.com.cn/s/blog_718f290a01012ch0.html)

2. 下载安装cygwin

   由于 NDK 编译代码时必须要用到 make 和 gcc，所以你必须先搭建一个 linux 环境， cygwin 是一个在windows 平台上运行的 unix 模拟环境,它对于学习 unix/linux 操作环境，或者从 unix 到 Windows 的应用程序移植，非常有用。通过它，你就可以在不安装 linux 的情况下使用 NDK 来编译 C、C++ 代码了。下载地址：<http://www.cygwin.com>

   - 然后双击运行吧，运行后你将看到安装向导界面。
   - 点击下一步,此时让你选择安装方式：Install from Internet：直接从 Internet 上下载并立即安装（安装完成后，下载好的安装文件并不会被删除，而是仍然被保留，以便下次再安装）。Download Without Installing：只是将安装文件下载到本地，但暂时不安装。Install from Local Directory：不下载安装文件，直接从本地某个含有安装文件的目录进行安装。
   - 选择第一项，然后点击下一步。
   - 选择要安装的目录，注意，最好不要放到有中文和空格的目录里，似乎会造成安装出问题，其它选项不用变，之后点下一步。
   - 上一步是选择安装cygwin的目录，这个是选择你下载的安装包所在的目录，默认是你运行setup.exe的目录，直接点下一步就可以。
   - 此时你共有三种连接方式选择：Direct Connection：直接连接。Use IE5 Settings：使用IE的连接参数设置进行连接。Use HTTP/FTP Proxy：使用HTTP或FTP代理服务器进行连接（需要输入服务器地址、端口号）。用户可根据自己的网络连接的实情情况进行选择，一般正常情况下，均选择第一种，也就是直接连接方式。然后再点击“下一步”。
   - 这是选择要下载的站点，选择后点下一步。
   - 此时会下载加载安装包列表
   - Search 是可以输入你要下载的包的名称，能够快速筛选出你要下载的包。那四个单选按钮是选择下边树的样式，默认就行，不用动。View 默认是 Category，建议改成 full 显示全部包再查，省的一些包被隐藏掉。左下角那个复选框是是否隐藏过期包，默认打钩，不用管它就行，下边开始下载我们要安装的包吧，为了避免全部下载，这里列出了后面开发NDK用得着的包：autoconf2.1、automake1.10、binutils、gcc-core、gcc- g++、gcc4-core、gcc4-g++、gdb、pcre、pcre-devel、gawk、make共12个包
   - 然后开始选择安装这些包吧，点 skip，把它变成数字版本格式，要确保 Bin 项变成叉号，而 Src 项是源码，这个就没必要选了。-下面测试一下 cygwin 是不是已经安装好了。

   运行 cygwin，在弹出的命令行窗口输入：cygcheck -c cygwin 命令，会打印出当前 cygwin 的版本和运行状 态，如果 status 是 ok 的话，则 cygwin 运行正常。

   然后依次输入 gcc –version，g++ --version，make –version，gdb –version 进行测试，如果都打印出版本信息和一些描述信息，则 cygwin 安装成功！

3. 配置NDK环境变量

   - 首先找到 cygwin 的安装目录，找到一个 home\< 你的用户名 >.bash_profile 文件，我的是：E:\cygwin\home\Administrator.bash_profile ， ( 注意：我安装的时候我的 home 文件夹下面什么都没有，解决 的办法：首先打开环境变量，把里面的用户变量中的 HOME 变量删掉，在 E:\cygwin\home 文件夹下建立名为 Administrator 的文件夹（是用户名），然后把 E:\cygwin\etc\skel.bash_profile 拷贝到该文件夹下 ) 。
   - 打开 bash_profile 文件，添加`NDK=/cygdrive/< 你的盘符 >/<android ndk 目录 >`例如：

   　`NDK=/cygdrive/e/android-ndk-r5`

   　`export NDK`

   NDK 这个名字是随便取的，为了方面以后使用方便，选个简短的名字，然后保存。

   - 打开 cygwin ，输入 cd $NDK ，如果输出上面配置的`/cygdrive/e/android-ndk-r5`信息，则表明环境变量设置成功了。

### 用 NDK 来编译程序

- 现在我们用安装好的 NDK 来编译一个简单的程序吧，我们选择 ndk 自带的例子 hello-jni ，我的位于E:\android-ndk-r5\samples\hello-jni( 根据你具体的安装位置而定 ) ，
- 运行 cygwin ，输入命令 cd /cygdrive/e/android-ndk-r5/samples/hello-jni ，进入到 E:\android-ndk-r5\samples\hello-jni 目录。
- 输入 NDK/ndk−build，执行成功后，它会自动生成一个libs目录，把编译生成的.so文件放在里面。(NDK是调用我们之前配置好的环境变量， ndk-build 是调用 ndk 的编译程序 )
- 此时去 hello-jni 的 libs 目录下看有没有生成的 .so 文件，如果有，你的 ndk 就运行正常啦！

### 在 eclipse 中集成 c/c++ 开发环境

- 装 Eclipse 的 C/C++ 环境插件： CDT ，这里选择在线安装。首先登录 <http://www.eclipse.org/cdt/downloads.php> ，找到对应你 Eclipse 版本的 CDT 插件 的在线安装地址。
- 然后点 Help 菜单，找到 Install New Software 菜单
- 点击 Add 按钮，把取的地址填进去，出来插件列表后，选 Select All ，然后选择下一步即可完成安装。
- 安装完成后，在 eclispe 中右击新建一个项目，如果出现了 c/c++ 项目，则表明你的 CDT 插件安装成功啦！

### 配置 C/C++ 的编译器

- 打开 eclipse ，导入ndk 自带的hello-jni 例子，右键单击项目名称，点击 Properties ，弹出配置界面，之后再点击 Builders ，弹出项目的编译工具列表，之后点击 New，新添加一个编译器，点击后出现添加界面，选择 Program ，点击 OK。
- 出现了添加界面，首先给编译配置起个名字，如： C_Builder，设置 Location 为`< 你 cygwin 安装路径 >\bin\bash.exe`程序，例：`E:\cygwin\bin\bash.exe`，设置 Working Directory 为`<你 cygwin 安装路径 >\bin 目录`，例如： `E:\cygwin\bin`，设置 Arguments 为`--login -c "cd /cygdrive/e/android-ndk-r5/samples/hello-jni && $NDK /ndk-build"`，上面的配置中`/cygdrive/e/android-ndk-r5/samples/hello-jni`是你当前要编译的程序的目录， $NDK 是之前配置的 ndk 的环境变量，这两个根据你具体的安装目录进行配置，其他的不用变， Arguments 这串参数实际是给 bash.exe 命令行程序传参数，进入要编译的程序目录，然后运行 ndk-build 编译程序。
- 接着切换到 Refresh 选项卡，给 Refresh resources upon completion 打上钩。
- 然后切换到 Build Options 选项卡，勾选上最后三项。
- 之后点击 Specify Resources 按钮，选择资源目录，勾选你的项目目录即可。
- 最后点击 Finish，点击 OK 一路把刚才的配置都保存下来，注意：如果你配置的编译器在其它编译器下边，记得一定要点 Up 按钮，把它排到第一位，否则 C 代码的编译晚于Java代码的编译，会造成你的 C 代码要编译两次才能看到最新的修改。
- 编译配置也配置完成啦，现在来测试一下是否可以自动编译呢，打开项目 jni 目录里的 hello-jni.c 文件把提示 Hello from JNI! 改成其他的文字：如： Hello ， My name is alex. ，然后再模 拟器中运行你的程序，如果模拟器中显示了你最新修改的文字，那么 Congratulations ！你已经全部配置成功啦！



