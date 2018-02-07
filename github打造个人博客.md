[TOC]

## 步骤

1. 在github中新建一个仓库，仓库的名称必须是 `你的用户名.github.io` ,例如我的新仓库名为 `zheng-song.github.io` 。
2. 然后往这个仓库中添加内容。例如HTML文档如下

```html
<!DOCTYPE html>
<html>
<body>
<h1>这是我使用github pages搭建的个人站点。</h1>
</body>
</html>
```

3. 在浏览器中输入 `zheng-song.github.io` 就可以进入你的个人站点了。



## [进阶](https://www.cnblogs.com/visugar/p/6821777.html)

​	如果你的html、css、js技术足够好，你完全可以利用这样的方式搭建一个高逼格且实用的个人站点。但是我并不会这些，我只是想要将其打造成为一个个人博客平台，用以记录自己平常写的一些东西和做的笔记。此时我们可以借用一些辅助工具来完成，我们使用的是[hexo](https://hexo.io/).

### 1. Ubuntu下安装NodeJs

​	Hexo是基于NodeJs环境的静态博客,此处我直接下载的[最新的tar文件](http://nodejs.cn/download/)，根据你的操作系统自行下载，我的是Ubuntu，然后解压之后建立软链接。`sudo ln -s 你的nodejs目录/bin/node /user/local/bin/node `, `sudo ln -s 你的nodejs目录/bin/npm /user/local/bin/npm `。 



### 2. 安装Hexo

​	`npm i -g hexo` 此时会自动进行安装工作，安装好的可执行文件在`你的nodejs目录/bin/`目录下，同样我们给其建立一个连接。`sudo ln -s 你的nodejs目录/bin/hexo /user/local/bin/hexo`. 使用`hexo -v`看是否会打印出一些信息。打印出来则说明安装成功。

​	新建一个工作目录，在你的工作目录下执行`hexo init` .我的目录为`~/app/hexo/` ,会自动完成初始化的过程，并在hexo目录下建立一些文件和目录。

![2018-02-06 17-14-33 创建的截图.png](http://upload-images.jianshu.io/upload_images/6128001-865f927ca82f90ca.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)

- scaffolds: 命令生成文章等的模板。
- source: 用命令创建的各种文章
- themes: 主题
- _config.yml: 整个博客的配置
- package.json: 项目所需要的模块项目的配置信息。



### 3. 使用Hexo

​	打开_config.yml文件，修改其中的一些配置`vim _config_yml` .在文件的最后添加内容如下：

```yml
deploy:
  type: git
  repo: https://github.com/YourgithubName/YourgithubName.github.io.git
  branch: master
```

保存之后执行`hexo clean && hexo generate && hexo server` .

> hexo 3.0将服务器独立成个别模块，需要单独安装：sudo npm i -g hexo-server

hexo server 运行成功以后会打印出如下内容，此时在浏览器中输入:`localhost:4000`即可进入。

![2018-02-06 17-31-46 创建的截图.png](http://upload-images.jianshu.io/upload_images/6128001-bee8fe4a03766a99.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)



### 4. 上传到github

​	首先要安装`sudo npm install hexo-deployer-git --save`,注意这里不使用-g选项，因为我们不要全局安装，而是要安装在当前目录下，安装之后会生成一个db.json文件。此时就可以运行 `hexo clean && hexo generate && hexo deploy` 命令了。deploy需要你输入你的github的用户名和密码。deploy的过程会自动的将hexo的配置等信息上传到你的github上。此时在浏览器中输入`zheng-song.github.io`就可以看到配置好的hexo的页面啦。

​	此时你的博客的首页的信息是`~app/hexo/source/_posts/hello_world.md`的内容。 你要定制信息就要修改中这个目录中的文件，以后你在其他电脑上就直接在github中修改这个文件下的内容就可以了，就不再重新配置一遍了。



### 5. 修改以及配置主题

​	hexo初始化之后的默认主题是landscape，我们可以修改这个主题，到[这个地址](https://hexo.io/themes/)里去找一个你喜欢的主题，然后可以跳转到github中下载下来，按照说明来安装即可，我是用的主题是[hueman]()。安装完成以后再执行 `hexo clean && hexo generate && hexo deploy` 。

关于hueman主题的配置可以到[这里](http://www.jianshu.com/writer#/notebooks/22040471/notes/23715025)去看.

