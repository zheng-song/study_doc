[TOC]

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

### 准备三大部件.

1. Bootloader模块: 
2. Kernel模块:
3. Ramdisk模块:






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