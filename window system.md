[XFree86、X11、X-window、窗口管理器、GNOME、KDE的关系][1]
窗口管理器和桌面环境是用户在X Window系统里面的主要界面,窗口管理器只是一个程序，他控制窗口的外表，位置和提供用户去操作这些窗口程序的方法。桌面环境为操作系统提供了一个较完整的图形操作界面，和提供了一定范围和用途的实用工具和应用程序。

### [X Window System][2]
* The **X Window System** is a windowing system for bitmap displays,common on UNIX-like computer operating systems.
* X provides the basic framework for a GUI environment:drawing and moving windows on the display device and interacting with mouse and keyboard. **X does not mandate(授权，托管，指示) the user interface--this is handle by individual 
  client programs，Program‘s may use X's graphical abilities with no user interface. As such ,the visual styling of X-based environments varies greatly;different programs may present radically(彻底的，根本的) different interfaces.**
* X is a architecture-independent system for remote graphical user interface and input device capabilities.In its standard distribution it is a complete,albeit(虽然，即使) simple,display and interface solution.
* X provides the basic framework ,or primitives(基础,原语),for building such GUI environments。 
* X was specially designed to be used over network connections rather than on an integral(完整的，整体的) or attached display device. X features network transparency,which means an X program running on a computer somewhere on a network can display its user interface on an X server running on some other computer on the network. ***the X server is typically the provider of graphics resource and keyboard/mouse events to X clients,meaning that the X server is usually running on the computer in front of a human user,while the X client applications run anywhere on the network and communicate with the user's computer to request the rendering of graphics content and receive events from input devices including keyboards and mice***

![simple X server example.png](http://upload-images.jianshu.io/upload_images/6128001-2b05b6a5969b52b5.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)

**To use an X client application on a remote machine,the user may do the following:**

1. on the local machine, open a terminal window.
2. use ssh with the X forwarding argument to connect to the remote machine
3. request local display/input service(e.g.,export DISPLAY=[*user's machine*]:0 if not using SSH with X forwarding enabled)

#### X Window System's Principles

* Do not add new functionality unless an implementor cannot complete a real application without it.

* It is as important to decide what a system is not as to decide what it is.Do not serve all the world's needs;Rather ,make the system extensible so that additional needs can be met in an upwardly compatible fashion.(向上兼容的风格)

* The only thing worse than generalizing from one example is generalizing from no examples at all.

* If a problem not completely understood,it is probably best to provide no solution at all.


* If you can get 90 percent of the desired effect for 10 percent of the work,use the simpler solution.
* Isolate complexity as much as possible(尽可能的隔离复杂性)
* Provide mechanism rather than policy,In particular,place user interface policy in the client's hands.

X primarily(根本上、首要地) defines protocol and graphics primitives ----it deliberately contains no specification for application user-interface design,such as button,menu,or window title-bar styles. instead, application software ---- such as window managements,GUI widget toolkits and desktop environments,or application-specific graphical user interfaces ----defined and provide such details.As a result ,there is no typical X interface and serveral different desktop environments have become popular among users.  



###OpenVMS

OpenVMS is  a multi-user,multiprocessing virtual memory-based OS **designed for use in time-sharing,batch processing and transaction processing**. it may approach real-time OS characteristics.The system offers high availability through clutering and the ability to distribute the system over multiple physical machines. this allow the system to be tolerant against disasters that may disable individual data-processing facilities. 

Enterprise-class evironments typically select and use OpenVMS for various purposes including mail servers,network servers,manufacturing or transportation(传输) control criticial applications and database,and particularly environments where system uptime and data access is critical .



![A high-level architecture overview, showing Glamor's role][3]



------
### XFree86
* XFree86 was an implementation(实现) of the X Window System. It was originally written for Unix-like operating systems on IBM PC compatibles(兼容) and was available for many other operating systems and platforms.XFree86 was rose with the grew of Linux. until February 2004,with version 4.4.0, The **XFree86 Project** adopted a license change that the **Free Software Foundation** considered **GPL incompatible**. Most open source operating systems using XFree86 found this unacceptable and move to a fork from before the license change.  XFree86 has since the license change(version 4.4.0) been superseded(被取代) by **X.org** and is used rarely nowadays.the XFree86 project was confirmed dormant(休眠的，静止的) in December 2011。 

![xFree86 is a display server.png](http://upload-images.jianshu.io/upload_images/6128001-bee5631d0e028436.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)

* The XFree86 server communicates with the host operating system's kernel to drive input and output device ,with the exception of graphics cards. 

* after XFree86 dormant,the **X.org server** become the official reference implementation of X11.the first version,X11R6.7.0,was forked from XFree86 version 4.4 RC2 to avoid the XFree86 license change,with X11R6.6 changes merged in. later,most of the open-source Unix-like operating systems have adopted the X.org Server in place of XFree86.



[1]: http://www.cnblogs.com/Akagi201/archive/2012/03/26/2418202.html
[2]: https://en.wikipedia.org/wiki/X_Window_System

[3]: https://upload.wikimedia.org/wikipedia/commons/thumb/2/2d/The_Linux_Graphics_Stack_and_glamor.svg/1280px-The_Linux_Graphics_Stack_and_glamor.svg.png	"A high-level architecture overview, showing Glamor's role"

