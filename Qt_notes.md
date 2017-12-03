`a.exec()`:表示将应用程序的控制权传递给Qt。此时，程序会进入事件循环状态，这是一种等待模式，程序会等候用户的动作，例如鼠标单击和按键操作等。用户的动作让可以产生响应的程序生成一些事件(event,也称为消息)，这里的响应通常就是执行一个或者多个函数。



## 为什么在头文件中有的是使用前置声明，而有的是包含头文件？

如下代码：

```
#include <QDialog>
class QCheckBox;
class QLabel;

class FindDialog :public QDialog
{
  Q_OBJECT
public:
  FindDialog(QWidget *parent=0);
  
private:
  QCheckBox *checkBox;
  QLabel    *label;
}
```

- 前置声明(forward declaration)会告诉C++编译程序类的存在，而不需要提供类定义中的所有的细节(通常放在它自己的头文件当中)。因为我们的这些私有变量都是指针，而且没有必要在头文件当中就去访问它们，因而编译程序就无需这些类的完整的定义。我们不使用这些类的相关的头文件(如`<QCheckBox>`,`<QLabel>`),而是使用前置声明，这可以使得变异的过程更快一些。
- 在真正的实现文件.cpp当中，我们可以包含`#include <QtGui>`,这个头文件包含了Qt GUI类的定义。Qt由数个模块组成，每一个模块都有自己的类库。最为重要的模块有QtCore、QtGui、QtNetwork、QtOpenGL、QtScript、QtSql、QtSvg、QtXml。其中，在`<QtGui>`头文件中为构成QtCore和QtGui组成部分的所有类进行了定义。在程序中包含这个头文件，就能够使我们省去在每一个类中分别包含的麻烦。在头文件中本可以直接包含`<QtGui>`即可，然而在一个头文件中再包含一个那么大的头文件实在不是一种好的编程风格，尤其对于比较大的工程项目更是如此。




### 为什么在Qt中大多数时候都是只见new 不见delete?

- Qt会在删除父对象的时候**自动的**删除其所属的子对象,包括其所有的子窗口部件和子布局对象.



### 信号和槽机制

- 信号和槽机制是Qt编程的基础.它可以让应用程序的编程人员把这些互不了解的对象绑定在一起.槽和普通的C++成员函数几乎是一样的----可以是虚函数;可以被重载,可以是公有,保护,私有类型,并且也可以直接被其他的C++成员函数直接调用.**它们的参数可以使任意的类型.唯一不同的是:槽还可以和信号连接在一起.**每次发射和槽连接在一起的信号时,就会自动的触发槽函数.
- **一个信号可以连接多个槽:** `connect(slider,SIGNAL(valueChanged(int)),this,SLOT(setValue(int)))`,`connect(slider,SIGNAL(valueChanged(int)),this,SLOT(updateStatusBarIndicator(int)))`.在发射这个信号的时候会以*不确定的顺序*一个接一个地调用这些槽.
- **多个信号可以连接同一个槽函数**
- **一个信号也可以和另外的一个信号相连接:**`connect(slider,SIGNAL(valueChanged(int)),this,SIGNAL(updateRecord(int)));`当发射第一个信号的时候,第二个信号也会被触发.除此之外,信号和信号之间的连接和信号和槽之间的连接是难以区分的.
- **连接可以被移除:**`disconnect(slider,SIGNAL(valueChanged(int)),this,SIGNAL(updateRecord(int)));`这种情况比较少用,因为当删除掉对象时,Qt会自动的移除和这个对象相关的所有的连接.
- **对应的信号和槽必须要有相同顺序和类型的参数,但是允许槽的参数比信号的参数少,信号中多余的参数会被简单的忽略掉.**
- **Qt的信号和槽机制是在QObject中实现的,并不仅仅局限于图形用户界面编程当中,这种机制可以使用于任何的QObject的子类当中.**





### Qt的元对象系统

Qt的主要成就之一就是使用了一种机制对C++进行了扩展,并且使用了这种机制创建了独立的软件组件.这些组件可以绑定在一起,但是任何组件对于他所要连接的组件的情况事先都一无所知.

这种机制称为元对象系统(meta-object system),它提供了关键的两项技术:信号-槽以及内省(introspection).内省功能对于实现信号和槽是必须的.并且允许应用程序的开发人员在运行的时候获得有关QObject子类的"元信息(meta-information)",包括一个含有对象的类名以及他所支持的信号和槽的列表.这一机制也支持属性(广泛用于Qt的设计师中)和文本翻译(用于国际化),并且它也为QtScript模块奠定了基础.

标准C++中并没有对Qt的元对象系统所需要的动态元信息提供支持.Qt通过一个独立的moc工具解决了这个问题,moc解析Q_OBJECT类的定义并且通过C++函数来提供可供使用的信息.由于moc使用纯C++来实现其所有的功能,所以Qt的元对象系统可以在任意的C++编译器上工作.



这一机制的工作过程是:

- Q_OBJECT宏声明了在每一个QObject子类中必须要实现的一些内省函数:metaObject()、tr(),qt_metacall(),以及其他的一些函数.
- Qt的moc工具生成了用于由Q_OBJECT声明的所有函数和所有信号的实现.
- 像connect()和disconnect()这样的QObject的成员函数使用这些内省函数来完成他们的工作.

由于所有的这些工作都是由qmake,moc和QObject自动处理的,所以很少需要再去考虑这些事情.但是如果你对此充满好奇,那么也可以阅读以下有关QMetaObject类的文档和由moc生成的C++源代码文件,可以从中看出这些实现工作是如何进行的.



### [Qt 的事件处理机制](http://mobile.51cto.com/symbian-272812.htm)

- 产生事件: 输入设备，键盘鼠标等。keyPressEvent，keyReleaseEvent,mousePressEvent,mouseReleaseEvent事件(他们被封装成QMouseEvent和QKeyEvent)，这些事件来自底层的操作系统，他们以异步的形式通知Qt事件处理系统。Qt也会产生很多事件，如QObject::startTimer()会触发QTimerEvent。用户的程序还能够自定义事件。
- 事件的接收和处理者：QObject类是整个的Qt对象模型的核心，事件处理机制是QObject三大职责(内存管理、内省(intropection)与事件处理机制)之一。任何一个想要接收并处理事件的对象必须要继承自QObject类，可以选择重载QObject::event()函数或者是事件的处理权转交给父类。
- 事件的派送者：对于non-GUI的Qt程序，由QCoreApplication负责将QEvent分发给QObject的子类-Receiver；对于GUI程序，则由QApplication负责派送。



### [Qt 中的窗口刷新事件](http://mobile.51cto.com/symbian-270250.htm)

- 在窗体刷新事件当中，主要说明一下`paintEvent`的使用：`void QWidget::paintEvent(QPaintEvent *event)`. Paint 这个事件只要是窗体事件需要被重绘了就会被调用，他是由窗体事件产生的,但是要求程序重画窗体部件的时候，事件循环就会从事件队列当中选中这个事件并把它分发到那个需要重画的widget当中。并不是所有的paint事件都是由窗口系统产生的，你也可以使用repaint()和update()来使用它。但是你需要知道的是，因为paintEvent()函数是protected的，你无法直接调用它。它也绕开了任何存在的事件过滤器。因为这些原因，Qt提供了一个机制，直接sending事件而不是posting。
- `void QWidget::update()`:用于更新窗体部件，它规划了所要处理的绘制事件。但是可以被Qt优化，有时`update()`执行之后不一定会直接转到paintEvent。因为Qt会把**多个绘制事件自动的合并成一个**来加快绘制的速度,所以推荐使用这个，而不是`repaint()`,几次调用`update()`的结果通常仅仅是一次`paintEvent()`调用。利用这一点，在实现程序的时候，我们可以把所有的绘制窗体的那些语句、函数...都放到paintEvent中，通过各种if-else语句进行判断来绘制，这样对速度有很好的优化并且可以防止闪烁。
- 绘制事件还有一点需要注意的是：当绘制事件发生时，更新的区域通常被擦除。如果需要在上一次绘制的基础上进行绘制的话，我们的做法是：使用一个临时变量保存着上次绘制之后的图，然后在这个图上进行绘制，最后再直接的显示一下这个图就ok了。这是个比较笨的方法，但也简单。***通过QPaintEvent::erased()可以得知这个窗口部件是否被擦除。写完之后记得检查一下 ，如果在设置了WRepaintNoErase窗口部件标记的时候是不会被擦除的。***

