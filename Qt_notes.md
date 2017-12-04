### 为什么在头文件中有的是使用前置声明，而有的是包含头文件？

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


----


### [Qt的元对象系统](http://mobile.51cto.com/symbian-270982.htm)

- Qt的主要成就之一就是使用了一种机制对C++进行了扩展,并且使用了这种机制创建了独立的软件组件.这些组件可以绑定在一起,但是任何组件对于他所要连接的组件的情况事先都一无所知.这种机制称为元对象系统(meta-object system),它提供了关键的两项技术:信号-槽以及内省(introspection).内省功能对于实现信号和槽是必须的.并且允许应用程序的开发人员在运行的时候获得有关QObject子类的**元信息(meta-information)**,包括一个含有对象的类名以及他所支持的信号和槽的列表.这一机制也支持属性(广泛用于Qt的设计师中)和文本翻译(用于国际化),并且它也为QtScript模块奠定了基础.
- 标准C++中并没有对Qt的元对象系统所需要的动态元信息提供支持.Qt通过一个独立的moc工具解决了这个问题,moc解析Q_OBJECT类的定义并且通过C++函数来提供可供使用的信息.由于moc使用纯C++来实现其所有的功能,所以Qt的元对象系统可以在任意的C++编译器上工作.

这一机制的工作过程是:

1. Q_OBJECT宏声明了在每一个QObject子类中必须要实现的一些内省函数:metaObject()、tr(),qt_metacall(),以及其他的一些函数.
2. Qt的moc工具生成了用于由Q_OBJECT声明的所有函数和所有信号的实现.
3. 像connect()和disconnect()这样的QObject的成员函数使用这些内省函数来完成他们的工作.

由于所有的这些工作都是由qmake,moc和QObject自动处理的,所以很少需要再去考虑这些事情.但是如果你对此充满好奇,那么也可以阅读以下有关QMetaObject类的文档和由moc生成的C++源代码文件,可以从中看出这些实现工作是如何进行的.

#### 元对象工具

- 元对象编译器moc(meta object compiler)对C++文件中的类声明进行分析并产生用于初始化元对象的C++代码,元对象包含全部信号和槽的名字以及指向这些函数的指针. moc读取C++源文件,如果发现有Q_OBJECT宏声明的类,他就会生成另外一个C++源文件,这个新生成的源文件中包含有该类的元对象代码. 例如，假设我们有一个头文件mysignal.h，在这个文件中包含有信号或槽的声明，那么在编译之前 moc 工具就会根据该文件自动生成一个名为mysignal.moc.h的C++源文件并将其提交给编译器；类似地，对应于mysignal.cpp文件moc 工具将自动生成一个名为mysignal.moc.cpp文件提交给编译器。
- 元对象代码是signal/slot机制所必须的。用moc产生的C++源文件必须与类实现一起进行编译和连接，或者用#include语句将其包含到类的源文件中。moc并不扩展#include或者#define宏定义,它只是简单的跳过所遇到的任何预处理指令。



----

### [Qt 进程间通信](http://mobile.51cto.com/symbian-270726.htm)

Qt的通信可分为Qt**内部通信**和**外部通信**两大类。对于这两类通信机制及应用场合做如以下分析：

#### 1. Qt内部对象间通信

对于这种内部对象间的通信，QT主要采用了信号和槽的机制。

- **QT**的**信号**和**槽**机制是用来在对象间通讯的方法，当一个特定事件发生的时候，signal会被 emit 出来，slot 调用是用来响应相应的 signal 的。简单点说就是如何在一个类的一个函数中触发另一个类的另一个函数调用,而且还要把相关的参数传递过去.好像这和回调函数也有点关系,但是消息机制可比回调函数有用多了,也复杂多了。例如，实现单击按钮终止应用程序运行的代码`connect(button , SIGNAL(clicked()) , qApp , SLOT(quit()) );`实现过程就是一个button被单击后会激发clicked**信号**，通过`connect()`函数的连接qApp会接收到此信号并执行槽函数`quit()`。在此过程中，信号的发出并不关心什么样的对象来接收此信号，也不关心是否有对象来接收此**信号**，只要对象状态发生改变此信号就会发出。此时槽也并不知晓有什么的信号与自己相联系和是否有信号与自己联系，这样信号和槽就真正的实现了程序代码的封装，提高了代码的可重用性。
- 关键字signals指出随后开始信号的声明，这里signals用的是复数形式而非单数，siganls没有public、 private、protected等属性，这点不同于slots。另外，signals、slots关键字是QT自己定义的，不是C++中的关键字。信号也可采用C++中虚函数的形式进行声明，也可以实现重载.
- **宏定义不能用在signal和slot的参数中,**因为moc工具不扩展#define，因此，在signals和slots中携带参数的宏就不能正确地工作，如果不带参数是可以的。
- **函数指针不能作为信号或槽的参数。**

```
class someClass : public QObject  
{  
	Q_OBJECT  
public slots:  
	void apply(void (*applyFunction)(QList*, void*), char*); // 不合语法  
}; 


typedef void (*ApplyFunctionType)(QList*, void*);   
class someClass : public QObject  
{  
	Q_OBJECT   
public slots:  
	void apply( ApplyFunctionType, char *);  // 合法   
}; 
```

- **信号与槽也不能携带模板类参数。**

```
public slots:  
	void MyWidget::setLocation (pair location); // 不和法

signals:  
	void MyObject::moved (pair location);   //不和法
	


typedef pair IntPair;  
public slots:  
	void MyWidget::setLocation (IntPair location); //合法    
signals:  
	void MyObject::moved (IntPair location); //合法
```





#### 2. Qt 与外部对象间的通信

QT与外部通信主要是将外部发来的消息以事件的方式进行接收处理。外部设备将主要通过socket与QT应用程序进行连接。在此，以输入设备与QT应用程序的通信为例说明QT与外部通信的原理。

- 在QT的应用程序开始运行时，主程序将通过函数调用来创建并启动qwsServer服务器，然后通过socket建立该服务器与输入硬件设备的连接。服务器启动后将会打开鼠标与键盘设备，然后将打开的设备文件描述符fd连接到socket上。等到QT应用程序进入主事件循环时，事件处理程序将通过Linux系统的select函数来检测文件描述符fd的状态变化情况以实现对socket的监听。如果文件描述符fd状态改变，说明设备有数据输入。此时，事件处理程序将会发出信号使设备输入的数据能及时得到QT应用程序的响应。数据进入服务器内部就会以事件的形式将数据放入事件队列里，等待QT客户应用程序接收处理。处理结束后再将事件放入请求队列里，通过服务器将事件发送到相应硬件上，完成外部输入设备与QT应用程序的整个通信过程。



#### 3. Qt 与其外部进程通信

QT可以通过QProcess类实现前端程序对外部应用程序的调用。这个过程的实现首先是将前端运行的程序看成是QT的主进程，然后再通过创建主进程的子进程来调用外部的应用程序。这样QProcess的通信机制就抽象为父子进程之间的通信机制。QProcess在实现父子进程间的通信过程中是运用Linux系统的无名管道来实现的.



####[4. Qt 内部进行间通信](http://mobile.51cto.com/symbian-270721.htm)

-  **QCOP协议**
- **信号-槽（Signal-Slot）机制**
- **FIFO机制**



#### 4. 其他通信方式

般操作系统中常用的进程间通信机制也都可以用于QT系统内部不同进程之间的通信，如消息队列、共享内存、信号量、有名管道等机制。其中信号量机制在QT中已经重新进行了封装；有些机制则可以直接通过操作系统的系统调用来实现。另外，如果我们只是想通过管道或socket来实现较简单的外部通信，也可以重新创建管道或socket来实现自己要求的功能。



----

### [Qt 的事件处理机制](http://mobile.51cto.com/symbian-272812.htm)

- 产生事件: 输入设备，键盘鼠标等。keyPressEvent，keyReleaseEvent,mousePressEvent,mouseReleaseEvent事件(他们被封装成QMouseEvent和QKeyEvent)，这些事件来自底层的操作系统，他们以异步的形式通知Qt事件处理系统。Qt也会产生很多事件，如QObject::startTimer()会触发QTimerEvent。用户的程序还能够自定义事件。
- 事件的接收和处理者：QObject类是整个的Qt对象模型的核心，事件处理机制是QObject三大职责(内存管理、内省(intropection)与事件处理机制)之一。任何一个想要接收并处理事件的对象必须要继承自QObject类，可以选择重载QObject::event()函数或者是事件的处理权转交给父类。
- 事件的派送者：对于non-GUI的Qt程序，由QCoreApplication负责将QEvent分发给QObject的子类-Receiver；对于GUI程序，则由QApplication负责派送。


##### [Qt 事件过滤器](http://mobile.51cto.com/symbian-272256.htm)

Qt事件模型真正强大的特色是一个QObject的实例能够管理另外一个QObject实例的事件.



假设已有一个CustomerInfoDialog的小部件,其包含有一系列的QLineEdit,现在,我们想要使用空格来代替Tab,使得焦点在这些QLineEdit之间进行切换. 

- 方法一: 子类化QLineEdit,重新实现keyPressEvent(),并在keyPressEvent()里面调用focusNextChild().如下:

```
void MyLineEdit::keyPressEvent(QKeyEvent *event)   
{   
     if (event->key() == Qt::Key_Space) {   
         focusNextChild();   
     } else {   
         QLineEdit::keyPressEvent(event);   
     }   
} 
```

但是这样做的缺点也是很明显的,若有很多不同的空间(如QComboBox,QEdit,QSpinBox等),我们就必须要子类化这么多控件,这是一个繁琐的任务.

- 方法二: 安装事件过滤器,让CustomerInfoDialog去管理他的子部件的按键事件.通常在构造函数中安装事件管理器.如下:

```
CustomerInfoDialog::CustomerInfoDialog(QWidget *parent)     
	: QDialog(parent)
{     
//  ...      
     firstNameEdit->installEventFilter(this);  
     lastNameEdit->installEventFilter(this);  
     cityEdit->installEventFilter(this);  
     phoneNumberEdit->installEventFilter(this);  
} 

bool CustomerInfoDialog::eventFilter(QObject *target, QEvent *event)   
{   
     if (target == firstNameEdit || target == lastNameEdit   
             || target == cityEdit || target == phoneNumberEdit) {   
         if (event->type() == QEvent::KeyPress) {   
             QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);   
             if (keyEvent->key() == Qt::Key_Space) {   
                 focusNextChild();   
                 return true;   
             }   
         }   
     }   
     return QDialog::eventFilter(target, event);   
}
```

***Qt 提供五个级别的事件处理和过滤***

1. 重新实现事件.如:mousePressEvent(),keyPressEvent(),paintEvent()...这是最常规的事件处理方法.
2. 重新实现QObject::event().这一般用在Qt没有提供该事件的处理函数时.也就是我们自己增加新的事件.
3. 安装事件过滤器
4. 在QApplication上安装事件过滤器.QApplication上的事件过滤器将会捕获应用程序的所有的事件,而且第一个获得该事件.也就是说事件在发送给其他的任何一个event filter之前发送给QApplication的event filter.
5. 重新实现QApplication的notify()方法.Qt使用notify()来分发事件.要想在任何的事件处理器捕获事件之前捕获事件,唯一的方法就是重新实现QApplication的notify()方法.

例如:实现一个模拟时钟小部件,主要就是使用paintEvent事件.

```
/********************widget.h********************/
#ifndef WIDGET_H
#define WIDGET_H
#include <QWidget>
namespace Ui {
class Widget;
}
class Widget : public QWidget
{
    Q_OBJECT
public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();
protected:
    void paintEvent(QPaintEvent *e);
private:
    Ui::Widget *ui;
};
#endif // WIDGET_H
```

```
/********************widget.cpp*************************/
#include "widget.h"
#include "ui_widget.h"
#include <QtGui>
#include <QTime>
#include <QDebug>
Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    //一秒更新一次，repaint
    QTimer *timer = new QTimer(this);
    connect(timer,SIGNAL(timeout()),this,SLOT(repaint()));
    timer->start(1000);
    this->setWindowTitle(tr("Clock"));
    this->resize(400,400);
}
Widget::~Widget()
{
    delete ui;
}
void Widget::paintEvent(QPaintEvent *e)
{
//时分秒指针颜色
    QColor hourColor(127,0,127);
    QColor minuteColor(0,127,127,191);
    QColor secColor(Qt::black);
    int side = qMin(this->width(), this->height());
    QTime time = QTime::currentTime();
//正式开始绘画
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);//无锯齿
    painter.translate(this->width()/2, this->height()/2);//将坐标系原点移动到正中心
    painter.scale(side/200.0, side/200.0);//
    painter.setPen(Qt::NoPen);
    painter.setBrush(QBrush(hourColor));//绘制特殊图形，用brush
    painter.save();//保存当前painter状态
    painter.rotate(30.0 * (time.hour() + time.minute()/60.0));//坐标系旋转，根据真实时间确定旋转角度
    static const QPoint hourHand[3] = {
        QPoint(7,8),
        QPoint(-7,8),
        QPoint(0,-40)
    };//
    painter.drawConvexPolygon(hourHand, 3);  //画不规则多边形
    painter.restore();//恢复save前原来的坐标系
    painter.setPen(hourColor);//画刻度线，用Pen，不用Brush
    for(int i=0; i<12; i++){
        painter.drawLine(88,0,96,0);//12小时对应的刻度
        painter.rotate(30.0);
    }
//画1-12数字，根据数学半径sin，cos计算所画text的位置
    int textR = 80;     //big ridaus
    int textW = 12;   //width = height
    int textH = 8;
    const double pi = 3.1415926;
    for(int i=0; i<12; i++){
        double angle = 30.0 * i * pi/ 180;
        int x = textR * cos(angle) - textW/2;
        int y = textR * sin(angle) - textH/2;
        //qDebug()<<i<<angle<<x<<y;
        painter.drawText(QRect(x, y, textW, textH), Qt::AlignCenter,
                         QString("%1").arg((i+3)>12?(i+3-12):(i+3)));
    }
//画分，同上
    painter.setPen(Qt::NoPen);
    painter.setBrush(QBrush(minuteColor));
    painter.save();
    painter.rotate(6.0 * (time.minute() + time.second()/60.0));
    static const QPoint minuteHand[3] = {
        QPoint(7,8),
        QPoint(-7,8),
        QPoint(0,-70)
    };
    painter.drawConvexPolygon(minuteHand, 3);
    painter.restore();
    painter.setPen(minuteColor);
    for (int j=0; j<60; j++){
        if ((j%5) != 0){
            painter.drawLine(92, 0, 96, 0);
        }
        painter.rotate(6.0);
    }
//画秒，同上
    painter.setPen(Qt::NoPen);
    painter.setBrush(QBrush(secColor));
    painter.save();
    painter.rotate(time.second() * 6.0);
    static const QPoint secHand[3] = {
        QPoint(3,4),
        QPoint(-3,4),
        QPoint(0,-85)
    };
    painter.drawConvexPolygon(secHand, 3);
    painter.restore();
}

```



----


### [Qt 中的窗口刷新事件](http://mobile.51cto.com/symbian-270250.htm)

- 在窗体刷新事件当中，主要说明一下`paintEvent`的使用：`void QWidget::paintEvent(QPaintEvent *event)`. Paint 这个事件只要是窗体事件需要被重绘了就会被调用，他是由窗体事件产生的,但是要求程序重画窗体部件的时候，事件循环就会从事件队列当中选中这个事件并把它分发到那个需要重画的widget当中。并不是所有的paint事件都是由窗口系统产生的，你也可以使用repaint()和update()来使用它。但是你需要知道的是，因为paintEvent()函数是protected的，你无法直接调用它。它也绕开了任何存在的事件过滤器。因为这些原因，Qt提供了一个机制，直接sending事件而不是posting。
- `void QWidget::update()`:用于更新窗体部件，它规划了所要处理的绘制事件。但是可以被Qt优化，有时`update()`执行之后不一定会直接转到paintEvent。因为Qt会把**多个绘制事件自动的合并成一个**来加快绘制的速度,所以推荐使用这个，而不是`repaint()`,几次调用`update()`的结果通常仅仅是一次`paintEvent()`调用。利用这一点，在实现程序的时候，我们可以把所有的绘制窗体的那些语句、函数...都放到paintEvent中，通过各种if-else语句进行判断来绘制，这样对速度有很好的优化并且可以防止闪烁。
- 绘制事件还有一点需要注意的是：当绘制事件发生时，更新的区域通常被擦除。如果需要在上一次绘制的基础上进行绘制的话，我们的做法是：使用一个临时变量保存着上次绘制之后的图，然后在这个图上进行绘制，最后再直接的显示一下这个图就ok了。这是个比较笨的方法，但也简单。***通过QPaintEvent::erased()可以得知这个窗口部件是否被擦除。写完之后记得检查一下 ，如果在设置了WRepaintNoErase窗口部件标记的时候是不会被擦除的。***


----

### [Qt 多线程](http://mobile.51cto.com/symbian-270693.htm)

- 假如一个类的任何函数在此类的多个不同的实例上,可以被多个线程同时调用,那么这个类被称为是**可重入**的,假如不同的线程作用在同一个实例上,而其仍然能够正常的工作,那么称之为**线程安全**的.大多数的C++类天生就是可重入的,因为它们典型的只是引用成员数据,任何线程可以在类的一个实例上调用这样的成员函数,只要没有别的线程在同一个实例上调用这个成员函数.

> ```
> class Counter{
>   public:
>   	Counter():n(0){};
>   	void increment(){++n};
>   	void decrement(){--n};
>   private:
>   	int n;
> }
> ```
>
> 上面的这个类不是**线程安全**的,因为如果多个线程都试图修改数据成员n,其结果是未定义的.这是因为c++中的++和--操作符不是原子操作。实际上，它们会被扩展为三个机器指令：
>
> - 把变量值装入寄存器.
> - 增加或减少寄存器中的值
> - 把寄存器中的值写回内存
>
> 要使其成为线程安全的类,最简单的方法是使用QMutex来保护数据成员:
>
> ```
> class Counter{
>   public:
>   	Counter():n(0){};
>   	void increment(){QMutexLocker locker(&mutex);++n};
>   	void decrement(){QMutexLocker locker(&mutex);--n};
>   	int value() const{QMutexLocker locker(&mutex);return n;}
>   private:
>   	mutable QMutex mutex;
>   	int n;
> }
> ```
>
> QMutexLocker类在构造函数中自动的对mutex进行加锁,在析构函数中自动的进行解锁.

- 大多数的Qt类是可重入的,非线程安全的.有一些类与函数是线程安全的,它们主要是线程相关的类.如QMutex,QCoreApplication::postEvent().






----


### Qt 之qSetMessagePattern

- 改变默认的消息处理输出. 允许改变qDebug(), qWaring(), qCritical(), qFatal()的输出.

**支持以下占位符:**

|                  占位符                   |                    描述                    |
| :------------------------------------: | :--------------------------------------: |
|               %{appname}               |   QCoreApplication::applicationName()    |
|              %{category}               |                   日志类别                   |
|                %{file}                 |                  原文件路径                   |
|              %{function}               |                    函数                    |
|                %{line}                 |                  源文件所在行                  |
|               %{message}               |                  实际的消息                   |
|                 %{pid}                 |    QCoreApplication::applicationPid()    |
|              %{threadid}               |           当前线程的系统范围ID（如果它可以获得）           |
|                %{type}                 |   “debug”、”warning”、”critical”或”fatal”   |
|            %{time process}             |   “debug”、”warning”、”critical”或”fatal”   |
|              %{time boot}              |              消息的时间，启动进程的秒数               |
|            %{time [format]}            | 消息产生时,系统时间被格式化通过把格式传递至QDateTime::toString()。如果没有指定的格式，使用Qt::ISODate。 |
| %{backtrace [depth=N] [separator=”…”]} |               很多平台不支持，暂略…                |

- 还可以使用条件类型，`%{if-debug}, %{if-info} %{if-warning}, %{if-critical}` 或 `%{if-fatal}`后面跟着一个`%{endif}`。如果类型匹配，`%{if-*}` 和 `%{endif}`之间的内容会被打印。
- 如果类别不是默认的一个，`%{if-category} ... %{endif}`之间的内容将被打印。

例如:
```
QT_MESSAGE_PATTERN="[%{time yyyyMMdd h:mm:ss.zzz t} %{if-debug}D%{endif}%{if-info}I%{endif}%{if-warning}W%{endif}%{if-critical}C%{endif}%{if-fatal}F%{endif}] %{file}:%{line} - %{message}"
```

- 默认的模式是：”%{if-category}%{category}: %{endif}%{message}”。也可以在运行时改变模式，通过设置`QT_MESSAGE_PATTERN`环境变量。如果既调用了 `qSetMessagePattern()`又设置了环境变量`QT_MESSAGE_PATTERN`，那么，环境变量优先。

----

#### 示例:

1. **qSetMessagePattern()**

```
int main(int argc, char **argv)
{
    QApplication app(argc, argv);

    // 改变缺省消息处理程序的输出
    qSetMessagePattern("Message:%{message} File:%{file} Line:%{line} Function:%{function} DateTime:%{time [yyyy-MM-dd hh:mm:ss ddd]}");

    // 打印信息
    qDebug("This is a debug message.");
    qInfo("This is a info message.");
    qWarning("This is a warning message.");
    qCritical("This is a critical message.");
    qFatal("This is a fatal message.");

    ...
    return app.exec();
}
```

输出如下:

```
Message:This is a debug message. File:..\MessagePattern\main.cpp Line:138 Function:main DateTime:[2016-07-06 15:21:40 周三] 
Message:This is a info message. File:..\MessagePattern\main.cpp Line:139 Function:main DateTime:[2016-07-06 15:21:40 周三] 
Message:This is a warning message. File:..\MessagePattern\main.cpp Line:140 Function:main DateTime:[2016-07-06 15:21:40 周三] 
Message:This is a critical message. File:..\MessagePattern\main.cpp Line:141 Function:main DateTime:[2016-07-06 15:21:40 周三] 
Message:This is a fatal message. File:..\MessagePattern\main.cpp Line:142 Function:main DateTime:[2016-07-06 15:21:40 周三]
```



2. **QT_MESSAGE_PATTERN环境变量**

选择：项目 -> 构建环境，添加环境变量：`QT_MESSAGE_PATTERN = [%{type}] %{appname} (%{file}:%{line}) - %{message}`

- 此时遵循环境变量优先的原则.即同时设置了qSetMessagePattern()和QT_MESSAGE_PATTERN环境变量,那么环境变量生效,qSetMessagePattern()不生效.

