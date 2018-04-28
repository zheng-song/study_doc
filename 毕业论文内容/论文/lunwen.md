这个文件用于记录使用latex编译论文时进行的一些工作。

==在latex中使用图片文件时，无法加载png文件，目前尝试的只能够加载pdf格式的图片文件,且图片的命名不能为中文，否则无法加载。需要将其他格式的图片文件转换为pdf格式==

若你的文件处于一个data目录下，而在外面的main.tex中使用include来包含这个文件时，该文件的中的图片的根目录仍然是main.tex所在的目录。

#### 编译bib文件的方法

​	在当前工作目录下新建一个myref.bib文件。内容如下：

```
@article{MartinDSP00,
     author = "A. Martin and M. Przybocki",
     title = "The {NIST} 1999 speaker recognition evaluation -- an overview",
     journal = "Digital Signal Processing",
     volume = "10",
     pages = "1--18",
     year = "2000",}
% 其中{NIST}中的大括号不会被显示，它的作用是保证将来生成的参考文献中NIST四个字保持原样，不会被小写。
```

在正文中需要加入

\bibliographystyle{ieeetr}

\bibliography{myref.bib}

之后就可以使用cite{MartinDSP00}来引用文献库当中的论文了。

​	编译一次正文之后，会生成一个aux文件，之后需要用bibtex来编译一次，以便在当前目录下生成bbl文件，再编译论文两次，就会生成完整的dvi。这样latex就保证了所有用cite引用到的文献都已经被列出，并且自动编号。同时，如果没有用ocite命令，latex还会保证所有列出的参考文献都是在正文中使用cite{}进行了引用的。

#### 一些语法

```
\TurnOffTabFontSetting 
\TurnOnTabFontSetting 
```

因为模板中设定了表格的行距和字号，使得使用中无法临时自定义表格的行距 和字号。故提供两个命令用于关闭和开启默认表格的行距和字号设置。比如你 如果需要输出一个自己定义字号的表格，可以使用如下示例： 

```
\begingroup 
\TurnOffTabFontSetting 
\footnotesize % 设置字号 
\begin{tabular}{...} 
	<content> 
\end{tabular} 
\TurnOnTabFontSetting 
\endgroup

```



2. \email{Email Address} 

用于生成邮箱地址。如\email{name@example.com}会生成如下效果的地址： name@example.com。









#### 2018/04/08

1. 添加lunwenSource文件夹，这个文件夹下放置的资源主要是图片，bib文件等资源。

