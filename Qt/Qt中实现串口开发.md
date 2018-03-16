## QT开发串口

当前的QtSerialPort模块中提供了两个C++类，分别是QSerialPort 和QSerialPortInfo。

QSerialPort 类提供了操作串口的各种接口。

QSerialPortInfo 是一个辅助类，可以提供计算机中可用串口的各种信息。





首先需要在QT的工程文件pro当中加入`QT += serialport`

然后在头文件当中包含串口`#include <QSerialPort>`



1. 配置打开串口

```C++
QSerialPort* myserial = new QSerialPort()：
this->myserial->setPortName("COM1");//待打开的串口号
this->myserial->setBaudRate(9600,QSerialPort::AllDirections);//设置波特率;双工
this->myserial->setDataBits(QSerialPort::Data8);//数据位
this->myserial->setParity(QSerialPort::NoParity);//校验位
this->myserial->setStopBits(QSerialPort::OneStop);//停止位
this->myserial->setFlowControl(QSerialPort::NoFlowControl);//流控制
this->myserial->open(QIODevice::ReadWrite);//打开串口
```

2. 发送数据

```C++
char test[5] = {0x01,0x02,0x03,0x04,0x05};
myserial.write(test);//发送
```

3. 接收数据：定义一个函数作为槽函数，当接收缓冲区有数据的时候回调此函数。此外，在打开串口函数里面关联信号和槽。已知，当接收缓冲区有数据的时候，串口对象会发送readyRead信号。`QObject::connect(myserial,&QSerialPort::readyRead,this,&MainWindow::ReadBuf);`

```C++
void ReadBuf()
{
　　QByteArray dataArray;
　　dataArray = myserial->readAll();//读取数据
　　if(!dataArray.isEmpty())
  	{
       QString str;
　　　　str = dataArray.toHex();//把数据直接转化为16进制的字符串形式
　　　　ui->textEdit->setText(str);
　　}
}
```

4. 关闭串口

```C++
this->mySerial->close();
```



若需要自动获取可用的串口号，并将其设置在QComboBox当中，那么需要以下步骤：

1. 包含头文件

```C++
#include <QSettings>
#include <qt_windows.h>
```

2. 自定义函数

```C++
QString getcomm(int index, QString keyorvalue)
{
　　HKEY hKey;
    wchar_t keyname[256]; //键名数组
    char keyvalue[256];  //键值数组
    DWORD keysize,type,valuesize;
    int indexnum;
    QString commresult;
    if (::RegOpenKeyEx(HKEY_LOCAL_MACHINE,\
                       TEXT("HARDWARE\\DEVICEMAP\\SERIALCOMM"),\
                       0, KEY_READ,&hKey) != 0)
    {
        QString error="Cannot open regedit!";
        return error;
    }
    QString keymessage;//键名
    QString message;
    QString valuemessage;//键值
    indexnum = index;//要读取键值的索引号
    keysize = sizeof(keyname);
    valuesize = sizeof(keyvalue);
    if (RegEnumValue(hKey, indexnum,\
                     keyname, &keysize, 0, \
                     &type, (BYTE*)keyvalue, &valuesize) == 0)
    {
        for (int i=0; i<(int)keysize; i++)
        {
            message = keyname[i];
            keymessage.append(message);
        }
        for (int j=0; j<(int)valuesize; j++)
        {
            if (keyvalue[j] != 0x00)
                valuemessage.append(keyvalue[j]);
        }
        if (keyorvalue == "key")
            commresult = keymessage;
        if (keyorvalue == "value")
            commresult=valuemessage;
    }
    else
        commresult = "nokey";
    RegCloseKey(hKey);//关闭注册表
    return commresult;
}
```

3. 将可以使用的串口放在QComBox当中

```c++
void MainWindow::InitInterface()
{
    QString path = "HKEY_LOCAL_MACHINE\\HARDWARE\\DEVICEMAP\\SERIALCOMM";
    QSettings *settings = new QSettings(path, QSettings::NativeFormat);
    QStringList key = settings->allKeys();
    QStringList comlist;
    comlist.clear();
    /* 取串口名 */
    for (int i=0; i<key.size(); i++)
        comlist << getcomm(i, "value");
    ui->comboBox->addItems(comlist);
}
```



==如何解决粘包问题：(以传输ascii为例)==

1. 接收数据，如果数据的长度刚好为协议那么长，那么就直接解析。
2. 如果长度不正确就把此包追加到本地的变量当中，然后根据报文头的位置把本地变量去掉报文头前面的数据(QString :: mid)
3. 判断长度是否足够(并且本地变量的头是协议头)，如果足够则取本地变量前n个数据进行解析，并将本地变量去掉这个包；如果不是协议头则清空此变量，再次进行循环。

流程：

![img](https://images2017.cnblogs.com/blog/990943/201712/990943-20171227155415191-1599431568.png)

