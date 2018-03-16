原型：void Q_INIT_RESOURCE(name)

说明：通过初始化使用指定的 ==\<基本名称.qrc\>==文件中指定的资源。通常情况下，Qt的资源在启动的时候自动加载。但是在某一些平台上，资源是贮存在一个静态库中的，因此需要用到`Q_INIT_RESOURCE(name)`这个宏来初始化资源。

> QT应用程序，无论是exe，staticlib，还是dll程序，都可以通过qrc文件来导入各种资源。qrc文件本质上是一个XML文件，里面描述了相应资源的位置。qrc会将这些资源文件转换为相应的代码，参与应用程序的编译。
>
> 这样做的好处就是：简化了应用程序发布时还需要同步发布各类资源文件的烦恼。
>
> 缺点是：应用程序会相对变大。
>
> **注意：** 这个宏不能够在`namespace`当中使用。他应该在`main()`函数中调用。如果没有办法做到在`main()`函数中调用。在MyNamespace::myFunction中可以使用以下的方法来初始化资源。
>
> ```c++
> inline void initMyResource()
> {
>   Q_INIT_RESOURCE(name);
> }
>
> namesapce MyNamespace{
>   //...
>   myFunction(){initMyResource();}
> }
> ```
>
> 



例1：若我们的应用程序中列出了一个名为 myapp.qrc，你可以通过在`main()`函数中加入`Q_INIT_RESOURCE(myapp);`来确保在应用程序在启动时初始化资源。

例2：假如我们创建的是staticlib工程，(staticlib工程的创建是通过 TEMPLATE = lib,LIB = staticlib 的pro文件来实现的)，那么在引用该lib文件的工程当中，我们就需要使用`Q_INIT_RESOURCE`宏来显式的初始化资源。

假设libtest是一个静态库，他通过libtest.qrc文件引入资源；

libtestapp是使用libtest.lib的exe程序，那么在libtestapp中，需要显式的通过Q_INIT_RESOURCE宏来初始化libtest的资源；`Q_INIT_RESOURCE(libtest);`。