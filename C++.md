[TOC]

# C++ primer plus

## 第八章 函数探幽

- 执行函数调用指令时,程序将会在函数调用之后立即存储该指令的内存地址,并将函数的参数复制到堆栈,跳到标记被调用的函数起点的内存单元,执行函数代码(也许还需要将返回值放入到寄存器或者堆栈中),然后跳回到地址被保存的指令处.这种函数调用是需要一定的开销的.
- C++ 内联:编译器会使用相应的函数代码替换函数调用(编译器不一定会满足内联要求,**编译器可能发现该函数过大,或者有递归调用,则会忽略内联请求;也有些编译器没有启用或实现这种特性.**).C++成为内联函数的方法有:

1. 在函数声明前加关键字inline.
2. 在函数定义前加关键字inline.

通常的做法是省略原型,将整个的定义放在本应提供原型的地方.



### 引用变量(&)

引用变量的主要用途是用作函数的参数.引用必须要在声明的时候初始化.而不能像指针那样,先申明再赋值.**C++中允许按引用传递允许被调用的函数能够访问调用函数中的变量(即使该变量是局部变量).**引入引用的主要目的是用于**结构和类**的传递,而不是基本的内置类型.

- **将引用作为返回值:**通常返回值机制将返回值复制到临时存储单元中,然后该临时返回存储单元的内容将会被复制到接收返回值的变量当中.然而,返回引用意味着调用程序将会直接访问返回值,而不需要拷贝.***返回引用时最重要的一点是,应该避免返回当函数终止时不再存在的内存单元的引用.即:***

```C++
const Type &clone(Type &a)
{
	Type tmp;
	tmp = a;
	return tmp;  //不应该出现这样的代码,因为该函数运行完之后tmp的内存不再存在,虽然程序能够通过编译(会有警告),但是执行时会崩溃.所以此时应返回const Type类型(复制返回),而不是const Type &类型.
}
```

该函数返回一个指向临时变量tmp的引用,函数运行完毕以后,他将不再存在.

- **为何将const用于引用返回类型:**使用const并不意味着Type类型本身为const,而是意味着你不能够使用返回引用来直接修改它指向的结构.

> ***将C-风格的字符串用作string对象的引用参数***
>
> `function("abcdef"); void function(const string &str){...};` C++的string类定义了一种char * 到string的转换功能,这使得可以使用C-风格字符串来初始化string对象.同事,对于const类型的引用有一个属性,假设实参的类型与形参不符,但是该实参可以被转换为引用类型,程序将会创建一个正确类型的临时变量,使用转换后的实参值来初始化它,然后传递一个指向该临时变量的引用.所以上面可以将char * 类型或者是const char *类型传递给const string &类型.



**什么时候使用引用?什么时候使用指针?什么时候使用按值传递?一些指导原则如下:**

对于使用传递的值而不作修改的函数:

- 若数据对象很小,如内置数据类型或小型结构,则按值传递
- 若数据对象是数组,则使用指针,因为这是唯一的选择.并将指针声明为const.
- 若数据对象是较大的结构,则使用const指针或者const引用,以提高程序的效率.
- 若数据对象是类对象,则使用const引用.类设计的语义常常要求使用引用,这是C++新增这一项的主要原因,因此,传递类对象的标准方式是按引用传递.

对于要修改调用函数中的数据的函数:

- 若数据对象是内置类型,则使用指针.
- 若数据对象是数组,则只能使用指针.
- 若数据对象是结构,则使用引用或指针
- 若数据对象是类对象,则使用引用.



### 默认参数

C++ 中设置函数的默认参数必须通过函数原型来实现.默认值必须从右向左添加,若要为某个参数设置默认值,则必须要为它右边的所有参数提供默认值.



### 函数重载

函数多态是C++的新功能,默认的参数能让你使用不同数目的参数调用同一个函数,而函数的多态(重载)能够让你使用多个同名的函数.可以通过重载来设计一系列的函数----他们完成相同的工作,但是使用不同的参数列表.C++使用上下文来确定要使用的重载函数版本.

- 函数重载的关键是函数的参数列表----也称为函数的特征标(function signature). 类型的引用和类型本身会被视为是同一种特征标.通过函数的类型并不能重载函数(即只有类型不同,其他都相同不是函数重载,而是错误).

> ***什么是名称修饰***
>
> C++ 使用**名称修饰(name decoration)**或者是**名称矫正(name mangling)**来跟踪每一个重载函数,他会根据函数原型中指定的形参类型对每一个函数名进行"加密",如`long func(int,float);`编译器会将名称转换为不太好看的内部表示,如下`?func@@YAXH@Z`,这样对原始名称进行的修饰将会对参数的数目和类型进行编码,添加的一组符号会随着函数的特征标而异,而修饰时使用的约定随编译器而异. 



### 函数模板

函数模板可以用来创建一个通用的函数，用以支持多种不同的形参，避免重载函数的函数体重复设计。它最大的特点是把函数使用的数据类型作为参数。声明形式为：

 ```C++
template<typename T>
T func(T x,int y)
{
  T x;
  //...
}
 ```

若主函数中有如下的调用语句

```C++
double b;
int a;
func(d,a);
```

那么系统将会使用实参d的数据类型double去替代函数模板中的T生成函数：

```c++
double func(double x, int y)
{
  double x;
  //...
}
```

函数模板只是声明了一个函数的描述即模板，不是一个可以直接执行的函数，只有根据实际情况使用实参的数据类型来代替类型参数描述符之后，才能产生真正的函数。

> 关键字`typename`是后来才加入到C++中的,可以使用关键字`class`替代 ，这时数据类型参数标识符就可以使用所有的C++数据类型类型。typename关键字使得参数T表示类型这一点更为明显;如果需要将同一种算法用于不同类型的函数,请使用模板,如果不考虑向后兼容,则声明类型参数时,应该使用关键字typename而不是使用class.
>
> **注意:**模板函数并不能缩短可执行程序,使用模板最终生成的代码是由编译器帮你生成的,最终的代码不会包含有任何的模板,而只包含了程序生成的实际函数.使用模板的好处是,它使得生成多个函数的定义更为简单,可靠.



#### 函数模板的生成

函数模板的数据类型参数标识符实际上是一个类型参数，在使用函数模板的时候，要将这个形参实例化为确定的数据类型。将类型形参实例化的参数称为模板参数，用模板实参实例化的函数称为模板函数。模板函数的生成就是讲模板函数的类型参数实例化的过程。

**注意：**

1. 函数模板允许使用多个类型参数，但是在template定义部分的每一个形参前必须有关键字typename或class，即：`template<class 标识符1,...,class 标识符n>`
2. 在template语句和函数模板定义语句 <返回值类型> 之间不允许有别的语句。即template语句之后要紧跟函数模板定义。
3. 如果C++编译器不是最新版本,则可能不支持模板.新版本可能支持用关键字typename代替class. 早期的g++版本要求将模板定义放在main()之前.

```C++
template<class T>
  int i;             // 这样定义是错误的，不能在template和函数模板之间有其他的语句
T min(T x, T y)
{
  //...
}
```

3. 模板函数类似于重载函数，但是两者有很大的区别：函数重载时，每一个函数体内可以执行不同的动作，但是同一个函数模板实例化后的函数都必须执行相同的动作。



#### 函数模板的异常处理

函数模板中的模板参数可以实例化为各种类型，但是当实例化模板参数的各模板参数之间不完全一致时，就可能发生错误，如：

```C++
template<typename T>
void min (T &x, T &y){return (x<y)?x:y;}
void func(int i, char j)
{
  min(i,i);
  min(j,j);
  min(i,j);
  min(j,i);//后两个调用是错误的，出错的原因是，在调用的时候，编译器按照最先遇到的实参的类型隐式的生成一个函数模板，并使用它对所有的模板函数进行一致性检查。
}
```

`min(i,j);` 先遇到的实参`i`是整型的，编译器就将模板形参解释为整形，此后出现的模板实参`j` 不能被解释为整型而产生错误，**此时没有隐含的类型转换功能。** 解决此种异常的方法有两种：

1. 采用强制类型转换：如将`min(i,j)` 转换为 `min(i,(int)j)` 

2. 用非模板函数重载模板函数：有两种实现方法。

   - 借用函数模板的函数体：

   ```C++
   //此时只声明非模板函数的原型，他的函数体借用模板函数的函数体，改写如下
   template<typename T>
   void min(T &x, T &y){return (x<y)?x:y;}
   int min(int, int);
   void func(int i, char j)
   {
     min(i,j);
     min(j,i); //此时就不会出错
   }
   ```

  - - 重新定义函数体：就像一般的重载函数一样，重新定义一个完整的非模板函数，他可以带的参数可以随意。C++中，函数模板与同名的非模板函数重载时，应该遵循以下的原则：

    1. 寻找一个参数完全匹配的函数，若找到就调用它。若参数完全匹配的函数多于一个，则这个调用是一个错误的调用。
    2. 寻找一个函数模板，若找到就将其实例化生成一个匹配的模板函数并调用它。
    3. 若上面两条都失败，则使用函数重载的方法，通过类型转换产生参数匹配，若找到就调用它。
    4. 若上面三条都失败，还没有找都匹配的函数，则这个调用是一个错误的调用。




#### 重载的模板

可以像重载常规函数定义一样重载模板定义, 和常规函数一样,被重载的模板的函数特征标必须不同.如:

```C++
template<typename T>
void Swap(T &a, T &b){
  //...
}

template<typename T>
void Swap(T &a, T &b,int c){
  //...
}
```






### 继承

- 继承的一个特性是,基类的引用可以可以指向派生类对象,而无需进行强制类型转换.这种特征的一个实际结果就是,可以定义一个接受基类引用作为参数的函数,调用该函数的时候,可以将基类对象作为参数,也可以将派生类对象作为参数.






## 第九章 内存模型和名称空间

> ***多个库的链接***
>
> C++ 允许每个编译器的设计人员以他认为合适的方式实现名称修饰,因此由不同的编译器创建的二进制模块很可能无法正常的链接.即----两个编译器将会为同一个函数生成不同的修饰名称.名称的不同将会使得连接器无法将编译器生成的函数调用与另外一个编译器生成的函数定义匹配.在链接编译模块的时候,要确保所有对象文件或者是库是由一个编译器生成的.如果有源代码,通常可以使用自己的编译器重新编译源代码来消除链接错误.

- 寄存器变量: 关键字register提醒编译器,用户希望它通过使用CPU寄存器,而不是堆栈来处理特定的变量,从而提供对变量的快速访问(CPU访问寄存器的速度比访问堆栈中的内存的速度快得多).但是编译器不一定会满足这种需求。现代编译器足够聪明，能够对程序自动的优化来使用或不使用寄存器变量。 如果变量被存储在寄存器中，则没有内存地址，因此不能够将地址操作符用于寄存器变量(即使编译器实际上并没有使用寄存器来存储变量)。
- volatile变量：即使程序的代码没有对内存单元进行修改，其值也有可能发生变化。
- mutable变量：即使结构(或类)变量为const，其某个成员也可以被修改，例如：

```C++
struct data{
  char name[30];
  mutable int accesses;
}

const data veep = {"hello",0};
strncpy(veep.name,"nihao",6); // 不允许
veep.accesses++; //允许 ，veep的const限定符禁止程序修改veep的数据，但是accesses成员的mutable修饰符使得其不受这种限制，而可以被修改。
```



再谈const

- 在C++中(不是C中),默认情况下全局变量的链接性为外部的,但是const全局变量的链接性是内部的.在C++看来,全局const就像使用了static说明符一样.若程序员希望某个常量的链接性为外部的,则可以如下使用`extern const int status = 50;`,此时必须在所有使用该常量的文件当中使用extern关键字来声明它.


- C++ 提供了作用于解析符(::),当放在变量或者是函数名称前时，表示使用的是变量或者函数的全局版本。






**函数和链接性:**

默认情况下,函数的链接性为外部,**可以使用static关键字将函数的链接性设置为内部,使之只能够在一个文件中使用,但是必须同时在原型和函数定义中使用该关键字**



**语言链接性:**

在C中,一个名称只对应一个函数,为满足内部需求,C语言编译器可能将spiff这样的函数名翻译成_spiff.这种方法被称为C语言的链接性(C language linkage).但是在C++中同一个名称可能对应多个函数,必须要将这些函数翻译为不同的符号名称.因此C++编译器执行名称矫正或者名称修饰,为重载函数生成不同的符号名称,如将spiff(int);转换为\_spiff\_i,将spiff(double,double);转换为\_spiff\_d\_d;这种方法称为C++语言链接(C++ language linkage).

**在C++中使用C库中预编译的函数,可以使用函数原型来指出要使用的约定.如下:**

```C++
extern "C" void spiff(int);//使用C语言特性
extern "C++" void spiff(int);//显示的指出是C++语言特性
void spiff(int);//通过默认方式指出是C++语言特性
```



###  布局new操作符

new通常是负责在堆(heap)中找到一个足以满足要求的内存块,new还有另外的一种变体称为**布局(placement)new操作符**,它能够让你能够使用指定的位置.可以使用这种特性来设置其内存管理规程或者处理需要通过特定地址进行访问的硬件.

- 要使用new布局特性,首先要包含头文件new,它提供了这种版本的new操作符的原型;然后将new操作符用于提供了所需地址的参数.除了需要指定参数以外,其余的用法与常规的new操作符相同

```c++
#include <new>
struct chaff{
  char dross[20];
  int slag;
};
char buffer1[50];
char buffer2[200];
int main()
{
  chaff *p1,*p2;
  int *p3,*p4;
  p1 = new chaff;
  p2 = new int[20];
  
  p3 = new (buffer1) int[20]; //从buffer1分配空间给包含20个元素的数组
  p4 = new (buffer2) chaff;//从buffer2分配空间给结构chaff
}

```

- 布局new操作符使用传递给他的地址,他不跟踪哪一些内存单元已经被使用,也不查找未使用的内存块.
- 布局管理new操作符申请的内存不一定能使用delete来释放,在上面的例子中,buffer指定的内存是静态内存,delete只能用于释放堆内存,所以上面的例子不能使用delete来释放. 但是如果这个buffer不是静态创建的,而是动态创建的,那么可以使用常规delete来释放整个内存块.将布局操作符用于类对象时会更复杂.





### 名称空间

C++通过定义一种新的声明区域来创建命名的名称空间,这样做的目的之一是提供一个声明名称的区域.一个名称空间中的名称不会与另外一个名称空间中的相同名称发生冲突.同时允许程序中的其他部分使用该名称空间中声明的东西.

- 名称空间可以使全局的,也可以位于另外一个名称空间中,但不能位于代码块中.因此,默认情况下在名称空间中声明的名称的链接性为外部的.

**using 声明和using 编译指令:**

C++提供两种机制来(using 声明和using编译指令)来简化对名称空间中名称的使用.using声明使得特定的标识符可用,using编译指令使整个名称空间可用.例如`using namespace std;`和`using std::cout;`

- C++ 标准不赞成在名字空间和全局作用域中使用关键字static;C++中建议代码如下写:

```C++
namespace { int counts;} // 使用未命名的空间
int main(){...}

//而不是使用static
static int counts;
int main(){...}
```





## 第十章 对象和类



**const 成员函数**

- 如何保证成员函数不会修改调用对象在C++中需要特殊的方法来实现,因为其成员可能并没有参数,因此就无法使用const修饰符来对参数进行限定,而该成员函数的内部是可以修改对象的私有数据的.于是在C++中使用了如下的方法来定义const成员函数:`void classTypename::show() const{...}`以这种方式定义和声明的类函数被称为const成员函数.只要类方法不修改调用的对象,就应该将其声明为const.



**作用域为整个类的常量:**

- 通常我们想要在类中声明作用域为整个类的常量,但是像下面的这种方式是不行的.

```C++
class Stock
{
private:
	const int len = 30 ; //错误的声明方式,因为类的声明只是描述了对象的形式,并没有真正的创建对象.因此,在被对象创建之前,将没有用于存储值的空间.
  	char company[len];
};
```

可以使用如下的方法来完成这一任务:

1. 在类中声明一个枚举,在类的声明中声明的枚举的作用域是整个类,因此可以使用枚举为整形常量提供作用域为整个类的符号名称.

```C++
class Stock{
  enum{Len=30,Total=50};
  char company[Len];
};
```

***注意,这种方式声明的枚举并不会创建类数据成员,即所有的对象中都不会包含有枚举.在作用域为整个类的代码中遇到Len时,编译器将会用30来代替它.***由于此处的枚举只是为了创建符号常量,并不打算创建枚举类型的变量,因此不需要提供枚举名.

2. 使用static关键字:

```C++
class Stock{
  static const int Len = 30;
  char company[Len];
};
```

这将会创建一个名为Len的常量,这个常量将会与其他静态变量存储在一起,而不是存储在对象当中.因此,只有一个Len常量,他被所有的Stock对象共享.



### 操作符重载

要重载操作符,需要使用被称为操作符函数的特殊函数形式.操作符函数的格式为:`operator op(argument-list)`假设有一个类A,并为他定义了一个operator +()成员函数,以重载+操作符.以便能够将两个A对象的销售额相加a,b,c都是A的对象,那么可以写这样的等式 `a = b + c`,编译器发现操作数是A类对象,会使用相应的操作符 函数替换上述操作符: `a = b.operator+(c)`;该函数会隐式的调用b(因为是b调用了方法),显示的调用c对象(作为参数被传递).

#### 重载限制

重载的操作符(有些例外)不必是成员函数,但是必须要至少有一个操作数是用户定义的类型.

- 重载后的操作符必须至少有一个操作数是用户定义的类型,这将防止用户为标准类型重载操作符.因此,不能够将减法操作符重载为计算两个double值的和,而不是他们的差.
- 使用操作符时不能够违反原来的操作符的句法规则,例如不能讲求模运算符%重载为只有一个操作数. 也不能够修改操作符的优先级.
- 不能定义新的操作符. 如不能定义operator**()函数来表示求幂.
- 不能重载下列操作符:
    1. sizeof
    2. . ---- 成员操作符
    3. .* ----成员指针操作符
    4. :: ----作用域解析操作符
    5. ?: ----条件操作符
    6. typeid ----一个RTTI操作符
    7. const_cast ----强制类型转换操作符
    8. dynamic_cast ----强制类型转换操作符
    9. reinterpret_cast ----强制类型转换操作符
    10. static_cast ----强制类型转换操作符






### 友元

C++对控制对类的私有部分的访问,通常公有类方法提供访问的唯一途径,但是有时候这种的限制太严格,以至于不适合特定的编程问题.此时C++提供了另外一种形式的访问权限:

- **友元函数：** 创建友元函数首先要将其原型放在类的声明当中，并在原型的前面加friend关键字。然后在类体之外编写函数的定义，因为其不是类成员函数，所以不需要类的域限定符，定义时也不需要使用friend关键字。


- **友元类:** 
- 友元成员函数


- ​

> \* 若要为类重载操作符，并将非类的项作为第一个操作数，则可以使用友元函数来反转操作数顺序。
>
> \* 只有在类声明中的原型才能够使用friend关键字。除非函数的定义也是实现，否则不能够在函数定义中使用该关键字。



### 重载操作符：作为成员函数还是非成员函数

对于很多操作符而言，可以选择适用成员函数或者是非成员函数来实现操作符重载。根据类的设计，使用非成员函数版本可能更好一些( **尤其是为类定义类型转换时** )，但是对于某些操作符而言，成员函数是惟一的选择("="，"( )"，"[ ]"，"->")。



### 类的自动类型转换和强制类型转换

可以将类转换为与基本类型或者是其他的类型相关，使得从一种类型到另外一种类型是有意义的。可以指示如何进行自动类型转换或者是通过强制类型转换。

- 只有接受一个参数的构造函数才能够作为隐式的转换函数，将构造函数作为自动的类型转换函数似乎是一项不错的选择，但是这种自动特性并不总是我们需要的，因为这会导致意外的类型转换。可以在构造函数前使用关键字`explict`来关闭这种特性。



#### 转换函数

构造函数可以用于从某种类型到类类型的转换，但是要进行相反的转换时，必须使用C++的特殊操作符函数----转换函数。转换函数是用户定义的强制类型转换，可以像使用强制类型转换那样使用它们。



创建转换函数：假设转换为`typeName`类型， `operator typeName();`

- **转换函数必须是类方法：** 
- **转换函数不能指定返回值类型：** `typeName`指出了要转换成的类型，因此不需要有返回类型。
- **转换函数不能有参数：** 它通过类对象来调用，从而告知函数要转换的值，因此，函数不需要参数。

原则上来讲，最好使用显示转换，而避免隐式转换，关键字`explict`的不能用于转换函数，但是只需要一个功能相同的非转换函数替换该转换函数即可：

```C++
Stonewt::operator int();//例如将这个函数函数替换为下面的形式。
int Stonewt::stonewt_to_int();
```

***应该要谨慎的使用隐式类型转换函数。通常最好选着仅仅被显示的调用时才会执行的函数。***

##### C++为类提供的类型转换如下：

- 只有一个参数的类构造函数将用于该类型与该参数相同的值转换为类类型。不过在构造函数声明中使用`explict`关键字可以防止该隐式转换，而只允许显示转换。
- 被称为转换函数的特殊成员操作符函数，将用于类对象转换为其他类型。转换函数是类成员，没有返回类型，没有参数，名为`operator typeName()`.typeName是被转换成的类型。将类对象赋给typeName类型变量或者是将其强制转换为typeName类型时，该转换函数将自动被调用。



#### 在主函数main()之前调用Bootstrap函数

在所有的可执行程序中，调用的第一个函数通常都是其入口main(),但是可以使用一些技巧来修改这种行为。**全局对象(即具有文件作用域的对象)**能够满足这种要求，因为全局对象将在程序的主函数被调用之前创建。程序员可以创建一个类，其默认构造函数将会调用所有的bootstrap函数。例如，它们初始化对象的不同数据部分。这样便可以创建一个全局对象。如下代码所示：

```C++
class CompileRequirements
{
  private:
  //essential information
public：
  CompileReauirements()
  {
    GetDataFromeSales();
    GetDataFromeManufacturing();
  	GetDataFromeFinance();
  }
};

// Instance of Req class has global scope
CompileRequirements Req; // uses default constructor

int main()
{
  // read req and build schedule
  BuildScheduleFromReq();
  
  // rest of program code
}
```



## 第十二章 类和动态内存分配

静态类成员的初始化化如：

```C++
class Test
{
private:
  char *str;
  int len;
  static int num;
public:
  Test();
};

int Test::num = 0; // 此处将静态成员初始化为0，注意不能在类的声明当中初始化静态成员变量，这是因为声明描述了如何分配内存，但是并不分配内存。静态类成员可以在类声明之外使用单独的语句来进行初始化，这是因为静态类成员是单独存储的，而不是对象的组成部分。但是如果静态数据成员是整型或枚举型const，则可以在类声明中初始化。

Test::Test():str(NULL),len(0)
{
 num++;
  //...
}
```



#### 使用显示复制函数来解决可能出现的问题（深拷贝和浅拷贝）

- **深度复制：** 即复制构造函数应当复制字符串，并将副本的地址传递给本对象的指针，而不是仅仅复制字符串的地址。这样每一个对象都有自己的字符串，而不是引用别人的字符串。***必须定义复制构造函数的原因在于，一些成员是使用new初始化的，指向数据的指针，而不是数据本身。***

#### 赋值操作符

C++允许类对象赋值是通过自动为类重载复制操作符实现的。将已有的对象赋值给另外一个对象时，将会自动的调用重载的赋值操作符

- 与赋值构造函数相似，赋值操作符的隐式实现也对成员进行逐个复制。如果成员本身就是类对象，则程序将使用为这个类定义的赋值操作符来复制该成员。***所以如果类成员当中含有指针时，就不应该使用默认的赋值操作符，而应该使用自己定义的赋值操作符来实现（进行深度复制）。***其实现与赋值构造函数相似，但是也有一些区别。

1. 由于目标对象可能引用了以前分配的数据，所以函数应该使用delete[]来删除这些数据
2. 函数应该避免将对象赋给自身；否则给对象重新赋值之前，释放内存操作可能会删除对象的内容。
3. 函数返回一个指向调用对象的引用。

```C++
//例如
StringBad & StringBad::operator=(const & string)
{
  if(this == &string)
    return *this; // 若是自己赋给自己则返回。
  delete[] str; //删除原先分配的空间
  len = std::strlen(string);
  str = new char[len+1];
  std::strncpy(str,st.str,len+1);
  return *this;
}
```

#### 静态类成员函数

可以将成员函数声明为静态的（函数的声明中必须包含关键字static，如果函数的定义是独立的，则其中不能包含关键字static），这样做的后果有两个：

1. **不能通过对象调用静态成员函数：** 实际上，静态成员函数甚至不能够使用this指针。若静态成员函数实在共有部分声明的，则可以使用类名和作用域解析操作符来使用它。例如：

```c++
//类中的声明为
static int number;
static int howMany();

//初始化方式为：
int String::number = 0;
int String::howMany(){return number;}

//调用方式为：
int count = String::howMany();
```

2. **由于静态成员函数不与特定的对象相关联，因此只能够使用静态数据成员** 同时，也可以使用静态成员函数设置类级(classwide)标记，以控制某一些类接口的行为。例如：类级标记可以控制显示类内容的方法所使用的格式？？？？？？？？？？？？



#### 将布局操作符new用于类对象和内置类型时的不同之处

将new布局用于类对象时，假设将类Test布局于buffer[1024]中时，若使用delete[]删除了该buffer,此时不会为使用了new布局的类对象调用析构函数。

- **程序员必须负责管理布局new操作符从中使用的缓冲区内存单元**要使用不同的内存单元，程序员需要提供两个位于缓冲区的不同地址，并确保这两个内存单元不重叠。
- **使用new布局操作符来为对象分配内存时,必须要确保其析构函数被调用:** delete可以与常规的new操作符配合使用,但是不能够与new操作符相配合使用.此时的解决方案就是显示的为使用布局new操作符创建的对象调用析构函数.**这是几种需要显示调用析构函数的特殊情形之一.**
- 对于使用布局new操作符创建的对象,应该以与创建相反的顺序进行删除.原因在于晚创建的对象可能依赖于早创建的对象.另外,仅仅当所有的对象都被销毁之后,才能够释放用于存储这些对象的缓冲区.





#### 成员初始化列表的句法

如下：

```C++
Classy::Classy(int n,int m):mem1(1),mem2(0),mem3(n*m+2+mem1){
  //...
}
```

- 这种格式的初始化只能用于构造函数
- 必须用这种格式来初始化非静态const数据成员
- 必须用这种格式来初始化引用数据成员
- 数据成员被初始化的顺序与它们出现在类声明中的顺序相同，与初始化器中的排列顺序无关。








## 第十三章 类继承

通过类继承可以完成:

- 在已有类的基础上添加新的功能
- 添加新的数据成员
- 修改类方法的行为

派生类不能够直接访问基类的私有成员,必须使用基类的公有方法来访问私有的基类成员,派生类的构造函数必须调用基类的构造函数(使用成员初始化列表来完成这一工作,若在成员初始化列表中省略了基类的构造函数,则会使用默认的基类构造函数来初始化.).



### 派生类和基类之间的特殊关系

1. 派生类可以使用基类的方法,条件是该方法不是私有的.
2. 基类的指针可以在不进行显示类型转换的情况下指向派生类的对象,基类引用可以在不进行显示转换的情况下引用派生类对象.不过基类指针或引用只能够调用基类方法.也不能够将基类对象和地址赋给派生类引用和指针.



### 继承 ---- is-a关系

派生类和基类之间的特殊关系是基于C++继承的底层模型的.



### 多态公有继承

若希望同一个方法在派生类和基类中的行为不同,即方法的行为取决于调用该方法的对象.这种较复杂的行为称为多态(同一种方法的行为将随着上下文而异).有两种方法可以实现多态公有继承:

1. 在派生类中重新定义基类的方法
2. **使用虚方法:** 如果方法不是virtual的,那么程序将会根据引用的类型或者是指针的类型选择方法;如果是该方法是virtual的.那么程序将会根据引用或者是指针指向的对象的类型来选择方法.经常会在基类中将派生类会重新定义的方法声明为虚方法.方法在派生类中声明为虚拟的之后,它在派生类中将自动成为虚方法.

> 若要在派生类中重新定义基类的方法,通常应该将基类的方法声明为虚拟的.这样,程序将根据对象类型而不是引用或者指针的类型来选择方法版本.为基类声明一个虚拟析构函数也是一种惯例.



#### 静态联编和动态联编

程序调用函数时,将使用哪一个可执行代码块?将源代码中的函数调用解释为执行特定的函数代码块称为函数名联编(binding).在C中,每个函数名都对应一个不同的函数.在C++中,由于函数重载的原因,这一任务变得复杂.编译器必须要查看函数参数以及函数名才能够确定使用哪一个函数.然而,C/C++编译器可以在编译过程中完成这种联编.在编译过程中进行的联编被称为**静态联编(static binding)** ,又称为**早期联编(early binding)** ,不过,虚函数使得这项工作变得困难,使用哪一个函数可能在编译时是无法确定的,因为编译器不知道用户将会选择哪一种类型的对象.所以,编译器必须要能够生成在程序运行时选择正确的虚方法的代码.这被称为**动态联编(dynamic binding)** ,又称为**晚期联编(late binding)** .

在C++中,动态联编与指针和引用调用的方法有关.将派生类引用或者指正转换为基类的引用或者指针被称为向上强制转换(upcasting).这使得公有继承不需要显示类型转换.

- 向上强制类型转换(upcasting)是可传递的.即如果ClassA派生出ClassAPlus,而ClassAPlus派生出ClassAPlusPlus,那么ClassA的指针或引用可以引用ClassA对象,ClassAPlus对象,ClassAPlusPlus对象.隐式向上强制类型转换使得基类指针或者引用可以指向基类对象或者派生类对象,因此需要动态联编.C++使用虚成员函数来满足这种需求.

将基类指针转换为派生类指针或引用称为向下强制类型转换(downcasting),如果不使用显式类型转换,则向下类型转化是不允许的.编译器对非虚拟的方法使用静态联编,对虚拟的方法使用动态联编.



> ***虚函数的工作原理.***
>
> C++规定了虚函数的行为,但是实现方法留给编译器的作者.对于我们只需要知道使用虚函数即可,但是了解虚函数的工作原理将有助于更好的理解概念. 通常,编译器处理虚函数的方法是:给每一个对象添加一个隐藏成员.隐藏成员中保存了一个指向函数地址数组的指针.这种数组称为虚函数表(virtual function table),虚函数表中存储了为类对象进行声明的虚函数的地址. 例如: 基类对象包含一个隐藏指针,该指针指向基类中所有虚函数的地址表.派生类对象将包含一个指向独立地址表的指针.如果派生类提供了虚函数的新定义,该虚函数表将保存新函数的地址;如果派生类没有重新定义虚函数,该虚函数表将保存函数原始版本的地址. 如果派生类定义了新的虚函数,则该函数的地址也将被加入到虚函数表当中. 注意,无论类中包含的虚函数是1个还是10个,都只需要在对象中添加1个地址成员,只是表的大小不同而已.
>
> 调用虚函数时,程序将查看存储在对象中的虚函数表地址,然后转向相应的函数地址表. 如果使用类声明中定义的第一个虚函数,则程序将会使用数组中的第一个函数地址. 并执行具有改地址的函数. 如果使用类声明中的第三个虚函数,程序将会使用地址为数组中第三个元素的函数.
>
> 使用虚函数时,在内存和速率方面有一定的成本,包括:
>
> - 每个对象都将增大,增大量为存储地址的空间
> - 对每一个类,编译器都将会创建一个虚函数地址表(数组)
> - 每一个函数调用都需要执行一步额外的操作,即,到表中查找地址
>
> 虽然非虚函数的效率比虚函数的要稍高,但是不具备动态联编功能.

虚函数的一些要点:

- 在基类方法的声明中使用关键字virtual可以使得该方法在基类以及所有的派生类(包括从派生类派生出来的类)中是虚函数.
- 如果使用指向对象的引用或者是指针来调用虚方法,程序将会使用为对象类型定义的方法,而不使用为引用或者是指针类型定义的方法.这称为动态联编或者是晚期联编. 这样基类的指针或引用就可以指向派生类的对象.
- 如果定义的类将被用作基类,则应该将那些要在派生类中重新定义的类方法声明为虚拟的.
- 构造函数不能是虚函数
- 析构函数应当是虚函数,除非类不用做基类.即使不用做基类,也可以给该类定义一个虚析构函数,这只是一个效率问题.
- 友元不能是虚函数,因为友元不是类成员,而只有成员才能是虚函数.
- 如果派生类没有重新定义函数,将会使用该函数的基类版本.如果派生类位于派生链当中,将会使用最新的虚函数版本.例外的情况是基类版本是隐藏的(见下面的介绍).

> ```C++
> class Delling{
> public :
> 	virtual void showperks(int a)const;
>   //...
> };
>
> class Hovel:public Delling{
> public :
> 	virtual void showperks()const;
>   //...
> };
> ```
>
> 如上所示,新定义的showperks()定义为一个不接受任何参数的函数.重新定义并不会生成该函数的两个重载版本,而是隐藏了一个参数为int的基类版本. 重新定义继承的方法并不是重载. 如果在派生类中重新定义函数,将不是使用相同的函数特征标覆盖基类声明,而是隐藏同名的基类方法,不管参数特征标如何.
>
> 1. 如果重新定义继承的方法,应该确保与原来的原型一致,但是如果返回类型是基类引用或者是基类指针,则可以修改为指向派生类的引用或指针. 这种特性被称为返回类型协变(covariance of return type),因为允许返回类型随类类型的变化而变化;
> 2. 如果基类声明被重载了,则应该在派生类中重新定义所有的基类版本.如果只定义了一个版本,则其他的版本将会被隐藏,派生类的对象将无法使用它们. 若不需要修改,则新的定义可以只调用基类版本.



### 访问控制: protected

关键字protected和private相似,在类外只能用公有的成员函数来访问protected部分中的类成员. **private和protected之间的区别只有在基类派生的类中才会表现出来.** 派生类的成员可以直接访问访问基类的保护成员,但是不能够直接访问基类的私有成员,因此对于外部来说,保护成员的行为与私有成员相似;但是对于派生类来说,保护成员的行为与公有成员相似.

> **最好对类的数据成员使用私有的访问控制,不要使用保护访问控制,同时通过基类方法使得派生类能够访问基类的数据. 然而对于成员函数而言,使用保护访问控制很有用,它能够让派生类能够访问公众不能够使用的内部函数.**



> ***单设计模式***
>
> 希望有且仅有一个类的实例返回给调用程序时,就可以使用单元素模式(Singleton pattern),这种类的声明如下:
>
> ```C++
> class TheOnlyInstance{
> public:
>   static TheOnlyInstance *GetTheOnlyInstance();
>   //other methods
> protected:
>   TheOnlyInstance(){}
> };
>
> TheOnlyInstance * TheOnlyInstance::GetTheOnlyInstance()
> {
>   static TheOnlyInstance objTheOnlyInstance;
>   return &objTheOnlyInstance;
> }
> ```
>
> 通过将构造函数声明为protected,并省略公有的构造函数,可以防止局部实例被创建, 只能通过静态方法GetTheOnlyInstance来访问类.该方法被调用的时候,将会返回类TheOnlyInstance的实例. GetTheOnlyInstance方法仅仅在第一次调用的时候创建TheOnlyInstance类的第一个实例.以这种方式构建的静态对象一直有效,直到程序终止,此时这种静态对象将被自动释放.要检索指向这个类的唯一一个实例的指针,只需要调用静态方法GetTheOnlyInstance,该方法返回单对象的地址.
>
> `TheOnlyInstance *pTheOnlyInstance = TheOnlyInstance::GetTheOnlyInstance();`
>
> 因为静态变量在函数调用结束之后仍然存在于内存当中,所以以后在调用GetTheOnlyInstance时,将会返回同一个静态对象的地址.



### 抽象基类

**纯虚函数:** 当类的声明当中包含有纯虚函数时,则不能创建该类的对象,包含有纯虚函数的类只能够作为基类. 在虚函数的原型中 =0 使得其成为纯虚函数. 但是C++甚至允许纯虚函数有定义. 





### 继承和动态内存分配

如果基类使用了动态内存分配,并重新定义了赋值和复制构造函数,那么将会如何影响派生类的实现? 这取决于派生类的属性. 如果派生类也使用动态内存分配,那么就需要学习几个新的小技巧.

1. 派生类不使用new: 见C++ primer Plus p460
2. 派生类使用new: 见C++ primer Plus p460







## C++中的代码重用

C++的一个主要目标是促进代码重用,公有继承是实现这种目标的机制之一,但是不是唯一的机制. 本章介绍其他方法,一种是包含(containment)、组合(composition)或层次化(layering)。另外一种方法是使用私有数据或保护继承。多重继承能够使用两个或者是更多的基类派生出新的类，将基类的功能组合在一起。



#### valarray 类

valarray类由头文件vlaarray支持，这个类用于处理数值(或者是具有类似特性的类)，**valarray被定义为一个模板类,以便能够处理不同的数据类型.** 声明valarray类的方式如下:

`valarray<int> q_values;` 或者 `valarray<double> weights;`



#### 私有继承

C++还有另外一种实现has-a关系的方法----私有继承. 使用私有继承,基类的公有成员和保护成员都将作为派生类的私有成员,这意味着基类的方法将不会成为派生对象公有接口的一部分,但是可以在派生类的成员函数中使用它们.

包含将对象作为一个命名的成员对象添加到类中,而私有继承将对象作为一个未被命名的继承对象添加到类中. 我们使用术语**子对象(subobject)**来表示通过继承或者是包含添加的对象.

因此,私有继承提供的特性与包含相同:获得实现,但是不获得接口.所以,私有继承也可以用来实现has-a关系.



#### 保护继承

使用保护继承时,基类的公有成员和保护成员都将成为派生类的保护成员.和私有继承一样,基类的接口在派生类中也是可用的,但是在继承结构层次之外是不可用的.当从派生类派生出另外一个类时,私有继承和保护继承之间的区别便显示出来. 使用私有继承时第三代类将不能使用基类的接口,因为基类的公有方法在派生类中变成了私有方法;使用保护继承时,基类的公有方法在第二代中将变成受保护的.,因此第三代派生类中可以使用它们.



#### 使用using重新定义访问权限

使用保护派生或者是私有派生时,基类的公有成员将成为保护成员或者是私有成员.假设要让基类的方法在派生类的外面可用,有以下两种方法:

1. **定义一个使用该方法的派生类方法;** 假设Student类能够使用valarray类的sum方法,可以在Student类的声明当中声明一个sum()方法,如下:

```C++
class Student:private std::valarray<double> , std::string{
public:
  double sum()const{return std::vararry<double>::sum();} 
  // 这样Student对象就可以调用Student::sum(),后者将再调用valarray<double>::sum()方法应用于被包含的valarray对象
}
```

2. **将函数调用包装在另外一个函数调用当中;** 即使用一个using声明(就像名称空间那样)来指出派生类可以使用特定的基类成员,即使采用的是私有派生.例如,假设希望通过Student类能够使用valarray的方法min()和max(),可以在student.h的公有部分加入如下using声明:

```C++
class Student:private std::valarray<double>,private std::string{
public:
  using std::valarray<double>::min;
  using std::valarray<double>::max;
}
```

上述using声明使得valarray< double >::min()和valarray< double >::max()可用,**注意,using声明只是用成员名----没有圆括号,函数特征标和返回类型.**





### 多重继承(Multi Inherit)

MI的两个主要问题是:

1. 从两个不同的基类继承同名方法;
2. 从两个或者更多的相关基类那里继承同一个类的多个实例;



####  虚基类

虚基类使得从多个类(它们有相同基类)派生出的对象只继承一个基类对象.如下:

```C++
class Singer:virtual public Worker{...};
class Waiter:public virtual Worker{...};
class SingingWaiter:public Singer,public Waiter{...};
```

##### 新的构造函数规则:

使用虚基类时,需要对类构造函数采用一种新的方法,对于非虚基类,唯一可以出现在初始化列表中的构造函数是即时基类构造函数,但是这些构造函数可能需要将信息传递给其基类.

```C++
class A{
  int a;
public:
  A(int n = 0 ){a = n;}
};

class B :public A{
  int b;
public:
  B(int m = 0, int n = 0 ):A(n){b = m;}
};

class C :public B{
  int c;
public:
  C(int q = 0, int m = 0, int n = 0 ):B(m,n){c = q;}
};
// C类的构造函数只能够调用B类的构造函数,而B类的构造函数只能够调用A类的构造函数.这里C类的构造函数将值m,n传递给B类的构造函数;而B类的构造函数将值n传递给A类的构造函数
```

如果我们的Worker是虚基类,那么上面的那种自动类型传递将会不起作用.如构造函数`SingingWorker(const Worker &wk, int p = 0, int v = Singer::other):Waiter(wk,p),Singer(wk,v){}` ;这样来完成这一构造函数存在的问题是: 自动传递信息的时候将通过两条不同的途径(Waiter和Singer)将wk传递给Worker对象. 为了避免这种冲突,C++在基类是虚拟的时,禁止信息通过中间类自动传递给基类.因此上述的构造函数将会初始化成员p和v, 但是wk参数中的信息将不会传递给子对象Waiter. 不过,编译器必须要在构造函数之前构造基类的对象组件; 此时我们需要显示的调用所需要的基类的构造函数.因此构造函数应该改为: `SingingWaiter(cosnt Worker &wk, int p = 0, int v = Singer::other):Worker(wk),Waiter(wk,p),Singer(wk,v){}` .**这个代码将显式的调用构造函数worker(const Worker &),对于虚基类,必须要这样做,但是对于非虚基类,这样做是非法的.如果类有虚基类,则除非只需使用该虚基类的默认构造函数,否则必须要显式的调用该虚基类的某个构造函数.**



##### 新的方法

多重继承可能导致函数的二义性. 例如,Worker类有方法show(). 而Worker类和Singer类各自重新定义了方法show(). 那么SingingWorker类将继承两个完全不同的show()方法.  对于单继承,如果没有重新定义show(),则会使用最近祖先中的定义. 而在多重继承中,每一个直接祖先都有一个show()方法.这将导致上述的二义性.

此时可以使用作用域解析符来澄清编程者的意图:

```C++
SingingWaiter newhire;
newhire.Singer::show();
```

不过更好的方法是在SingingWaiter中重新定义show方法.并指出使用那个show方法,或者两个都使用.如下:

```C++
void SingingWaiter::show()
{
  Singer::show();
  Waiter::show();
}
```

**但是这仍然会存在一个问题,这两个show方法可能会调用基类的show方法两次,因为他们可能都会调用Worker::show().**



### 类模板

继承和包含并不是总能够满足重用代码的需求. 容器类被设计来存储其他对象或者是数据类型. C++的模板提供参数化(parameterized)类型, 即能够将类型名作为参数传递给接收方来建立类或者是函数.





























































## 第15章 友元、异常和其他

###  友元

类并非只能够拥有友元函数,也可以拥有友元类. 在这种情况下,友元类的所有方法都可以访问原始类的私有成员和保护成员. 另外,也可以做更严格的限制----只将特定的成员函数指定为另外一个类的友元. 

#### 友元类

假定需要编写一个模拟电视机和遥控器的简单程序.需要定义一个Tv和Remote类,他们之间并不适用于is-a关系,也不适用于has-a关系. 但是Remote应该可以改变Tv的状态,因此,应该将其作为Tv类的一个友元.

在Tv类中使用如下语句 `friend class Remote` ,使得Remote类成为Tv类的友元类; 友元类的声明可以位于公有,私有或者保护部分.声明位于哪里都没有任何影响.



### 嵌套类

在C++中, 可以将类的声明放在另外一个类当中. 在另外一个类中声明的类被称为嵌套类(nested class). 他通过提供新的类型类作用域来避免名称混乱. 包含类的成员函数可以创建和使用被嵌套类的对象; 而仅仅当声明位于共有部分的时候,才能在包含类的外面使用嵌套类,而且必须使用作用域解析操作符.





## 第16章 string类和标准模板库





























颜色深度（Color Depth）用来度量图像中有多少颜色信息可用于显示或打印像素，其单位是“位（Bit）”，所以颜色深度有时也称为位深度。常用的颜色深度是1位、8位、24位和32位。1位有两个可能的数值：0或1。较大的颜色深度（每像素信息的位数更多）意味着数字图像具有较多的可用颜色和较精确的颜色表示。

因为一个1位的图像包含2种颜色，所以1位的图像最多可由两种颜色组成。在1位图像中，每个像素的颜色只能是黑或白；一个8位的图像包含256种颜色，或256级灰阶，每个像素可能是256种颜色中的任意一种；一个24位的图像包含1670万（2^24）种颜色；一个32位的图像包含2^32种颜色，但很少这样讲，这是因为32位的图像可能是一个具有Alpha通道的24位图像，也可能是CMYK色彩模式的图像，这两种情况下的图像都包含有4个8位的通道。图像色彩模式和色彩深度是相关联的（一个RGB图像和一个CMYK图像都可以是32位，但不总是这种情况）.