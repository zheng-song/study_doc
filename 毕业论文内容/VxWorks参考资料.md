# 以下内容来自VxWorks Programmer's Guide

## 2. basic OS

### 2.1 简介

​	现代实时系统基于多任务和任务间通信的互补概念。多任务环境允许将实时应用程序构建为一组独立任务，每个任务都有自己的执行线程和一组系统资源。任务间通信设施允许这些任务同步并进行通信以协调其活动。在VxWorks中，任务间的通信工具包括快速信号量、消息队列、管道、套接字。

​	实时系统的另外一个关键设施是硬件中断处理，因为中断是通知系统发生外部事件的常用机制。为了尽可能的快速响应中断，VxWorks中的中断服务例程(ISR)在任何任务的上下文之外的特定上下文中运行(*interrupt service routines (ISRs)* in VxWorks run in a special context of their own, outside any task's context.)。





## I/O System

### 4.1 介绍	

VxWorks I/O系统为基本I/O和缓冲I/O提供了标准C库。基本I/O库是UNIX兼容的；缓冲I/O库是ANSI C兼容的。==在内部，VxWorks I / O系统具有独特的设计，使其比大多数其他I / O系统更快，更灵活。这些是实时系统中的重要属性。==

![VxWorks I/O系统概述](https://upload-images.jianshu.io/upload_images/6128001-179cacc6be802f2b.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)



Terminal的COM的工作模式一般为LINE模式，即发现行结束标志(ENTER)才发送一次缓冲数据。 此处VxWorks Host上的COM的工作模式为RAW，无缓冲。LINE模式和RAW模式的区别参考VxWorks Programmer‘s Guide。

### 4.2 文件、设备和驱动程序

​	在VxWorks中，应用程序通过打开命名*文件来*访问I / O设备。一个*文件*可以引用以下两件事之一：

- 非结构化的“ *原始* ” *设备，*例如串行通信通道或任务管道。
- 在结构化、包含文件系统的随机存取设备上的“逻辑文件”。

VxWorks I / O系统为驱动程序处理每个特定设备提供了相当大的灵活性。驱动程序符合这里介绍的传统用户视图，但可能会有不同的细节。==虽然所有I / O都是针对指定文件的，但它可以在两个不同的级别完成：*基本*和*缓冲*。这两者在缓冲数据的方式和可以进行的调用类型方面有所不同。==



### 4.3 基本I/O

​	基本I/O是VxWorks中最低级别的I/O。基本I / O接口与标准C库中的I / O原语是源兼容的。有7个基本I/O调用，如下表所示：

| Call     | Description                              |
| -------- | ---------------------------------------- |
| creat( ) | Creates a file.                          |
| delete() | Deletes a file.                          |
| open()   | Opens a file. (Optionally, creates a file.) |
| close()  | Closes a file.                           |
| read()   | Reads a previously created or opened file. |
| write()  | Writes to a previously created or opened file. |
| ioctl()  | Performs special control functions on files. |



### 4.7.1 串行I/O设备(终端和伪终端)

​	VxWorks提供终端和伪终端设备驱动程序（*tty*和*pty*驱动程序）。的*TTY*驱动程序是实际终端; 的*PTY*驱动程序用于模拟终端的进程。这些伪终端在远程登录设备等应用中非常有用。

​	VxWorks串行I / O设备是缓冲的串行字节流。每个设备都有一个用于输入和输出的环形缓冲区（循环缓冲区）。从*tty*设备读取提取输入环中的字节。写入*tty*设备会将字节添加到输出环中。在系统初始化期间创建设备时指定每个环形缓冲区的大小。



#### RAW模式和LINE模式

​	TTY设备有两种工作模式：RAW模式和LINE模式，RAW模式是默认的模式。在RAW模式下，每一个刚从设备输入的字符对于读者都是有效的；在LINE模式下所有的输入字符都会被存储，直到NELLINE(ENTER)字符被输入。可以使用带FIOSETOPTIONS功能的ioctl()程序来进行设置。





### 4.8 VxWorks和主机系统I/O之间的区别

VxWOrks中最常见的I/O使用的是与UNIX和Windows中的I/O完全兼容的源，但是，注意以下的区别：

- 设备配置：在VxWorks中可以动态的安装和移除设备驱动程序。
- 文件描述符：在UNIX和WINDOWS中fd对每一个进程都是唯一的。在VxWorks中，除了标准的输入，标准输出和标准错误以外，fd是全局实体，可以由任何任务访问，这可以是任务特定的。
- I/O控制：传递给ioctl()函数的具体参数可以在UNIX和VxWorks之间有所不同。
- 驱动程序例程：在UNIX中设备驱动程序以系统模式执行，不能够被抢占。而在VxWorks当中，驱动程序例程可以被抢占，因为他们在调用他们的任务的上下文中执行。



###  4.9 内部结构



#### 4.9.5 驱动程序支持库

下表中的子例程库可以帮助编写设备驱动程序。通过使用这些库，大多数遵循标准协议的设备驱动程序只需要编写几页与设备相关的代码即可完成。有关详细的信息，请参阅每个库的参考条例。

| Library  | Description                          |
| -------- | ------------------------------------ |
| errnoLib | Error status library                 |
| ftpLib   | ARPA File Transfer Protocol library  |
| ioLib    | I/O interface library                |
| iosLib   | I/O system library                   |
| intLib   | Interrupt support subroutine library |
| rngLib   | ring buffer subroutine library       |
| ttyDrv   | Terminal driver                      |
| wdLib    | Watchdog timer subroutine library    |





## [一个Linux下open()调用的流程](https://blog.csdn.net/new_abc/article/details/7607731)

​	在操作一个设备和文件的时候，我们首先要通过open()系统调用去打开这个设备，这样才能够调用其他的函数如read、write来操作它，即通知内核新建一个代表改文件的结构，并且返回该文件的描述符(一个整数)，该描述符在进程内唯一。Linux中当我们在用户空间(**注意VxWorks中没有用户空间和内核空间之分**)

`int open(const char * pathname,int oflag, mode_t mode )`调用流程如下：

当open系统调用产生时，就会进入下面这个系统调用函数():

```C
SYSCALL_DEFINE3(open, const char __user *, filename, int, flags, int, mode) 
{  
    long ret;  
    /*检查是否应该不考虑用户层传递的标志、总是强行设置  
    O_LARGEFILE标志。如果底层处理器的字长不是32位，就是这种  
    情况*/   
    if (force_o_largefile())  
        flags |= O_LARGEFILE;  
     /*实际工作*/   
    ret = do_sys_open(AT_FDCWD, filename, flags, mode);  
    /* avoid REGPARM breakage on x86: */  
    asmlinkage_protect(3, ret, filename, flags, mode);  
    return ret;  
}  
```

在这个函数里面继续调用do_sys_open完成 open操作

```C
long do_sys_open(int dfd, const char __user *filename, int flags, int mode)  
{  
     /*从进程地址空间读取该文件的路径名,将其从用户态拷贝到内核态*/    
    char *tmp = getname(filename);  
    int fd = PTR_ERR(tmp);  
    if (!IS_ERR(tmp)) {  
         /*在内核中，每个打开的文件由一个文件描述符表示,该描述符在特定于进程的数组中充当位置索引(数组是task_struct->files->fd_arry)，该数组的元素包含了file结构，其中包括每个打开文件的所有必要信息。因此，调用下面函数查找一个未使用的文件描述符,返回的是上面说的数组的下标*/    
        fd = get_unused_fd_flags(flags);  
        if (fd >= 0) {  
            /*fd获取成功则开始打开文件，此函数是主要完成打开功能的函数*/  
            //如果分配fd成功，则创建一个file对象  
            struct file *f = do_filp_open(dfd, tmp, flags, mode, 0);  
            if (IS_ERR(f)) {  
                put_unused_fd(fd);  
                fd = PTR_ERR(f);  
            } else {  
/*文件如果打开成功，调用fsnotify_open()函数，根据inode所指定的信息进行打开 函数（参数为f）将该文件加入到文件监控的系统中。该系统是用来监控文件被打开，创建， 读写，关闭，修改等操作的*/   
                fsnotify_open(f->f_path.dentry);  
/*将文件指针安装在fd数组中将struct file *f加入到fd索引位置处的数组中。如果后续过程中，有对该文件描述符的操作的话，就会通过查找该数组得到对应的文件结构，而后在进行相关操作。*/
                fd_install(fd, f);  
            }  
        }  
        putname(tmp);  
    }  
    return fd;  
}  
```

这个函数完成的主要工作如下：

1. 将文件名参数从用户态拷贝至内核，调用函数get_name()；
2. 从进程的文件表中找到一个空闲的文件表指针，调用了函数get_unused_fd_flgas()；
3. 完成真正的打开操作，调用函数do_filp_open()；
4. 将打开的文件添加到进程的文件表数组中，调用函数fd_install()；









## Linux 和 VxWorks的主要区别

内核结构不同，linux是宏内核，VxWorks是微内核

**微内核（英文中常译作micro kernel）:** 是一种只提供必要服务的操作系统内核；这些必要的服务包括任务管理、中断处理、内存管理等。其他服务，如文件管理、网络支持等通过接口连到微内核。在微内核中，用以完成系统调用功能的程序模块通常只进行简短的处理，而把其余工作通过消息传递交给内核之外的进程来处理。在典型情况下，每个系统调用程序模块都有一个与之对应的进程，微内核部分经常只不过是一个消息转发站，这种方式有助于实现模块间的隔离。这种内核设计的最根本思想就是要保持操作系统的内核尽可能小，因为内核是直接与计算机硬件相关的，内核越小，就越便于在不同的硬件系统间进行移植。微内核结构的另外一个优点是，可以使不需要的模块不加载到内存中，因此，微内核就可以更有效地利用内存。微内核具有很好的扩展性，并可简化应用程序开发。用户只运行他们需要的服务，这有利于减少磁盘空间和存储器需求。

**宏内核（英语：Monolithic kernel）：**又称单核心，是操作系统核心架构的一种，除了基本的服务，内核还包括文件系统、网络协议等。此架构的特性是整个核心程序都是以核心空间（Kernel Space）的身份及监管者模式（Supervisor Mode）来运行。宏内核的内部可被分为若干模块（或者是层次或其他），但在运行时，它是一个独立的二进制大映像。模块间的通信不是通过消息传递，而是通过直接调用其他模块中的函数来实现的。现在多数采行宏内核架构设计的操作系统，如OpenVMS、Linux、FreeBSD、以及Solaris等，在运作运行阶段中，以动态方式来加载（Load）、卸载（Unload）可运行的模块，不过这些模块是属于二进制代码的层次，或称镜像层次，而非核心架构的层次。即使宏内核进行模块化转化，也不会与微核心或混核心架构的核心产生区分上的混淆，因为微核心、混核心的模块是属于系统架构的层次。

就实务上，动态加载/卸载模块的作法，等于是用一种较简易的方式来弹性管控运行中的操作系统核心，若没有动态加载/卸载机制，操作系统的核心想要进行任何的调整、变换，都必须重启才能达成。因此模块化是必然且必要的，如此才能让核心功效轻松地扩展、延伸，此外也能适时减轻硬件的运行运作负担。



## USB 键盘上的内容

​	USB的应用层是指各种各样的USB设备驱动，这些设备通过USBD层的接口来实现与USB设备的通信，实现了USB设备驱动。

​	USB键盘驱动主要包括两个部分：**USB键盘核心驱动，USB键盘初始化。**其中USB键盘驱动核心是由usbKeyboardLib来完成的，USB键盘初始化是由函数库usrUsbKbdInit来完成。函数库usbKeyBoardLib提供了一些API函数，这些API函数在实现标准SIO驱动接口之外还针对USB的热插拔特性做了一些扩展。作为一个USB设备，USB键盘驱动主要是由系统向键盘发送IRP来完成按键的获取以及状态指示灯的控制。同时和标准的SIO驱动一样，这个模块的驱动也可以通过调用usbKeyBoardDevInit函数来完成初始化。usbKeyboardDevInit()函数初始化了与USBD的连接及其他一些内部资源。与其他SIO驱动不同的是，在调用usbKeyboardDevInit()函数进行初始化之前，不需要对模块内部的任何变量进行初始化。

​	需要注意的是，在调用usbKeyboardDevInit()函数之前，必须要首先调用 usbdInitialize()函数以确保USBD的初始化。而且还必须保证至少一个USB HCD(USB Host Controller Driver)已经attach到USBD函数——这一步可以通过调用usbdHcdAttach()来完成。



​	USB键盘设备统一向USBD层注册一个USBD_CLIENT结构变量，并通过类注册函数usbdDynamicAttachRegister向USBD层注册回调函数usbKeyboardAttachCallback。对每个USB键盘设备，都会注册一个interrupt pipe，并通过该pipe向设备发送interrupt Irp。



​	函数库usbKeyboardLib为上层提供了一系列通用的函数接口，这个结构封装在数据结构sio_drv_funcs中，对每个USB键盘，都会在函数库中对应一个USB_KBD_SIO_CHAN结构变量，该结构变量是从结构SIO_CHAN派生而来的，而结构SIO_CHAN中有一个元素SIO_DRV_FUNCS * pDrvFuncs则是指向该USB键盘的一些基本的操作函数，这些函数都封装在结构sio_drv_funcs中。



​	==函数库usbKeyBoardLib中有如下关键数据结构：==

**LOCAL LIST_HEAD sioList：**sioList链记录了系统管理的每一个USB键盘的信息，每一个键盘信心都用一个USB_KBD_SIO_CHAN结构变量来描述。函数库USBKeyboardLib支持多个USB键盘，当系统检测到一个USB键盘的时候，就在sioList链中添加一个USB_KBD_SIO_CHAN结构。

**LOCAL LIST_HEAD reqList：**由于USB设备属于即插即用设备，为了支持USB键盘的即插即用特性，需要在系统启动的时候注册一个USB键盘驱动的动态挂载和卸载程序。对于



## DEV_HDR 结构体

​	所有的底层驱动都要对其驱动的硬件设备维护一个结构，用以保存设备的关键参数：**设备的寄存器基地址、中断号、设备硬件缓存区基地址等。**这些信息将随着设备类型的不同而有所差异。 对于此处要我们的驱动要保存的信息，我们将其保存在一个CP210X_DEV结构体当中。

DEV_HDR结构体定义在内核的`h/iosLib.h`文件当中，代码如下

```C
typedef struct 
{
  DL_NODE node;  //device linked list node
  short drvNum;  // driver number for this device
  char *name;    // device name
}DEV_HDR;
```

​	自定义设备结构的第一个成员必须是DEV_HDR结构类型。我们自定义的结构体CP210X_DEV仅仅对驱动本身有效，但是对于内核I/O子系统而言，它会将所有的设备结构都看做是DEV_HDR类型的。内核只会对DEV_HDR结构进行管理，在系统设备列表中，内核只使用DEV_HDR结构中的成员，对于自己定义的结构体中的其他成员的含义，将由你的驱动进行解释，内核并不关心。







## VxWorks内核解读

​	为了实现应用程序的平台无关性操作系统为应用层提供了一套标准的接口函数，这些接口函数在所有的平台上都保持一致，只是随着平台的变化，底层驱动或接近驱动部分操作系统中间层可能会随着调整。这样可以使用户程序独立于具体的硬件平台，增加了应用层开发的效率，避免了重复编码。通用操作系统GPOS(General Purpose Operating System)比如Unix,Linux，将这套提供给应用层的标准接口函数从操作系统中独立出来，专门以标准库的形式存在，增加了应用程序的平台无关性，平台之间的差别完全被操作系统屏蔽。	

​	==和Unix,Linux类似，VxWorks也对应用层提供了一套标准文件操作接口函数，实际上与GPOS提供接口类似，我们将其称作为标准I/O 库，VxWorks 下由ioLib.c 文件提供。ioLib.c 文件提供如下标准接口函数：creat、open、unlink、remove、close、rename、read、write、ioctl、lseek、readv、writev 等。VxWorks 操作系统区别于GPOS的一个很大不同点是：VxWorks 不区分用户态和内核态，用户层程序可以直接对内核函数进行调用，而无须使用陷阱指令之类的机制，以及存在使用权限上的限制。因此VxWorks 提供给应用层的接口无须通过外围库的方式，而是直接以内核文件的形式提供。用户程序可以直接使用ioLib.c 文件定义的如上这些函数，这些函数名称与GPOS标准库下的函数名一致，是VxWorks 对标准库的模拟。==

​	==在linux系统进程中，分为内核空间和用户空间，当一个任务（进程）执行系统调用而陷入内核代码中执行时，我们就称进程处于内核运行态（内核态）。在内核态下，CPU可执行任何指令。当进程在执行用户自己的代码时，则称其处于用户运行态（用户态）。用户态不能访问内核空间，包括代码和数据。所有进程的内核空间（3G－4G）都是共享的。当我们在用户空间调用open之后，会产生一个软中断，然后通过系统调用进入内核空间。通过系统调用号，我们就可以跳转到该中断例程的入口地址。==

​	VxWorks的I/O系统给应用程序提供了简单、统一、与下列设备无关的接口：

1. 面向终端的字符设备或者是通信线路设备
2. 随机访问块设备，例如硬盘；
3. 虚拟设备，例如管道和套接字；
4. 用于监控和控制的数据或者模拟I/O设备；
5. 访问远程设备的网络设备。

在VxWorks系统中，应用程序通过文件来访问I/O设备，一个文件通常代表以下两类设备：

1. 无结构的原始（raw）设备，比如串口通信通道，用于任务间通信的管道设备；
2. 包含文件系统的、可以随机访问设备上的逻辑文件；例如`/usr/myfile`,`/pipe/mypipe`,`/tyC0/0`

第一个文件myfile在一个名为/usr的硬盘上；第二个在命名管道上，通常管道以/pipe开始；第三个文件映射到串行通道。==I/O系统可以采用文件的方式处理这些设备，在VxWorks中尽管这些设备物理性质诧异巨大，但是它们都称为文件，在这一点上借鉴了Unix的设计思想。==

​	在VxWorks系统中的设备由程序模块来处理，这些程序模块被称为驱动(Driver);使用I/O系统并不需要了解这些驱动和设备的具体实现机制，但==VxWorks的I/O系统赋予特定设备的驱动极大的灵活性，尽管所有的I/O系统均是以文件来索引，但是却有两种不同的实现方式：基本I/O实现和带缓存的I/O设想，这两种实现方式的差别在于数据是否缓存、以及系统调用实现的方式，==如图1所示。



![VxWorks I/O系统概述](https://upload-images.jianshu.io/upload_images/6128001-0ea27cf519ba0b5c.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)

​	文件名用一个字符串表示，一个没有结构的文件用一个设备名表示。以文件系统设备为例，设备名字后面紧跟着文件名，比如`/tyCo/0`用于命名一个指定的串行I/O通道，`DEV1:/file1`表示在设备DEV1：上的一个文件file1。

​	当在一个I/O调用中指定一个文件名时，I/O系统会根据这个文件名来搜素设备，然后I/O例程定位到这个设备；如果I/O系统找不到文件名指定的设备，I/O例程就会被定向到默认的设备上，我们可以将这个默认的设备指定为系统中的任何一个设备，或者不包含任何设备，如果不包含任务设备，那么当I/O系统找不到文件名指定的设备时，将会返回一个错误。VxWorks提供接口ioDefPathGet()获取当前默认的设备，也可以使用接口ioDefPathSet()设置当前默认的设备。

​	非块设备在添加到I/O系统上时会被命名，这通常在系统初始化时完成。块设备在它们被指定的文件系统初始化时命名。VxWorks I/O系统在给设备命名的方式上没有限制。除非是在寻找匹配的设备和文件时，否则I/O系统不对设备和文件名做任何解释。

​	但是采用一个传统的惯例来给设备和文件命名还是非常有用的：大多数的设备名都是以斜杠“/”开始，但是非NFS网络设备和VxWorks DOS设备除外。

​	按照惯例，基于NFS的网络设备都是以一个斜杠“/”开始的名字来命名挂载点，例如/usr。非NFS网络设备通常以一个远程的机器名跟一个冒号来命名，例如host:，其余的名字则是远程系统目录中的文件名 。使用dosFs的文件系统通常使用大写字母和数字组合的形式，并紧跟一个冒号:来命名，例如DEV:



## VxWorks的I/O框架

​	==VxWorks的I/O系统和Unix或者是Linux系统的不同之处在于，响应用户请求的工作分布在与I/O系统无关的设备和设备驱动本身之间。== ??????

​	在GPOS中，设备驱动提供某一些底层的I/O例程用于从字符串定位的设备中读取或者是写入字符序列。基于字符设备的高级别通信协议例程通常由I/O系统中独立于设备的部分实现。在驱动例程获取控制权之前，用户请求严重的依赖于I/O系统的服务。==尽管这种解决方案使得驱动实现起来较为容易，并且驱动的行为表现的尽可能的相似，但是它存在这种缺陷：即当驱动的开发者实现现存I/O系统中不存在的协议时，将会面临极大的困难。而在实时系统当中，如果某一些设备的吞吐量至关重要时，我们需要绕过标准的协议，或者说这类设备本身就不适合标准的模型。==

​	在VxWorks系统中，在控制权转接到设备的驱动程序之前，用户的请求进行尽可能少的处理。==VxWorks I/O系统的角色更像是一个转接开关，负责将用户的请求转接到合适的驱动例程上。每一个驱动都能够处理原始的用户请求，到最适合它的设备上。另外，驱动程序的开发者也可以利用高级别的库例程来实现基于字符设备或者是块设备的标准协议.==因此，VxWorks的I/O系统具有两方面的有点：一方面使用尽可能少的使用驱动相关代码就可以为绝大多数的设备写成标准的驱动程序，另外一方面驱动程序的开发者可以在合适的地方使用非标准的程序自主的处理用户请求。

​	设备一般有两种类型(暂不考虑网络设备)：块设备或者是字符设备。块设备用于存放文件系统，在块设备中数据以块的形式存放，块设备采用随机访问的方式，像硬盘或者软盘都属于块设备；不属于块设备范畴的设备被称为是字符设备，像串行设备或者是图形输入设备都属于字符设备。

​	VxWorks的I/O系统包含三个元素：**驱动、设备和文件**。接下来我们以字符设备为例，其中大部分的分析也适用于块设备。当然，块设备必须和vxWorks 文件系统进行交互，所以其在组织结构上和字符设备稍有不同。



## 7.2 VxWorks I/O基本接口

​	O基本接口是VxWorks系统中最底层的接口，VxWorks的基本I/O接口和标准C库中的I/O接口原语实现了源码级兼容，由VxWorks的ioLib库提供支持，基本接口有7个，如下表1所示。

![VxWorks 基本I/O接口](https://upload-images.jianshu.io/upload_images/6128001-14a2777a7a8aa264.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)

​	在基本I/O基本，文件是用一个文件描述符fd(file descriptor)来引用，fd是上表中open()或者creat()返回的一个整数。其它的基本I/O接口使用这个fd作为参数来指定操作的文件。

​	文件描述符fd对系统来说是全局的，例如任务A调用write()操作fd7和任务B调用write()操作fd7，所引用的都是同一个文件。

​	当打开一个文件时fd将会被分配并返回给用户使用，当文件关闭时，fd也将会被释放。在VxWorks系统中fd的数目是有限的，为了避免超过VxWorks系统的限制，当fd使用完毕后，最好将其关闭。fd的数目在VxWorks系统初始化时设定。

​	在VxWorks系统中下面三个描述符是系统保留的，具有特殊的意义：

0：标准输入；

1：标准输出；

2：标准错误输出；



VxWorks的基本I/O例程open()和creat()通常不会返回上面三个描述符，标准的描述符使得任务和模块独立于他们实际的I/O分配。如果一个模块发送输出信息到标准输出描述符(fd=1)上，那么输出可以重定向到任何一个文件或者设备上，而不需要修改该模块。

​	==VxWorks允许两级重定向，首先，存在一个全局的分配三个标准文件描述符；其次个别的任务可以重新分配三个标准描述符，将其重定位到只分配给这些任务的设备上。==

​	**全局重定向：** 在VxWorks系统初始化时，标准描述符被重定向到系统的终端上。当任务被创建时，他们没有被分配私有的文件描述符fd，只能够使用全局的标准描述符。VxWorks提供了`ioGlobalStdSet()`用于重定向全局描述符，例如下面的例子将全局标准输出描述符(fd=1)重定向到文件描述符fileFd指向的一个打开文件：`ioGlobalStdSet(1,fileFd);`

​	如果任务没有私有的文件描述符，都可以使用系统的全局描述符,例如任务tRlogind调用`ioGlobalStdSet()`将I/O输出重定向到rlogin会话任务上。

​	**任务级的重定向：**特定任务的标准描述符可以通过接口ioTaskStdSet()进行重定向，该例程的第一个参数是需要进行重定向的任务ID(ID为0表示自身)，第二个参数是需要重定向的标准描述符，第三个参数是需要重定向到的文件描述符。例如下面的例子，将任务A的标准输出重定向到fileFd描述符:`ioTaskStdSet(0,1, fileFd)`。



## 7.3 VxWorks驱动层次结构

​	VxWorks的I/O框架由ioLib.c文件提供，但是ioLib.c文件提供的函数仅仅是一个最上层的接口，并不能完成具体的用户请求，而是将请求进一步向其他内核模块进行传递，位于ioLib.c模块之下的模块就是iosLib.c。我们将ioLib.c 文件称为上层接口子系统，将iosLib.c文件称为I/O子系统，==注意两者的区别，上层接口子系统直接对用户层可见，而I/O子系统则一般不可见(当然用户也可以直接调用iosLib.c中定义的函数，但是一般需要做更多的封装，且违背了内核提供的服务层次)，其作为上层接口与下层驱动系统的中间层而存在。==下图展示了VxWorks系统驱动层次。

![VxWorks内核驱动层次](https://upload-images.jianshu.io/upload_images/6128001-a7437fe19bb00091.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)

​	从上图可以看出I/O子系统在整个的驱动层次中起着十分重要的作用，其对下管理着各种类型的设备驱动。即：各种类型的设备驱动都必须要向I/O子系统进行注册方可被内核访问。所以在I/O子系统这一层次，内核维护着三个十分关键的数组用以对设备所属驱动、设备本省以及当前系统文件句柄进行管理。

​	需要指出的是，VxWorks文件系统在内核驱动层次中实际上是作为块设备驱动层次中的一个中间层而存在的，其向I/O 子系统进行注册，而将底层块设备驱动置于自身的管理之下以提高数据访问的效率。在这些文件系统中，dosFs 和rawFs 是最常用的两种文件系统类型，在VxWorks早期版本就包含对这两种文件系统的支持。





# 参考文献

[基于VxWorks的嵌入式实时系统设计](http://www.jb51.net/books/261984.html)

[嵌入式实时操作系统的设备驱动-华科硕士论文](http://xueshu.baidu.com/s?wd=paperuri%3A%28ffa25f88f13bba55c07adbbf5bf0f271%29&filter=sc_long_sign&tn=SE_xueshusource_2kduw22v&sc_vurl=http%3A%2F%2Fwww.doc88.com%2Fp-0989361693174.html&ie=utf-8&sc_us=4497328787020817259)

[基于USB接口的数据广播终端的设计与实现-华科硕论文](http://xueshu.baidu.com/s?wd=paperuri%3A%2825dbf283a8139b1033b64cab072e4787%29&filter=sc_long_sign&tn=SE_xueshusource_2kduw22v&sc_vurl=http%3A%2F%2Fwww.doc88.com%2Fp-0522029096613.html&ie=utf-8&sc_us=750599083948907462)

[基于实时操作系统VxWorks的驱动程序的开发](http://xueshu.baidu.com/s?wd=paperuri%3A%28667bc32b4499adc634eb82143ffb30ba%29&filter=sc_long_sign&tn=SE_xueshusource_2kduw22v&sc_vurl=http%3A%2F%2Fwww.doc88.com%2Fp-2793084075526.html&ie=utf-8&sc_us=13782008646235443649)





​	

