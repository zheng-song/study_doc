# QAbstractItemModel

​	`QAbstractItemModel`类为`item model`类提供了一个抽象接口，QAbstractItemModel类定义了item model类必须要使用的标准接口，以便与model/view体系中结构当中的其他组件互操作。它不应该被实例化。相反，你应该将其子类化以创建新模型。

​	 QAbstractItemModel类是Model/View类当中的一个，并且是model/view框架的一部分。它可以作为QML中的Item view元素的底层数据模型或者是Qt Widgets模型的Item视图类(Item view classes)。

​	如果你需要使用一个带有项目视图(item view)的模型，例如QML的列表视图元素(List View element)或者是C++ widgets的QListView或者QTableView，你应该考虑将QAbstractListModel或QAbstractTableModel子类化，而不是直接使用。

​	底层数据模型以表的层次结构的形式展现给视图和委托(views and delegates)。如果你不使用层次模型，那么模型就是一个简单的行和列的表(a simple table of rows and columns)。每一个Item都有一个QModelIndex指定的唯一索引。

![图片.png](https://upload-images.jianshu.io/upload_images/6128001-319aa989fec29c51.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)

​	可以通过模型访问的每一个数据项都有一个关联的模型索引。你可以使用index()函数来获取该模型索引。每一个索引可能有一个(兄弟姐妹)sibling()索引；子条目有一个parent()索引。

​	每一个Item都有许多与之关联的数据元素，并且这些数据元素可以通过指定一个角色(role)(参见Qt::ItemDataRole)给模型的data()函数来获取。使用itemdata()函数可以同时获取所有可获得角色的数据(Data for all available roles)。

​	每一个角色的数据都使用一个特定的Qt::ItemDataRole来设置。单个角色的数据可以使用setData()来单独设置，或者使用setItemData来设置所有的角色。

​	可以使用flags()(参见Qt::ItemFlag)函数来查询Items，以判断他们是否可以通过其他的方式来选择、拖拽或操作。	

​	如果一个item有子对象，hasChild()函数为对应的index返回true。

​	该模型对应于层次结构的每一个层级都有rowCount()函数和columnCount()函数。行和列可以使用insertRows()，insertColumns)(), removeRows(),  removeColumns()来进行插入和删除。

​	该模型可以发射信号来指示改变。例如，