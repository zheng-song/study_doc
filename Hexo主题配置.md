[TOC]

## 修改默认主题

​	hexo初始化之后默认的主题是`landscape` , 然后你可以去[这个地址](https://hexo.io/themes/)里面找到你想要的主题。在github中搜索你要的主题名称，我选的是`hueman`。下面以`hueman`为例介绍如何对其进行配置.

​	hueman提供了一个配置的样例文件'_config.yml.example',我们以其为模板进行配置.

![Menu.png](http://upload-images.jianshu.io/upload_images/6128001-439cb93592c4704d.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)

首先其在样本文件的Categories中只有About条目,我希望增加一些如下:



我们可以使用一个简单的[在线的文字Logo制作工具](http://www.qt86.com/)来制作一个文字Logo,将制作好的Logo保存在`.../hueman/source/css/images/`目录下,然后在配置文件中使用这个Logo

![logo](http://img.blog.csdn.net/20180207113302011?watermark/2/text/aHR0cDovL2Jsb2cuY3Nkbi5uZXQvWlMxMjNaUzEyM1pT/font/5a6L5L2T/fontsize/400/fill/I0JBQkFCMA==/dissolve/70/gravity/SouthEast)

