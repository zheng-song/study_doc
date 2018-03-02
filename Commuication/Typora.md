[TOC]

### 数学表达式

​	Typora支持LaTex数学公式，要启用这个功能，首先到Preference->Editor中启用。然后使用\$符号包裹Tex命令，例如: $lim_{x\to\infty}\exp(-x)=0$

$x<0$, $$y=f(x).$$

详见[LaTex数学公式手册](https://artofproblemsolving.com/wiki/index.php/LaTeX:Symbols)

### 下标

下标使用\~包裹，例如：H\~2\~O将产生水的分子式H~2~O。

### 上标

上标使用 ^ 包裹，例如：y^ 2 ^ = 4将产生表达式:y^2^ = 4



### 插入表情

使用: cry :输入哭的表情:cry:等。



### 下划线

用HTML的语法\<u\>Underline \</u\>将产生下划线 <u>Underline</u>.

### 删除线

GFM添加了删除文本的语法，这是标准的Markdown语法没有的。使用\~\~包裹的文本将会具有删除的样式，例如~~删除文本~~将产生删除文本的样式。



### 标注

我们可以对某一个词语进行标注，例如

I love typora[^注释]此时将鼠标放在注释上，会出现标注的内容。

[^注释]: 我喜欢typora