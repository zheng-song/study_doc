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
- 在真正的实现文件.cpp当中，我们可以包含`#include <QtGui>`,这个头文件包含了Qt GUI类的定义。Qt由数个模块组成，每一个模块都有自己的类库。最为重要的模块有QtCore、QtGui、QtNetwork、QtOpenGL、QtScript、QtSql、QtSvg、QtXml。其中，在`<QtGui>`头文件中为构成QtCore和QtGui组成部分的所有类进行了定义。在程序中包含这个头文件，就能够使我们省去在每一个类中分别包含的麻烦。在头文件中本可以直接包含`<QtGui>`即可，然而在衣蛾头文件中再包含一个那么大的头文件实在不是一种好的编程风格，尤其对于比较大的工程项目更是如此。


