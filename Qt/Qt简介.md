# Qt的特点

​	Qt是一个跨平台的C++应用程序框架，支持Windows、Linux、Mac OS X、Android、iOS、Windows Phone、嵌入式系统等。也就是说，Qt可以同时支持桌面应用程序开发、嵌入式开发和移动开发，覆盖了现有的所有主流平台。你只需要编写一次代码，发布到不同平台前重新编译即可。

​	Qt不仅仅是一个GUI库，他除了可以创建漂亮的界面，还有很多其他的组件，例如，你不在需要研究STL，不在需要C++的\<string\>头文件，不再需要去找解析XML、连接数据库、socket的各种第三方库，这些Qt都已经内置了。

​	Qt虽然庞大，封装较深，但是它的速度并不慢，虽然不及MFC，但是比Java、C#要快。Qt程序最终会编译成本地代码，而不是依托于虚拟机。



## Qt可以做什么

​	使用Qt开发的程序非常多。1997年，Qt被用来开发Linux桌面环境KDE，大获成功，使得Qt成为Linux环境下开发C++ GUI程序的事实标准。Linux也是嵌入式的主力军，广泛应用于消费类电子、工业控制、军工电子、电信/网络/通讯、航空航天、汽车电子、医疗设备、仪器仪表等相关行业，这些地方都有 Qt 的影子。

​	在Windows下，GUI的解决方案比较多，基于C++的有Qt、MFC、WTL、wxWidgets、DirectUI、Htmlayout，基于C#的有WinForm、WPF，基于Java的有AWT、Swing，基于Pascal的有Delphi，还有国内初露头角的aardio；若你有Web开发经验，也可以基于Webkit或者是Chromium将网页转换为桌面程序。



## Qt 开发 Windows 桌面程序的优点

- 简单易学：Qt 封装的很好，几行代码就可以开发出一个简单的客户端，而 MFC 封装简陋，还需要了解 Windows API，普遍反映难学。
- 资料丰富：资料丰富能够成倍降低学习成本，否则你只能去看源码，关于 DirectUI、Htmlayout、aardio 的资料就很少。
- 漂亮的界面：Qt 很容易做出漂亮的界面和炫酷的动画，而 MFC、WTL、wxWidgets 比较麻烦。
- 独立安装：Qt 程序最终会编译为本地代码，不需要其他库的支撑，而 Java 要安装虚拟机，C#要安装 .NET Framework。
- 跨平台：如果你的程序需要运行在多个平台下，同时又希望降低开发成本，Qt 几乎是必备的。

## 使用Qt开发的程序

​	WPS、YY语音、Skype、豆瓣电台、虾米音乐、淘宝助理、千牛、暴雪的战网客户端、VirtualBox、Opera、咪咕音乐、Google地图、Adobe Photoshop Album 等。

​	但是，由于Android本身支持Java，iOS本身支持Objective-C和Swift，所以Qt 在移动端的贡献寥寥无几。