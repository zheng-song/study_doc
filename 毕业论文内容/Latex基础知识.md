### 1. 简介

​	Latex的工作方式类似于web page，都是由源文件(.tex or .html)经由引擎(Tex or browser)渲染产生最终的效果(得到PDF文件 or 生成HTML页面)。两者极其相似，包括语法规则与工作方式。

![](https://liuchengxu.github.io/blog-cn/assets/images/posts/sketch.png)

一般的规范写法中都是在HTML文件中写入web page的结构内容，再由css控制着页面生成的样式。当然你也可以选择在HTML中直接写入样式内容，不过不提倡这样写。同样在Latex中，你可以再tex源文件中同时写入主题的内容和样式，也可以内容和样式分离，以网络上流传广泛的[清华大学LaTeX模板](https://github.com/xueruini/thuthesis)为例，以.cls(class)结尾的thuthesis.cls便可看作是与css起到同样作用的样式文件。

​	我第一次修改清华大学模板就是直接修改的thuthesis.cls与thuthesis.cfg文件。直接从ins和dtx文件开始做的话要花费很多学习如何编写宏包的成本，我的本科毕业论文时间并不多，只能在cls文件上直接修改，虽然会很ugly。

​	LaTeX中还有宏包的概念，`\usepackage{foo}`即可使用宏包foo中定义的内容。这跟C语言的`include`是一样的，将文件加载进来进行使用。所谓宏包就是一些写好的内容打包出来以便大家使用而已。利用宏包，我们可以使用很多现成的好用的样式。当然了，如果要编写一个自己的个性化的宏包也是可以的，不过需要学习成本而已。

​	初期的话，我们可以选择一个LaTeX模板进行改造。不过第一次见到一些模板的话，可能会对很多文件的作用一头雾水，下面是简单的介绍，详细内容可见[在LaTeX中进行文学编程](http://liam0205.me/2015/01/23/literate-programming-in-latex/)，当然更多介绍的话可以自行搜索。

| LaTeX模板常见文件类型 | 功能简要介绍                                   |
| ------------- | ---------------------------------------- |
| .dtx          | **D**ocumented La**T**e**X** sources，宏包重要部分 |
| .ins          | installation，控制 TeX 从 .dtx 文件里释放宏包文件     |
| .cfg          | config， 配置文件，可由上面两个文件生成                  |
| .sty          | style files，使用`\usepackage{...}`命令进行加载   |
| .cls          | classes files，类文件，使用`\documentclass{...}`命令进行加载 |
| .aux          | auxiliary， 辅助文件，不影响正常使用                  |
| .bst          | BibTeX style file，用来控制参考文献样式             |

class与style似乎在功能上类似，但是也有区别。[这里是关于.cls与.sty文件的区别](https://tug.org/pracjourn/2005-3/asknelly/nelly-sty-&-cls.pdf)

[额外推荐阅读材料:来自北京大学李东风老师的LaTeX排版心得](http://www.math.pku.edu.cn/teachers/lidf/docs/textrick/tricks.pdf)



### 2. 简单的规则

1. 空格: Latex中空格不起作用。
2. 换行：用控制命令“\\”,或“ \newline”.
3. 分段：用控制命令“\par” 或空出一行。
4. 换页：用控制命令“\newpage”或“\clearpage”
5. 特殊控制字符：#，$, %, &, - ,{, }, ^, ~



### 3. 西文字符转换表

\\rm   罗马字体          \\it     意大利字体
\\bf   黑体              \\sl     倾斜体
\\sf   等线体            \\sc     小体大写字母
\\tt    打字机字体       \\mit     数学斜体



### 4. 纵向固定间距控制命令

\\smallskip             \\medskip               \\bigskip



### 5. 页面排版

\textwidth=14.5cm
\textheight=21.5cm
系统默认：字号10pt= 五号字；西文字体为罗马字体；textwidth=12.2cm,textheight=18.6cm。相当于美国标准信纸大小。

#### 5.1 段落换行

​	用一个空行或者是`\par`命令可以开启新的段落，同时会有默认的首行缩进。用`\\`或者是`\newline`可以强制换行在下一行继续，且在下一行不会有缩进。

#### 5.2 列表环境

​	Latex中的列表环境有三种：无序列表(itemize)、有序列表(enumerate)和描述列表(description)。示例代码如下：

```latex
\begin{itemize}  
  \item javascript  
  \item html  
  \item css  
\end{itemize}  
  
\begin{enumerate}  
  \item javascript  
  \item html  
  \item css  
\end{enumerate}  
  
\begin{description}  
  \item[javascript] javascript  
  \item[html] html  
  \item[css] css  
\end{description}  
```

效果如下：

![](https://user-gold-cdn.xitu.io/2018/4/12/162b8754ae0256c7?w=227&h=321&f=png&s=9628)

#### 5.3 章节目录设置

​	在book 和report 文档类中，可以使用`\part`、`\chapter`、`\section` 、`\subsection`、`\subsubsection`、`\paragraph`、`\subparagraph` 这些章节命令，在article 文档类中，除了`\chapter` 不能用，其它的都可以用。

​	用`\tableofcontents`命令可以自动从各章节标题生成目录。在导言区中用下面的命令载入hyperref 宏包`\usepackage{hyperref}`就可以让生成的文章目录有链接，点击时会自动跳转到该章节。而且也会使得生成的pdf 文件带有目录书签。

​	如果要调整章节标题在目录页当中的格式，可以使用titletoc宏包。该宏包的基本命令参数如下：

```latex
\titlecontents{标题层次}[左间距]{整体格式}{标题序号}{标题内容}{指引线和页码}[下间距]
```

#### 5.4 参考文献

引用文献的基本环境是：

```latex
\begin{thebibliography}{}  
\bibitem[显示符号]{引用标签} Book Title, Author  
\end{thebibliography}  
```

其中`\begin{thebibliography}{}`的大括号内填入的数字表示最大标号值。`\bibitem`表示一条文献记录。其中`[显示符号]`表示在参考文献区域显示的标号，可不填，默认使用数字1,2,3进行编号。`引用标签`则是在正文中引用的标签。参考文献的引用和其他的引用有点不同，需要用`\cite{引用标签}`来引用。

​	在LaTeX 中使用参考文献很容易，代码如下：

```latex
\begin{thebibliography}{123456}  
\bibitem {JW1}Jingwhale, T.A.O.C.P. , Yunlong Zhang , 2015,Vol. 1.  
\bibitem {JW2}Jingwhale, T.A.O.C.P. , Yunlong Zhang , 2015,Vol. 6.  
\bibitem {JW2}Jingwhale, T.A.O.C.P. , Yunlong Zhang , 2015,Vol. 8.  
\end{thebibliography}  
```

**TIPS:**

- 默认thebibliography会自动添加标题Reference，所以无需重复添加
- 默认参考文献的行间距为一行，这有时候显得太大了。可以在\begin{thebibliography}{}后添加\addtolength{\itemsep}{-1.5ex}来缩小行间距。-1.5ex表示每行缩小1.5ex。其实细心观察可以发现，thebibliography其实是一个枚举环境，因此对于itemize和enumerate，可以用同样的方法缩小行间距。
- thebibliography是十分繁琐的。因为你还需要把作者等信息一个个地填上去,可以使用bib文件。

### 6. 数学公式

​	数学公式的练习始于markdown，因为很多markdown编辑器是支持LaTeX数学公式的，比如Typora，不仅可以写出漂亮的公式，还能方便做笔记。

​	Latex的数学公式有两种模式：行内模式(inline)和行间模式(display).前者在正文的行文中插入数学公式；后者独立排列单独成行。在行文中，使用`$ ... $`可以插入行内公式，使用`$$ ... $$`可以插入行间公式，如果需要对行间公式进行编号，可以使用equation环境，例如：

```latex
$$
\begin{equation}
......
\end{equation}
$$
```

例如行内公式：$\sum_{i=0}^n i^2 = \frac{(n^2+n)(2n+1)}{6}$

行间公式：

$$
\sum_{i=0}^n i^2 = \frac{(n^2+n)(2n+1)}{6}
$$
关于数学公式更多的内容见[这里](https://artofproblemsolving.com/wiki/index.php/LaTeX:Symbols)



### 7. 插入表格

​	插入表格经常使用的是tabular 环境，这个环境是LaTeX 中预先定义好的。

**1. tabular 环境插入表格：**

- 首先，tabular 环境的参数|l|c|r| 指明了各列的对齐方式，l、c 和r 分别表示左对齐、居中对齐和右对齐。中间的竖线| 指明各列之间有竖线分隔，如果在某些地方不需要竖线，去掉相应位置的| 即可。
- 表格各行的元素之间用& 号分隔，两行内容用\\ 分隔。\hline 表示两行之间的横线；你可以用连续两个\hline 得到双横线，或者去掉\hline 以不显示该横线。
- 如果需要在某个单元格中填写多行内容，不能直接用\\ 或\newline 命令，而应该将它们放在一个盒子里面（比如\parbox 盒子）。

例如：

```latex
\begin{tabular}{|l|c|r|}  
  \hline  
  % after \\: \hline or \cline{col1-col2} \cline{col3-col4} ...  
  左列 & 中列 & 右列 \\  
  \hline  
  2行1列 & 2行2列 & 2行3列 \\  
  \hline  
  3行1列 & 3行2列 & 3行3列 \\  
  \hline  
  4行1列 & 4行2列 & 4行3列 \\  
  \hline  
\end{tabular}  
```

显示效果如下：

![image](http://images.cnitblog.com/blog/707050/201501/261441371125811.png)



**2. 跨列表格**

​	复杂的表格经常需要跨行和跨列，在tabular 环境中，我们可以用命令\multicolumn 得到跨列表格，而跨行表格需要使用multirow 宏包，暂不介绍。

```latex
\begin{tabular}{|l|c|r|}  
  \hline  
  % after \\: \hline or \cline{col1-col2} \cline{col3-col4} ...  
  左列 & 中列 & 右列 \\  
  \hline  
  2行1列 & 2行2列 & 2行3列 \\  
  \hline  
  \multicolumn{2}{|c|}{跨越2015} & 3行3列 \\  
  \hline  
  4行1列 & 4行2列 & 4行3列 \\  
  \hline  
\end{tabular} 
```

显示效果如下：

![image](http://images.cnitblog.com/blog/707050/201501/261441385973299.png)

\multicolumn 命令的第一个参数指明要横跨的列数，第二个参数指明对齐和边框线，第三个参数指明该单元格的内容。

**3. 浮动表格：**

​	前面所说的插入表格的例子中，表格是在tabular 环境对应的位置排版出来的。如果表格高度大于当前页剩余高度，表格就会被放置到下一页中，造成这一页下部留出很大空白。大部分时候我们并不需要严格限定表格出现的位置，而只要求表格在该段正文的附近出现即可。此时，我们可以用table 浮动环境来达到自动调整位置的效果。

```tex
\begin{table}[htbp!]  
  \centering  
  
  \begin{tabular}{|l|c|r|}  
  \hline  
  % after \\: \hline or \cline{col1-col2} \cline{col3-col4} ...  
  左列 & 中列 & 右列 \\  
  \hline  
  2行1列 & 2行2列 & 2行3列 \\  
  \hline  
  3行1列 & 3行2列 & 3行3列 \\  
  \hline  
  4行1列 & 4行2列 & 4行3列 \\  
  \hline  
\end{tabular}  
  
  \caption{示例表格}\label{we}  
\end{table}  
```

其中的可选参数里，h（here，当前位置）、t（top，页面顶部）、b（bottom，页面底部）、p（page，单独一页）表明允许将表格放置在哪些位置，而! 表示不管某些浮动的限制。用table 浮动环境，还可以用\caption命令指明表格的名称，并得到表格的自动编号。



### 8. 插入图片

在LATEX 文档中插入现有的图形，可以使用graphics。我们这里只介绍graphicx 宏包。需要载入graphicx 宏包`\usepackage{graphicx}  `

#### 8.1 图文混排

命令：`\includegraphics[选项]{图形文件}` 。如下的代码：

```tex
\documentclass[UTF8]{ctexart}  
\usepackage{graphicx}  
\begin{document}  
和Microsoft Office Word 等所见即所得的办公软件不同，用  
\raisebox{-2mm}{\includegraphics[scale=0.8]{picture/tupian.jpg}}  
排版文档，首先要用文本编辑器编辑好tex文档，然后通过各种程序编译，得到pdf 文档用于打印或者阅读。  
\end{document}  
```

显示效果如下：

![img](http://images.cnitblog.com/blog/707050/201501/261441415662572.png)

- `\includegraphics` 命令有许多选项，上面例子中使用的选项`scale=0.8` 指明了整体的伸缩因子，常用的选项还有宽度值和高度值选项，例如`width=64mm` 和`height=48mm` 等等。如果宽度值和高度值只指明一项，将按同比例对另一项作伸缩。
- 默认情况下，图片是和正文的基线对齐的，当图片高度比行距大时，结果不是很美观。可以用`\raisebox`命令稍微降低图片的位置。

#### 8.2 图文分开

​	如果插入的图片需要独立居中显示。这可以通过把插入的图片放在center 环境中来实现。如下面的代码：

```latex
\documentclass[UTF8]{ctexart}  
\usepackage{graphicx}  
\begin{document}  
和Microsoft Office Word 等所见即所得的办公软件不同，用  
\begin{center}  
\includegraphics{picture/tupian.jpg}  
\end{center}  
排版文档，首先要用文本编辑器编辑好tex文档，然后通过各种程序编译，得到pdf 文档用于打印或者阅读。  
\end{document} 
```

显示效果如下：

![image](http://images.cnitblog.com/blog/707050/201501/261441433785860.png)

​	图片最好设置宽和高，让它按照我们的约定显示。图片的伸缩因子需要计算有些麻烦，如果将它设为页芯宽度`\textwidth` 的某个因子，如`\includegraphics[width=0.9\textwidth]{picture/tupian.jpg}`，图片将页芯宽度显示。

#### 8.3 浮动图片

​	同表格的插入情形类似，自动调整图片的位置。用figure 浮动环境来达到这个效果。

示例如下：

```latex
\documentclass[UTF8]{ctexart}  
\usepackage{graphicx}  
\begin{document}  
和Microsoft Office Word 等所见即所得的办公软件不同，用  
\begin{figure}[h]  
  \centering  
  \includegraphics{picture/tupian.jpg}\\  
  \caption{latex}  
\end{figure}  
排版文档，首先要用文本编辑器编辑好tex文档，然后通过各种程序编译，得到pdf文档用于打印或者阅读。  
\end{document} 
```

显示效果如下：

![image](http://images.cnitblog.com/blog/707050/201501/261441462536105.png)







### latex处理参考文献



latex通常使用bibtex数据库来处理参考文献，在引用bibtex中的参考文献时，编译过程中会自动的对其进行编号。但是对于没有被引用的参考文献，不会在文件中显示出来，此时我们需要在

```latex
\nocite{*} %加上这一个语句就可以显示未被引用的参考文献
\bibliography{ref/myref.bib}
```





### 几个Latex网站推荐

- [Detexify LaTeX handwritten symbol recognition](http://detexify.kirelabs.org/classify.html).：通过手写识别LaTeX符号，识别率很高，尤其是当看到一个符号却不知道其LaTeX命令的时候它很有用。只要画出记忆中符号的样子，就会自动出现各种可能想要的表示方法。
- [LaTeX公式编辑器](http://zh.numberempire.com/texequationeditor/equationeditor.php)： 对于尚不熟悉的人书写LaTeX公式提供一点便利。
- [在线LaTeX编辑器shareLaTeX](https://cn.sharelatex.com/)：好处就是不用本地搭建环境，有中文界面，直接在线操作。还有很多LaTeX模板可供选择。