[TOC]

http://blog.csdn.net/leo115/article/details/7532677

​	Qt 4推出了一组新的item view类，它使用Model/View结构来管理数据与表示层的关系。这种结构带来的功能上的分离给了开发人员更大的弹性来定制数据项的表示，它也提供一个标准的model接口，使得更多的数据源可以被这些item view使用。这里对model/view的结构进行了描述，结构中的每个组件都进行了解释，给出了一些例子说明了提供的这些类如何使用。

### Model/View结构

​	Model-View-Controller(MVC)，是从Smalltalk发展而来的一种设计模式，常常被用于构建用户界面。经典设计模式的著作中有这样的描述：

==**MVC由三种对象组成，Model是应用程序对象，View是它的屏幕表示，Controller定义了用户界面如何对用户的输入进行响应。在MVC之前，用户界面的设计倾向于将三者揉合在一起，MVC对他们进行了解耦合，提高了灵活性和重用性。**==

​	假如把view和controller结合在一起，结果就是model/view结构。这个结构依然是把数据存储和数据表示进行了分离，它和MVC都基于同样的思想，但是它更简单一些。这种分离使得在几个不同的view上显示同一个数据成为可能，也可以重新实现新的view，而不必改变底层的数据结构。为了更灵活的对用户的输入进行处理，引入了delegate这个概念。它的好处是，数据项的渲染和编程可以进行定制。

![图片.png](https://upload-images.jianshu.io/upload_images/6128001-682548199d764eeb.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)

如上图所示，model与数据源通讯，并提供接口给结构中的其他组件使用。通讯的性质依赖于数据源的种类和model的实现方式

​	

