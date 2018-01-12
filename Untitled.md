[TOC]
## 编译地址 
32位的处理器,它的每一条指令都是4个字节,以4个字节存储顺序,进行顺序执行,CPU是顺序执行的,只要没有发生什么跳转,它会顺序进行执行,编译器会对每一条指令分配一个编译地址, 这是编译器分配的, 在编译过程中分配的地址, 我们称之为编译地址.

## 运行地址
​	是指程序指令真正运行的地址，是由用户指定的，用户将运行地址烧录到哪里，哪里就是运行的地址。比如有一个指令的编译地址是0x5，实际运行的地址是0x200，如果用户将指令烧到0x200上，那么这条指令的运行地址就是0x200.

​	当编译地址和运行地址不同的时候会出现什么结果？结果是不能跳转，编译后会产生跳转地址，如果实际地址和编译后产生的地址不相等，那么就不能跳转。

​	C语言编译地址：都希望把编译地址和实际运行地址放在一起的，但是汇编代码因为不需要做C语言到汇编的转换，可以人为的去写地址，所以直接写的就是他的运行地址这就是为什么任何bootloader刚开始会有一段汇编代码，因为起始代码编译地址和实际地址不相等，这段代码和汇编无关，跳转用的运行地址. 



## 编译地址和运行地址如何计算

1.    假如有两个编译地址a=0x10，b=0x7，b的运行地址是0x300，那么a的运行地址就是b的运行地址加上两者编译地址的差值，a-b=0x10-0x7=0x3，

​       a的运行地址就是0x300+0x3=0x303。

2. 假设uboot上两条指令的编译地址为a=0x33000007和b=0x33000001，这两条指令都落在bank6上，现在要计算出他们对应的运行地址，要找出运行地址的始地址，这个是由用户烧录进去的，假设运行地址的首地址是0x0，则a的运行地址为0x7，b为0x1，就是这样算出来的。



##  相对地址

​	以NOR Flash为例，NOR Falsh是映射到[bank0](https://zhidao.baidu.com/question/589612485.html)上面，SDRAM是映射到bank6上面，uboot和内核最终是在SDRAM上面运行，最开始我们是从Nor Flash的零地址开始往后烧录，uboot中至少有一段代码编译地址和运行地址是不一样的，编译uboot或内核时，都会将编译地址放入到SDRAM中，他们最终都会在SDRAM中执行，刚开始uboot在NOR Flash中运行，运行地址是一个低端地址，是bank0中的一个地址，但编译地址是bank6中的地址，这样就会导致绝对跳转指令执行的失败，**所以就引出了相对地址的概念**。

​	至少在bank0中uboot这段代码要知道不能用b+编译地址这样的方法去跳转指令，因为这段代码的编译地址和运行地址不一样，**那如何去做呢？**

​	要去计算这个指令运行的真实地址，计算出来后再做跳转，应该是b+运行地址，不能出现b+编译地址，而是b+运行地址，而运行地址是算出来的。

   _TEXT_BASE:
  .word TEXT_BASE //0x33F80000,在board/config.mk中这段话表示，用户告诉编译器编译地址的起始地址


