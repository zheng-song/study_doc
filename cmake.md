http://blog.nemoge.com/trick-for-overload-macro-function-in-c/

http://www.tk4479.net/osean_li/article/details/55057690

cmake 和autotools是makefile的上层工具,它们的目的是为了产生可移植的makefile,并简化自己动手写makefile时的巨大的工作量.这样的项目构建工具能够帮助我们在不同的平台上更好的管理和组织我们的代码以及其编译过程.cmake和autotools是不同的项目管理工具，有各自的特点和用户群.

#### cmake的主要特点

1. 开放源代码,使用类 BSD 许可发布。
2. 跨平台,并可生成 native 编译配置文件,在 Linux/Unix 平台,生成 makefile,在 苹果平台,可以生成 xcode,在 Windows 平台,可以生成 MSVC 的工程文件.
3. 能够管理大型项目,KDE4 就是最好的证明。
4. 简化编译构建过程和编译过程。Cmake 的工具链非常简单:cmake+make。
5. 高效率，按照 KDE 官方说法,CMake 构建 KDE4 的 kdelibs 要比使用autotools来构建 KDE3.5.6 的 kdelibs 快 40%,主要是因为 Cmake 在工具链中没有 libtool。
6. 可扩展,可以为 cmake 编写特定功能的模块,扩充 cmake 功能。

----

#### [cmake中一些预定义的变量](http://blog.csdn.net/wzzfeitian/article/details/40963457/)

- ​


----

#### cmake 用法

1. 安装cmake
2. 在当前工程目录下创建CMakeLists.txt文件

> 该文件用来保存你需要cmake处理的信息,即cmake使用的"代码".输入的内容如下:
> \# cmake最低的版本需求,不加入会收到警告信息
> CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
>
> \# 项目的名称
> PROJECT(HELLO)
>
> \# 把当前目录下的所有的源代码文件和头文件加入变量SRC_LIST
> AUX_SOURCE_DIRECTORY(. SRC_LIST)
>
> \# 生成应用程序hello(在windows下会自动生成hello.exe)
> ADD_EXECUTABLE(hello ${SRC_LIST})
>
> \# 至此CMakeLists.txt文件就完成了

3. 编译项目

> 此时我们使用**外部编译**的方式来编译源文件,在我们的项目目录中新建一个文件夹**build**来存储我们编译过程中生成的中间文件和可执行文件,进入该build目录下,运行***cmake ../***  注意"../" 不能少,因为我们的CMakeLists.txt在当前目录的外面.这个命令会让camke检测编译环境,并生成相应的makefile.之后运行make就可以进行编译.编译生成的所有的中间文件和可执行文件都会在build目录下.
>
> - 之所以要使用这种**外部编译**的方式,是因为cmake生成的makefile无法跟踪所有的中间文件,即.使用make distclean的凡是无法将所有的中间文件删除.因此,为了不跟源文件混淆在一起,我们建立一个新的目录来存放文件,要删除的时候只需将目录下的所有文件删除即可.