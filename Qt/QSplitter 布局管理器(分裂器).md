### QSplitter 布局管理器(分裂器)

​	Splitter可以让用户通过拖拽子部件之间的边界来控制他们的大小。一个Splitter可以控制任意多个子部件。QSplitter的典型用法是创建几个widgets并使用insertWidget()或者addWidget()将他们加入Splitter中。如下：

```C++
QSplitter *splitter = new QSplitter(parent);//默认水平布局
QListView *listview = new QListView;
QTreeView *treeview = new QTreeView;
QTextEdit *textedit = new QTextEdit;
splitter->addWidget(listview);
splitter->addWidget(treeview);
splitter->addWidget(textedit);
```

​	如果在使用insertWidget()或者是addWidget()的时候widget已经在一个QSplitter内部了，那么他会被放置在新的位置(按照insert的顺序)。这个特性可以用来重新排序widget的位置。你可以使用`indexOf()` `widget()` `count()`来获取一个Splitter内部的widgets。

​	默认的QSplitter的布局方向是水平的(side by side);你可以使用`setOrientation(Qt::Vertical)`来使其子部件垂直布局(vertically).通常你可以在`minimumSizeHint()`或者`minimumSize（）`和`maximumSize()`之间任意改变widget的大小。

​	QSplitter默认情况下动态的resize子部件的大小，如果你只想在resize操作的最后才改变子部件的大小，那么你可以调用`setOpaqueResize(false)`来改变这个属性。

Qt的布局管理器除了QLayout及其子类外，还可以使用QSplitter。与QLayout不同的是：

- QSplitter是一个带切分条（splitterhandle）的布局管理器，可以通过setHandleWidth()函数来设置切分条的宽带；
- QSplitter在创建的时候需要指定“orientation”或者在后续通过setOrientation()函数来指定，子窗件按加载顺序进行指定方向排列；
- QSplitter一次只能水平或者垂直分割（相当于QHLayout或QVLayout），不能像QGridLayout一样进行网格分割。

==注意：== 在QSplitter中加入一个QLayout是不允许的(无论是通过setLayout()或者是使得QSplitter成为QLayout的parent)。

### QSplitter的用途

​	QSplitter使得用户可以通过拖动子窗口之间的边界来控制它们的大小，例如：

![图片.png](https://upload-images.jianshu.io/upload_images/6128001-8ad42bc280644861.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)

### QSplitter的添加方法

​	 QSplitter的添加方法有2种：

1. 通过Qt Creator的界面设计工具添加；
2. 直接使用C++代码添加。

其中，方法a最为直观和方便，本文将重点介绍这种方法。而方法2可以见参考资料[^1]。



#### 1. 通过Qt Creator添加QSplitter控件

​	与Push Button等控件的添加方法不同，在“设计”视图左侧的控件列表中，并没有对应的QSplitter控件，而是在上方面的工具栏中，如下图红色圈住的位置所示：

![图片.png](https://upload-images.jianshu.io/upload_images/6128001-7698dd6f8948a695.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)

与上图中左侧的Push Button等控件的使用方法不同的是，QSplitter不可以直接使用拖放的方式将其添加到界面中。根据参考资料[^2][^3]的说明可知，使用QSplitter之前，需要先添加将被分裂的两个控件，然后同时选中它们，这时候上图的QSplitter按钮变成可用状态，点击“水平分裂器”即可将它们进行水平的布局。



#### 2. 取消QSplitter

​	 取消上述的分裂布局的方法是，同时选中已经被分裂的控件，然后点击工具栏上方的“打破布局(B)”按钮即可，如下图所示：

![图片.png](https://upload-images.jianshu.io/upload_images/6128001-3f528e8519237289.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)



#### 3. 动态的改变子窗口的大小

​	默认情况下，使用鼠标拖动分割子窗口间的边界时，子窗口会动态的改变其大小。然而，如果希望在松开鼠标时才改变其大小，可以设置下面的参数，取消其勾选状态即可：

![图片.png](https://upload-images.jianshu.io/upload_images/6128001-56d1d0e23f4a1902.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)



#### 4. 子窗口最小尺寸

在拖动子窗口间的边界线时，有时我们并不希望子窗口的宽度或者高度被缩小到零，因此可以设置子窗口的最小尺寸：

![图片.png](https://upload-images.jianshu.io/upload_images/6128001-8e24f733fa485a2e.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)

 然而，就算是设置了上述值，还不行，还需要将下面的选择去掉勾选状态：

![图片.png](https://upload-images.jianshu.io/upload_images/6128001-25fabe7cb3b0cc19.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)

#### 5. 子窗口比例

​	默认的情况下，QSplliter中各个子窗口的大小等比例的，但是很多时候我们并不希望这样，因此参考资料[^5][^6][^7][^8]都提到如何解决这个问题，但都是直接通过C++代码的方式去实现的。这里主要介绍如何通过Qt Creator的“设计”界面来达到同样的目的。 选中QSplitter中的子窗口，然后设置其sizePolicy属性如下图所示:

![图片.png](https://upload-images.jianshu.io/upload_images/6128001-7e90ca6f15c87864.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)

分别将QSplliter中各子窗口的“水平伸展”值设置为非零的值。此值越大，表示对应的子窗口在QSplliter中的分割比例越大(分割效果要运行程序时才呈现出来)。





[^1]: [QSplitter 学习](http://blog.csdn.net/dreamcs/article/details/8070243)
[^2]: [Using Layouts in Qt Designer](http://doc.qt.io/qt-4.8/designer-layouts.html)
[^3]: [qt 如何把qsplitter 添加到主窗口](http://zhidao.baidu.com/link?url=KOjsJsiuFwtDLKtttUDi1VCF0NfhHpYsf6gs53LfgkkTALmA5PDLsgoayDmHC97_9WVaqVgsJPmjcar11r_Cua)
[^4]: [QSplitter的比例分割问题](http://bbs.csdn.net/topics/360258432)
[^5]: [QT layout 使用总结](http://blog.chinaunix.net/uid-11640640-id-2139896.html) 
[^6]: [QSplitter的比例分割有关问题](http://www.educity.cn/wenda/188837.html)
[^7]: [QSplitter setStretchFactor 函数的理解](http://blog.csdn.net/wangsky2/article/details/38707863)
[^8]: [QSplitter大小](http://blog.csdn.net/tujiaw/article/details/40165059)
[^9]: [请问QSplitter怎么固定大小呢？](http://www.qtcn.org/bbs/simple/?t7752.html)

