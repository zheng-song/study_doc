[TOC]

###在C语言中，如何在main()函数之前执行函数

在gcc中可使用attribute关键字，声明constructor和destructor，如下所示：

```C
#include <stdio.h>
__attribute((constructor))void before_main()
{
  printf("%s\n",__FUNCTION__);
}
__attribute((destructor))void after_main()
{
  printf("%s\n",__FUNCTION__);
}

int main()
{
  printf("%s\n",__FUNCTION__);
  return 0;
}
```

这样可以在main前后调用多个函数，在gcc下使用attribute声明多个constructor、destructor。	



另外：物理的方法，可以用linker指定函数的入口，如下：

```C
int main(int argc, char **argv)
{
  //...
  return 0 ;
}

int before_main(int argc,char **argv)
{
  //...
  return main(argc,argv);
}

// gcc test.c -e _before_main -o test
```

逻辑(间接)的方法，可以用main调用main实现在main前执行一段代码，如下：

```C
#include<stdio.h>
#include<stdbool.h>

int main(int argc, char **argv) 
{
  static _Bool firstTime = true;
  if(firstTime) {
    firstTime = false;
    printf("BEFORE MAIN\n");
    return main(argc, argv);
  }
  
  printf("main\n");
  return 0;
}
// gcc test.c -o test
```

