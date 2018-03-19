[TOC]

# Q_INIT_RESOURCE宏

原型：Q_INIT_RESOURCE(name)

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

----



#QT_BEGIN_NAMESPACE和QT_END_NAMESPACE宏 

- 定义：

```c++
#define QT_BEGIN_NAMESPACE namespace QT_NAMESPACE{

#define QT_END_NAMESPACE }
```

如果你定义以下的内容：

```C++
#include <qglobal.h>
QT_BEGIN_NAMESPACE
class QAction;
class QMenu;
class QPlainTextEdit;
QT_END_NAMESPACE
```

那么在编译的时候就会变成：

```C++
namespace QT_NAMESPACE 
{
class QAction;
class QMenu;
class QPlainTextEdit;
}
```

QT_BEGIN_NAMESPACE和QT_END_NAMESPACE是Qt自己定义的命名空间。以前Qt4是没有Qt命名空间的，后来才加上的，编译Qt源码的时候会有选项，是否将这些类放到专用的Qt命名空间内，默认是没有的。这就出来问题了，为了统一，如果你的代码在默认没有Qt命名空间的ＳＤＫ中编译，那你就不用在前置声明下面这些类的时候加上命名空间，但如果你在有Qt命名空间的ＳＤＫ中编译，那就得加上命名空间了，为了屏蔽这个差异，使得你的源码能在这两种情况下都进行编译，Qt提供了QT_BEGIN_NAMESPACE宏，这样开发者就省得自己来用程序或宏进行处理了。

```C++
class QDialogButtonBox;
class QPushButton;
class QSqlTableModel;
```

至于说加快编译速度什么的，那是上述三个类的前置声明的作用，这是与Qt无关的（也即与QT_BEGIN_NAMESPACE宏无关）。如果你在头文件中只用到一些类的指针（而非实现），那么就可以不包含这些类的头文件，而只使用上面形式的类型前置声明，而在具体实现时才包含入上面这些类型的头文件。如此，头文件里所包含的头文件减少了，那么使用到这个头文件的程序就能包含更少的内容，而不是每次都包含很多头文件，解析起来都耗时。



----



# Q_OBJECT宏

==The Q_OBJECT macro at the beginning of the class definition is necessary for all classes that define signals or slots.== 只有在你的类定义的时候加入了Q_OBJECT宏，你才可以使用signal和slot机制。比如编写时间接口等程序时，有时会出现如下的问题：在PC端成功编译，但是在开发板端却会显示错误：

```C++
QObject::connect: No such slot QWidget::*******()
QObject::connect:  (sender name:   'unnamed')
QObject::connect:  (receiver name: 'unnamed') 
```

这个时候就必须要在头文件派生类的时候，首先像下面那样引入Q_OBJECT宏：

```C++
class MainWindow : public QMainWindow
{
    Q_OBJECT
    ......
}
```



----

# Qt中的moc

moc是QT的预编译器，用来处理代码中的slot，signal，emit，Q_OBJECT等。
moc文件是对应的处理代码，也就是Q_OBJECT宏的实现部分，在官方文档上有详细说明，与其文件在同一目录下。
今天项目不能编译过去，最后和没改动的解决方案比较了下发现原来是把Generated MOC Files里面少了两个moc**_xxx.cpp文件！！



----



# QMap 和QHash

​	QMap是一个提供基于red-black-tree[^1]字典的临时类，`QMap<Key,T>`是Qt常用的容器类，用于存储\<key,value\>pairs，并且提供了对于一个key对应的value的快速差查找。QMap是一个以==升序键顺序==存储键值对的数据结构。QMap中的键值对**根据key**进行了排序，QMap中的==key类型必须重载`operator < //(小于操作符)`==。

​	QHash一个提供基于hash-table[^2]字典的临时类，`QHash<Key,T>`也是Qt常用的容器类，存储<key,value>pairs,并且提供非常快速的从key到value的查找。

QMap和QHash提供的功能类似，不同点在于：

- QHash提供的查找速度更快，这是因为他们之间的实现算法不同导致的。
- 当使用QHash迭代的时候，内容是任意排序的。而使用QMap的时候，内容是根据key来排序的
- QHash的key的类型必须提供了重载运算符`operator ==()`和一个全局的qHash(Key)函数。而QMap的key类型必须要提供重载运算符`operator <()`来说明排序的规则。



[^1]: A red-black-tree is a kind of [self-balancing binary search tree](https://en.wikipedia.org/wiki/Self-balancing_binary_search_tree) in computer science.Each node of the binary tree has an extra bit, and that bit is often interpreted as the color (red or black) of the node. These color bits are used to ensure the tree remains approximately balanced during insertions and deletions.
[^2]: In computing, a **hash table** (**hash map**) is a data structure which implements an [associative array](https://en.wikipedia.org/wiki/Associative_array) [abstract data type](https://en.wikipedia.org/wiki/Abstract_data_type), a structure that can map keys to values. A hash table uses a [hash function](https://en.wikipedia.org/wiki/Hash_function) to compute an *index* into an array of *buckets* or *slots*, from which the desired value can be found.



