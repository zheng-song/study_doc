[TOC]

# Android 简介

Android 操作系统是基于Linux内核, Google使用Linux内核构建了一个可移植和健壮的手机平台,而并没有使用传统Linux的任何其他的东西. Android并不完全依赖于Linux内核, 但是Android底层的开发和传统的嵌入式Linux系统开发关系非常密切, Android系统的驱动与Linux的驱动在开发上几乎保持了完全一致, 另外,Android底层开发和移植的环境也与嵌入式Linux的环境保持了基本一致.

Android是Google于2007年宣布的基于Linux平台的开源手机操作系统. Android是一个包括操作系统, 中间件, 用户界面,和关键应用的移动设备软件堆. 换句话说就是Android是基于Java并运行在Linux内核上的轻量级操作系统. 



# Android 移植

Android 移植分为两个部分: 应用移植和系统移植.

1. 应用移植: 

   - 为保证应用程序能在新的平台上正常运行，需要对源代码就行一些修改，因为硬件平台之间以及Android SDK API之前都可能存在差异。如果无法获取应用程序的源代码，只能重新在新的硬件平台上实现；
   - Android应用移植不涉及驱动和HAL程序库(注：HAL程序库是Android新增加的硬件抽象层)；

2. 系统移植:

   使操作系统在特定硬件平台上运行的条件：

   - 操作系统支持硬件平台上的CPU架构。Linux内核本身支持很多常用的CPU架构，如ARM，X86，PowerPC，因此不需要做过多的改动，但不代表不做改动；
   - 识别硬件平台上的各种硬件。这些工作主要也是由Linux内核完成的，主角是Linux驱动。

   HAL

   - 位于第二层，也是普通的Linux程序库(.so文件)，Android SDK通过HAL之间访问Linux驱动(一般的Linux系统都是由应用程序直接访问驱动)。



总结:

- Android移植的主要工作：移植Linux驱动和移植HAL；
- Android驱动与Linux驱动的区别就是Android增加了HAL,一般的Android驱动会有对应的HAL,但也不是必须的, 通过NDK也可以直接访问Linux驱动(Android O还可以吗?).  
- Android 并不能够使用从[www.kernel.org下载的Linux](http://www.kernel.xn--orglinux-w39li654ahotc/)内核，必须使用Google提供的网址下载可以供Android使用的Linux内核源代码；
- Android移植的基本原则是尽可能找到驱动和HAL的源代码，在源代码的基础上改比从头开始编写容易得多，实在无法获取源代码，就只能从头开始做起了；
- Android移植很大程度上是Linux内核移植，Linux内核移植主要是移植驱动程序。不同Linux版本的驱动程序不能通用，需要修改源代码，并在新的Linux内核下重新编译才可以运行在新的Linux内核版本下。

![](http://img.blog.csdn.net/20160127132726022?watermark/2/text/aHR0cDovL2Jsb2cuY3Nkbi5uZXQv/font/5a6L5L2T/fontsize/400/fill/I0JBQkFCMA==/dissolve/70/gravity/Center)

## Android 系统架构

Android O 之前的系统划分

![](http://img.blog.csdn.net/20160127132905415?watermark/2/text/aHR0cDovL2Jsb2cuY3Nkbi5uZXQv/font/5a6L5L2T/fontsize/400/fill/I0JBQkFCMA==/dissolve/70/gravity/Center)

![Android O 新架构](file:///home/zs/%E5%9B%BE%E7%89%87/ape_fwk_all.png?lastModify=1515466963)

Android从高层到低层分别是应用程序层、应用程序框架层、系统运行库层和linux核心层。Linux内核和应用程序框架层之间的HAL层主要是对linux内核驱动的封装，将硬件抽象化，屏蔽掉了底层的实现细节。HAL规定了一套应用层对硬件层读写和配置的统一接口，本质上就是将硬件的驱动分为**用户空间**和**内核空间**两个层面；Linux内核驱动程序运行于内核空间，硬件抽象层运行于用户空间。

Android 并不是Linux

1. 它没有本地窗口系统
2. 它没有glibc支持
3. 它并不包括一整套标准的Linux使用程序
4. Android专有的驱动程序



### Linux内核层

包括驱动程序以及管理内存、进程、电源等资源的程序,因为Android是基于Linux内核的，所以Android和其他Linux系统的核心部分的差异很小；不同版本的Android使用的Linux内核版本有差异，所以不同Android版本的驱动程序可能不通用；



### c/c++代码库

包括使用C/C++编写的代码库(Linux下的.so文件使用C/C++编写的)，以及嵌入到APK程序中的NDK代码，也包括Dailvk虚拟机的

#### 运行时(Runtime)；

Dailvk虚拟机是Google公司设计的基于Android平台的虚拟机，它可以支持已转换为 .dex（即Dalvik Executable）格式的Java应用程序的运行；这些库能被Android系统中不同的组件使用，通过Android应用程序为开发者提供服务；

一些核心库：

1. 系统C库：一个从BSD继承来的标准C系统函数库(libc)，是专门为基于embedded linux的设备定制的；
2. 媒体库：基于PacketVideo OpenCORE，支持多种常用的音频、视频格式回放和录制，同时支持静态图像文件。编码格式包括MPEG4，H.264，MP3，AAC，AMR，JPG，PNG；
3. Surface Manager：对显示子系统的管理，并且为多个应用程序提供了2D和3D图层的无缝融合
4. LibWebCore：最新的web浏览器引擎，支持Android浏览器和一个可嵌入的web视图；
5. SGL：底层的2D图形引擎；
6. 3D Libraries：基于OpenGL ES 1.0 APIs实现，该库可以使用硬件3D加速(如果可用)或者使用高度优化的3D软加速；
7. FreeType：位图和矢量字体显示；
8. SQLite：一个对于所有应用程序可用，功能强劲的轻型关系数据库引擎； 

Android运行库：

Android包括了一个核心库，该核心库提供了Java编程语言核心库的大多数功能。每一个Android应用程序都在自己的进程中运行，都拥有一个独立的Dalvik虚拟机实例，Dalvik被设计成一个设备可以同时高效地运行多个虚拟系统。Dalvik虚拟机执行.dex的Dalvik可执行文件，该格式文件针对小内存使用做了优化。同时虚拟机是基于寄存器的，所有的类都经由Java编译器编译，然后通过SDK中的dx工具转化成.dex格式由虚拟机执行；



### Android SDK API

直接面向应用程序的Java APK. 这一层可以成为Java API层，因为Android SDk API是用Java语言编写的。实际上这一层就是用Java编写的各种Library，只不过这些library是基于Dalvik虚拟机格式的。



###应用程序

这一层是所有的Android用户都要接触到的，因为这一层相当于Android的UI。所有的Android应用程序(包括拍照，电话，短信，Android的桌面，浏览器以及各种游戏)都属于这一层。主要依靠第三层中的Android SDK API来完成各种功能。





## Android 启动流程

当你按下电源开关后Android设备执行了以下步骤：

![](http://img.blog.csdn.net/20160127132759836?watermark/2/text/aHR0cDovL2Jsb2cuY3Nkbi5uZXQv/font/5a6L5L2T/fontsize/400/fill/I0JBQkFCMA==/dissolve/70/gravity/Center)

Android 系统运行所需要的三大部件: bootloader,kernel , ramdisk.

- bootloader:  作为引导系统,引导linux内核的加载.如uboot
- kernel:  Linux内核
- Ramdisk :  文件系统



## 移植前的准备

目标硬件平台: 树莓派 ARM-V9 四核

开发环境: ubuntu 17.04-64 位

编译器 : 

Android 源码: Android 8.0



### 嵌入式交叉编译环境的搭建

​	交叉编译环境的搭建是Android移植的第一步,不同的体系结构,不同的操作内容甚至是不同版本的内核,都会用到不同的交叉编译器. gcc用来生成交叉编译器,主要生成arm-linux-gcc交叉编译工具, 之后用此交叉编译来编译Linux内核.  我们通常使用网上已有的交叉编译器而不是自己生成, 广泛使用的是[crosstool](http://crosstool-ng.github.io/)的交叉编译工具链.可以在crosstool的[docs](http://crosstool-ng.github.io/docs/build/)中查看使用说明. 





### 准备三大部件.

1. Bootloader模块:  烧写U-boot到开发板
2. Kernel模块: 烧写Linux内核镜像到开发板
3. Ramdisk模块: 烧写文件系统镜像到开发板




#### [U-Boot](https://github.com/u-boot/u-boot)镜像的下载与烧写

##### 1. 简介

​	bootloader是系统上电后最初加载运行的代码。它提供了处理器上电复位后最开始需要执行的初始化代码。通过这一小段程序，我们可以初始化硬件设备、建立内存空间的映射表，从而建立适当的系统软硬件环境，为最终调用操作系统内核做好准备。在PC机上引导程序一般由BIOS开始执行，BIOS完成硬件检测和资源分配之后,将位于硬盘MBR(Main Boot Record，主引导记录)中的OS Bootloader(例如LILO或GRUB)读到系统的RAM当中,然后将控制权交给OS Bootloader,并进一步引导操作系统的启动,主要完成的工作是将内核印象从硬盘上读到RAM当中, 然后跳转到内核的入口点去执行, 即开始启动操作系统。然而在嵌入式系统中通常没有像BIOS那样的固件程序，因此整个系统的加载启动就完全由bootloader来完成。

​	比如在一个基于ARM7TDMI core的嵌入式系统中，系统在上电或复位时通常都从地址0x00000000处开始执行，而在这个地址处安排的通常就是系统的Boot Loader程序。（先想一下，通用PC和嵌入式系统为何会在此处存在如此的差异呢？）

​	[Bootloader](http://blog.csdn.net/prike/article/details/51971832)进行所谓的“kernel引导”，其过程不过是从bootloader里的一句跳转代码，跳转到kernel代码处（执行kernel中的第一个函数），所谓传递参数也不过是bootloader和kernel约定一个内存地点存放。在这个过程中，bootloader和kernel虽然都处于同一个内存里，但是它们除了“引导”与“传递有限的参数”这样的关系，并无其它关系，完全是两个独立的程序。之所以在kernel之前用一个bootloader来引导（为什么开机不能直接执行kernel，所有事情都交给kernel做？），其思想类似于一个板级支持包：kernel假定执行的时候已经具备了一个基本的硬件运行条件，这个环境的初始化（最底层的一些硬件初始化、硬件信息设定）需要bootloader来完成，也许这样kernel的设计才能保持一定的一致性。

​	Bootloader是基于特定硬件平台来实现的，因此几乎不可能为所有的嵌入式系统建立一个通用的Bootloader，不同的处理器架构都有不同的Bootloader，Bootloader不但依赖于cpu的体系结构，还依赖于嵌入式系统板级设备的配置。对于2块不同的板子而言，即使他们使用的是相同的处理器，要想让运行在一块板子上的Bootloader程序也能运行在另一块板子上，一般也需要修改Bootloader的源程序。

​	U-Boot，全称 Universal Boot Loader，是遵循GPL条款的开放源码项目。U-Boot的作用是系统引导。U-Boot不仅仅支持嵌入式Linux系统的引导，它还支持NetBSD, VxWorks, QNX, RTEMS, ARTOS, LynxOS, android嵌入式操作系统。U-Boot除了支持PowerPC系列的处理器外，还能支持MIPS、 x86、ARM、NIOS、XScale等诸多常用系列的处理器。

​	一个**嵌入式的存储设备**通常包括四个分区:

1. 存放bootloader(通常是U-boot).

2. 存放bootloader要传递给系统内核的参数.

3. 系统内核

4. 根文件系统

   ![](http://images.cnblogs.com/cnblogs_com/hanyan225/201105/201105202109126154.jpg)

##### U-boot目录结构

* board [目标板](https://baike.baidu.com/item/%E7%9B%AE%E6%A0%87%E6%9D%BF)相关文件，主要包含SDRAM、FLASH驱动；
* common 独立于处理器体系结构的通用代码，如内存大小探测与故障检测；
* cpu 与处理器相关的文件。如mpc8xx子目录下含串口、网口、LCD驱动及中断初始化等文件；
* driver 通用设备驱动，如CFI FLASH驱动(目前对INTEL FLASH支持较好)
* doc U-Boot的说明文档；
* examples可在U-Boot下运行的示例程序；如hello_world.c,timer.c；
* include U-Boot头文件；尤其configs子目录下与目标板相关的配置头文件是移植过程中经常要修改的文件；
* lib_xxx 处理器体系相关的文件，如lib_ppc, lib_arm目录分别包含与PowerPC、ARM体系结构相关的文件；
* net 与网络功能相关的[文件目录](https://baike.baidu.com/item/%E6%96%87%E4%BB%B6%E7%9B%AE%E5%BD%95)，如bootp,nfs,tftp；
* post [上电自检](https://baike.baidu.com/item/%E4%B8%8A%E7%94%B5%E8%87%AA%E6%A3%80)文件目录。尚有待于进一步完善；
* rtc RTC驱动程序；
* tools 用于创建U-Boot S-RECORD和BIN[镜像文件](https://baike.baidu.com/item/%E9%95%9C%E5%83%8F%E6%96%87%E4%BB%B6)的工具；

##### 工作模式

​	U-boot的3种映像格式(U-Boot , U-Boot.srec , U-Boot.bin)都可以烧写到Flash中, 但是需要看加载器是否能够识别这些格式. 最常使用的是U-Boot.bin格式, U-Boot的工作模式有启动加载模式和下载模式。

1. 启动加载模式(Flash 启动方式): Bootloader的正常工作模式，嵌入式产品发布时，Bootloader必须工作在这种模式下，Bootloader将嵌入式操作系统从FLASH中加载到SDRAM中运行，整个过程是自动的。Flash有NOR Flash和NAND Flash两种。NOR Flash可以支持随机访问，所以代码可以直接在Flash上执行，Bootloader一般是存储在Flash芯片上的。另外Flash上还存储着参数、内核映像和文件系统。这种启动方式与网络启动方式之间的不同之处就在于，在网络启动方式中，内核映像和文件系统首先是放在主机上的，然后经过网络传输下载进目标板的，而这种启动方式中内核映像和文件系统则直接是放在Flash中的，这两点在我们u-boot的使用过程中都用到了。
2. 下载模式(网络启动方式): Bootloader通过某些通信手段将内核映像或根文件系统映像等从PC机中下载到目标板的FLASH中。用户可以利用Bootloader提供的一些命令接口来完成自己想要的操作。

##### [启动流程](http://www.cnblogs.com/andrew-wang/archive/2012/12/06/2806238.html)

​	大多数BootLoader都分为stage1和stage2两大部分，U-boot也不例外。依赖于cpu体系结构的代码(如设备初始化代码等) 通常都放在stage1且可以用汇编语言来实现，而stage2则通常用C语言来实现，这样可以实现复杂的功能，而且有更好的可读性和移植性。

1. stage1(start.s代码结构)

   U-boot的stage1代码通常放在start.s文件中，它用汇编语言写成，其主要代码部分如下：

   - 定义入口`_start`。由于一个可执行的image必须有一个[入口点](https://baike.baidu.com/item/%E5%85%A5%E5%8F%A3%E7%82%B9)，并且只能有一个全局入口，通常这个入口放在rom(Flash)的0x0地址，因此，必须通知[编译器](https://baike.baidu.com/item/%E7%BC%96%E8%AF%91%E5%99%A8)以使其知道这个入口，该工作可通过修改连接器脚本来完成。

   - 设置异常向量(exception vector)。这样在CPU发生异常的时候就跳转到/cpu/xxxx/interrupts中去执行相应的中断代码.  在interrupts**文件中大部分的异常代码都**没有实现具体的功能，只是打印一些异常消息**，其中关键的是reset中断代码，**跳到reset入口地址。

     reset复位入口**之前有一些段的声明**。

     - 在reset中，首先是将cpu设置为svc32模式下，并屏蔽所有irq和fiq。
     - 在u-boot中除了**定时器使用了中断外**，**其他的基本上都不需要使用中断**，比如串口通信和网络等通信等，在u-boot中只要完成一些简单的通信就可以了，所以在这里屏蔽掉了所有的中断响应。
     - 初始化外部总线。这部分首先设置了I/O口功能，包括串口、网络接口等的设置，其他I/O口都**设置为GPIO**。然后设置BCFG0~BCFG3，即外部总线控制器。这里bank0对应Flash，设置为16位宽度，总线速度设为最慢，以实现稳定的操作；Bank1对应DRAM，设置和Flash相同；Bank2对应RTL8019。

   - 设置CPU的速度、[时钟频率](https://baike.baidu.com/item/%E6%97%B6%E9%92%9F%E9%A2%91%E7%8E%87)、系统重映射(告诉处理器在系统发生中断的时候到外部存储器中去读取中断向量表)及中断[控制寄存器](https://baike.baidu.com/item/%E6%8E%A7%E5%88%B6%E5%AF%84%E5%AD%98%E5%99%A8)。

   - 初始化内存控制器 。

   - 将rom中的程序复制到ram中。通常把stage2加载到RAM空间中来执行，因此必须为加载Boot Loader的stage2准备好一段可用的RAM空间范围。空间大小最好是memory page大小(通常是4KB)的倍数. 一般而言，**1M**的RAM空间已经足够了。flash中存储的u-boot可执行文件中，代码段、数据段以及BSS段都是首尾相连存储的，  所以在计算搬移大小的时候就是利用了用BSS段的首地址减去代码的首地址，这样算出来的就是实际使用的空间。程序用一个循环将代码搬移到0x81180000，即RAM底端1M空间用来存储代码。然后程序继续将中断向量表搬到RAM的顶端。由于stage2通常是C语言执行代码，所以还要建立堆栈区。

   - 初始化[堆栈](https://baike.baidu.com/item/%E5%A0%86%E6%A0%88) 。在堆栈区之前还要将malloc分配的空间以及全局数据所需的空间空下来，他们的大小是由宏定义给出的，可以在相应位置修改。

   - 转到ram中执行，该工作可使用指令ldrpc来完成。

   ![搬移后内存分布情况图](http://hi.csdn.net/attachment/201111/7/0_1320661431vvMa.gif)




2. stage2（C语言代码部分）

​	 这个部分是相对变化不大的部分, 我们针对不同的板子改变它调用的一些初始化函数，并且通过设置一些**宏定义来改变初始化的流程**，所以这些代码在移植的过程中并不需要修改，也是错误相对较少出现的文件。在文件的开始先是定义了一个**函数指针数组**，通过这个数组，程序通过一个循环来按顺序进行常规的初始化，并在其后通过一些宏定义来初始化一些特定的设备。在最后程序进入一个循环，**main_loop**。**这个循环接收用户输入的命令，以设置参数或者进行启动引导**。

​	lib_arm/board.c中的start [armboot](https://baike.baidu.com/item/armboot)是C语言开始的函数，也是整个启动代码中C语言的主函数，同时还是整个u-boot（armboot）的主函数，该函数主要完成如下操作：

- 调用一系列的初始化函数。
- 初始化flash设备。
- 检测系统内存映射,初始化系统内存分配函数。
- 如果目标系统拥有nand设备，则初始化nand设备。
- 如果目标系统有显示设备，则初始化该类设备。
- 初始化相关网络设备，填写ip,c地址等。
- 将内核从Flash读取到RAM中并为内核设置启动参数.
- 进入命令循环（即整个boot的工作循环），接受用户从串口输入的命令，然后进行相应的工作。

[代码分析](http://blog.csdn.net/hare_lee/article/details/6916325)



U-boot上电启动之后,可以按任意键退出自动启动状态,进入命令行模式.![uboot](/home/zs/桌面/uboot.png)

在命令行模式下可以执行U-Boot的命令并执行. U-Boot可以支持几十个常用的命令,通过这些命令,可以对开发版进行调试,可以引导Linux内核, 还可以擦写Flash完成系统部署等功能. 掌握这些命令才能顺利的进行嵌入式系统的开发.  输入help可以得到当前U-Boot的所有命令列表.






## 编译Android 源码

### Android源码目录结构

以下的Android 源码目录结构相关的内容来自[mr_raptor](http://blog.csdn.net/mr_raptor/article/details/20934501)的CSND博客,Android编译的相关内容也可以查看其博客.



在Android源码中，按照不同功能代码被放在不同的目录下：

| 目录          | 描述                                       |
| :---------- | :--------------------------------------- |
| bionic      | 针对Android系统定制的仿生标准C库、链接器等所在目录，Android系统并没有使用Linux的glibc库，bioinc C库针对嵌入式系统做了优化，添加了一些Android特定的函数API同时大大减少库的体积，也避免了LGPL版权的问题。 |
| bootable    | Android系统引导启动代码，用来引导系统、更新系统、恢复系统。        |
| build       | Android的编译系统目录，里面包含大量的Makefile，用来编译目标系统、Host主机开发环境等。 |
| cts         | 兼容性测试工具目录。                               |
| dalvik      | Dalvik虚拟机，Android系统得以运行的虚拟执行环境。          |
| development | 程序开发所需要的模板和工具。                           |
| external    | Android系统使用的其它开源代码目录，如jpeg图片解码开源库、opencore开源代码等。 |
| frameworks  | 框架层代码，frameworks/base目录下存放目标系统的框架库，frameworks/policies/base下存放应用程序框架代码。 |
| hardware    | HAL（Hardware Abstraction Layer）硬件抽象层代码。  |
| kernel      | Linux内核目录，默认下载的Android源码里没有，需单独下载。       |
| packages    | Android系统级应用程序源码目录，如摄像应用、电话应用等。          |
| prebuilt    | 主机编译工具目录，如arm-linux-gcc交叉系统工具链等。         |
| sdk         | SDK及模拟器。                                 |
| system      | init进程、蓝牙、无线WIFI工具、uevent进程目录。           |
| devices     | 厂商设备配置目录，针对不同设备，由不同的子目录来分别管理，用来裁剪实现不同设备上Android目标系统。 |





## Android 编译后的out目录分析

Android编译完成后，将在根目录中生成一个out文件夹，所有生成的内容均放置在这个文件夹中。

![out](/home/zs/桌面/out.png)

主要的两个目录是host和target, 前者表示在主机（x86）生成的工具，host目录是一些在主机上用的工具，有一些是二进制程序,有一些是JAVA的程序。后者表示目标机（模认为ARMv5）运行的内容。target中中common目录表示通用的内容，product中则是针对产品的内容。