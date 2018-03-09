[TOC]

# HomeKit && HomeBridge  2018/01/16

1. 安装NodeJs,使用二进制包安装，不使用`apt`安装，使用`apt`时安装的不是最新的版本。

2. 将下载源码放在`~/App`目录下并解压，之后给解压后的文件创建符号链接`sudo ln -s ~/app/node-v8.9.4-linux-x86/bin/node /usr/local/bin/node` && `sudo ln -s /home/zs/app/node-v8.9.4-linux-x86/bin/npm /usr/local/bin/npm` && `sudo ln -s /home/zs/app/node-v8.9.4-linux-x86/bin/npx /usr/local/bin/npx`

3. 下载homebridge： `sudo npm -g install homebridge`下载后的文件仍然在`~/app/node-v8.9.4-linux-x86/bin/`目录下，同样给他创建一个符号链接`sudo ln -s /home/zs/app/node-v8.9.4-linux-x86/bin/homebridge /usr/local/bin/homebridge` 此时就可以使用`homebridge`命令来运行他。


