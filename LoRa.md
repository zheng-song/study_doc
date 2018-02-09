[TOC]

## [LoRa Alliance : Wide Area Networks for IoT](https://docs.wixstatic.com/ugd/eccc1a_ed71ea1cd969417493c74e4a13c55685.pdf)

​	LoRa是一个用来建立长距离通信链路的物理层或无线模块.很多传统的无线网络使用频移键控([frequency shifting keying](https://baike.baidu.com/item/%E9%A2%91%E7%A7%BB%E9%94%AE%E6%8E%A7))作为物理层的模块,因为它是非常高效实现低功耗的模块。LoRa基于[chirp spread spectrum](https://baike.baidu.com/item/%E7%BA%BF%E6%80%A7%E8%B0%83%E9%A2%91/5306386?fr=aladdin)模块,其在保持低功耗的特性的同时可以大幅提升通信距离。chirp spread spectrum已被用于军事和太空通信数十年之久，但是LoRa是其第一次低功耗的商业尝试。

### Long Range

​	LoRa的优势是长距离传输，一个单独的网关或基站能够覆盖整个的城市(数百平方公里，hundreds of square kilometers)，距离要根据当地的环境和障碍(obstructions)来决定，但是LoRa和LoRaWAN相比于其他的通信标准有更高的链路预算(link budget)。link budget是在给定环境中决定距离的一个主要因素，通常以分贝(decibels,dB)的形式来给出。

### Where does LPWAN fit

​	一项技术无法适用于所有的IoT项目，WiFi和BTLE是个人设备之间通信的广泛适用标准，Cellular技术更适用于高数据吞吐量和电源供电的应用。LPWAN提供数年的电池寿命，专为每小时需要传输几次少量数据的长距离传感器和应用而打造。

### What is LoRaWAN

​	当LoRa物理层允许长距离通信链路时，LoRaWAN定义了LoRa网络的通信协议和系统结构。这个协议和网络的体系结构对于节点的电池寿命、网络能力、服务质量、安全、网络应用都有很大的影响。

![LoRa](http://img.blog.csdn.net/20180205133009413?watermark/2/text/aHR0cDovL2Jsb2cuY3Nkbi5uZXQvWlMxMjNaUzEyM1pT/font/5a6L5L2T/fontsize/400/fill/I0JBQkFCMA==/dissolve/70/gravity/SouthEast)

### Network Architecture

​	许多目前的网络布置(deploy)使用的是网状网络结构(mesh network architecture)。在网状结构当中，独立的终端节点分发其他节点的信息，以此来增加通信距离和网络规模。由于节点可能从其他无关的节点接收和转发信息，这在增加距离的同时也会增加复杂度、降低网络性能、减少电池寿命。星型结构(star architecture)对于长距离连接实现电池寿命的保持具有重要的意义。

![LoRa2](http://img.blog.csdn.net/20180205134913382?watermark/2/text/aHR0cDovL2Jsb2cuY3Nkbi5uZXQvWlMxMjNaUzEyM1pT/font/5a6L5L2T/fontsize/400/fill/I0JBQkFCMA==/dissolve/70/gravity/SouthEast)

在LoRaWAN网络中，节点不会和特定的网关相关联。相反，一个节点传输的数据会被多个网关接收。每个网关都会通过一些backhaul(either cellular,Etherent,satellite,or WiFi)转发从节点收到的数据包到基于云的网络服务器中(cloud-based network server), 分析和复杂性(The intelligence and complexity)工作被放到网络服务器中。如果一个节点是移动的，那么就没有从网关到网关切换的必要，这对于设备追踪(asset tracking)应用而言是一个至关重要的特性。

### Battery lifetime

​	当LoRaWAN网络中的节点间有数据要传输时，他们是异步通信的而不是事件驱动或预先计划(event-driven or scheduled)好的。这种类型的协议是典型的参考了[Aloha](https://baike.baidu.com/item/Aloha/3695024?fr=aladdin)原理。在一个网状网络或一个异步网络，例如蜂窝网络，节点需要频繁的唤醒以达到网络同步和检查信息的目的。这种同步消耗了大量的能量，也是电池寿命下降的首要因素。最近的研究表明，LoRaWAN相对于其他的可选技术显示出了3-5倍的优势。

### Network capacity

​	为了使得长距离星型网络可行(viable)，网关必须要有非常高的容量和能力来接收大量节点的信息。在LoRaWAN中实现高网络容量是通过使用(utilizing)[自适应数据率](https://baike.baidu.com/item/%E9%80%9F%E7%8E%87%E8%87%AA%E9%80%82%E5%BA%94/16913450)(adaptive data rate) and by using multichannel multi-modem transceiver in the gateway so that simultaneous(同时的) messages on multiple channels can be received。影响容量的关键因素是并发的信道数量(the number of concurrent channels)、数据率(time on air)、负载长度(payload length)、节点传输的频率。由于LoRa是基于[chirp spread spectrum](https://baike.baidu.com/item/%E7%BA%BF%E6%80%A7%E8%B0%83%E9%A2%91/5306386?fr=aladdin)的模块，当使用(utilizing)不同的扩频因子(spreading factors)时，信号之间几乎是正交的(practically orthogonal)。随着扩频因子的改变，有效数据率也会改变。网关利用这一特性来同时在通一个信道上接收多重的变化的数据率(receive multiple different data rates on the same channel at the same time)。

### Device Classes - Not All Nodes Are Created Equal

​	终端设备用于不同的应用和要求，为了优化不同的终端应用配置(variety of end application profiles),LoRaWAN 使用了不同的设备类(device classes)。设备类交换(trade-off)网络的下行通信延迟和电池寿命？？？(the device classes trade-off network downlink communication latency versus battery lifetime)。在一个control or actuator-type的应用中，下行通信延迟是一个重要的因素。

- 双向终端(bi-directional)是类型A(Class A): A类允许双向通信，通过(whereby)在每个设备的上行传输后跟随两个短的下行接收窗口。传输间隔(transmission slot)由设备根据自己的通信需要来安排，使用的是一个基于随机时间基础的小变种(ALOHA类型的协议)。这个A类操作对于应用而言是最低能量终端系统，只需要来自服务器的下行通信跟随在终端设备的上行通信之后。来自服务器的下行通信在任何的其他时间都必须要等待直到下一个预定好的上行通信到来。
- 有计划好的接受时间的双向终端(Class B): 除了A类的随机接收窗口之外，B类设备在一个计划好的(scheduled)时间上打开一个额外的接收窗口。为了实现在设备上这个计划好的接收窗口，他会从网关接收一个时间同步信标(time-synchronized beacon)。这让服务器可以知道什么时候终端设备处于监听状态。
- 带有最大接收间隙的双向终端设备(Class C): C类终端设备几乎有持续打开的接收窗口，只会在传输数据的时候关闭接收窗口。

### Security

​	LoRaWAN使用两个层面的安全：一个面向网络，一个面向应用。网络层的安全确保网络中的节点的真实性(authenticity)，应用层的安全确保网络运营商(network operator)无法获得终端用户的应用数据。使用IEEE EUI64标志符密钥交换技术(key exchange utilizing an IEEE EUI64 identifier)的AES加密技术被用在LoRaWAN当中。

​	对于每一个技术的选择都有一个平衡(trade-off),但网络架构、设备类别、安全性、容量可扩展性和移动优化方面的LoRaWAN™功能可满足各种潜在物联网应用的需求。





## [LoRaWAN geolocation whitepaper](https://docs.wixstatic.com/ugd/eccc1a_d43b3b29dfff4ec2b00f349ced4225c4.pdf)

![LoRa3](http://img.blog.csdn.net/20180206091607735?watermark/2/text/aHR0cDovL2Jsb2cuY3Nkbi5uZXQvWlMxMjNaUzEyM1pT/font/5a6L5L2T/fontsize/400/fill/I0JBQkFCMA==/dissolve/70/gravity/SouthEast)

​	LoRaWAN有两种定位方案可选择，粗略的(coarse)定位可使用RSSI(Received Signal Strength Indication)，为了更好地精度可以选择TDOA(Time Difference Of Arrival:信号到达时间差)。TDOA更适用于低功耗，电池供电，精度要求更高的终端设备，也是本文档的介绍重点。

> ***室内定位精度的几个影响因素:*** [非视距传播](https://baike.baidu.com/item/%E9%9D%9E%E8%A7%86%E8%B7%9D/7186349) 、[多径传播](https://baike.baidu.com/item/%E5%A4%9A%E5%BE%84%E6%95%88%E5%BA%94) 、[阴影效应](https://baike.baidu.com/item/%E9%98%B4%E5%BD%B1%E6%95%88%E5%BA%94) 。
>
> ​	通常非视距传播主要影响基于TOA(信号到达时间)定位方法或基于[TDOA](https://zh.wikipedia.org/wiki/%E5%88%B0%E8%BE%BE%E6%97%B6%E9%97%B4%E5%B7%AE)(信号到达时间差)定位方法的精度，多径和阴影主要影响AOA(到达角测量)定位方法和基于RSSI参数的定位方法的估计精度，同时也会影响基于时间的定位算法。

​	Rual deployments with clear line of sight and recommended gateway-deployment geometry will achieve accuracies near the lower end of the scale.[Multipath issues]() inherent in urban and dense urban environments will provide accuracies toward the higher end of the scale.(具有清晰视线的农村部署和建议的网关部署几何将达到接近较低端的精度。城市和密集的城市环境中固有的多路径问题将提供更高的精度。)

​	对于不需要频繁定位的(几天或几星期)终端设备能够实现最佳功效。这些终端设备通常是A类的静止或不常用的移动资产。the geolocation capability for these end-devices comes at no additional bill of materials(BOM) cost(这些终端设备的地理定位功能不需要额外的物料清单成本).

​	本文档将聚焦于室外的、广域的地理定位。

### The LoRaWAN geolocation capability

LoRaWAN终端设备的上行信号需要三个以上的网关设备收到才能够进行定位。这个信号不必是特定的定位信息，可以是普通的应用数据帧。几个网关同时收到相同的上行信息，终端设备的位置由[多点定位技术](https://en.wikipedia.org/wiki/Multilateration)来决定。

> **查水表的定位术** :有关部门可以通过网络运营商提供的数据定位嫌疑人。网络运营商会提供某可疑IP上网时间以及该IP分配电话、宽带注册地址等等信息供相关部门锁定嫌疑人真实信息。如果嫌疑人有预留电话号码，无论该电话号码是否是实名注册，通过移动手机设备自身的一个串码即可以查询嫌疑人的手机用过几张SIM卡以及每个卡的号码是多少，当确定某个电话号码开通，便会利用无线电天线来侦查嫌疑人位置，调查人员还可以监听手机短信。当我们的手机接通后基站会给我们建立一个通讯频率，该频率运营商是确切掌握的，手机会通过本振频率来放大无线电波，放大无线电波的目的是为了准确的搭建起来语音通话频。运营商的无线电波属于短波F/0，穿透力比较弱，周期比较短。当确定嫌疑人手机号码后运营商会提供一个网络波段的密码。调查人员通过设备连接到该频率即可收听到语音通话内容。
>
> 语音监听的步骤大致为，A手机号码给B手机号码打电话的时候，手机发送频率会上升，当手机频率上升后会发送给基站一个信号，这个信号里面包括手机串号和手机卡串号进行一个身份认证和扣费计时，我们每次打电话的时候都要等待1-5秒的时间才会有嘟嘟的声音的过程就是信号的发送过程。当B手机接通以后手机频率会降低，降低就是为了减少无线电波的干扰和噪音，也就是跳频技术。当降到1200-900MHZ在这个范围里本振频率开始工作，每个手机都有这个功能，这个本振频率有一个振动器是感觉不到的，它的作用就是将手机信号放大，当调查人员进入被监听者手机频率的时候，被监听者会听到电话里有滋滋的电波声音，或者亚音破音。这说明目标已被锁定，可以通过语音取证了。运营商提供给公众的语音服务就是点对点的一个加密模式，运营商提供给有关部门通话频率密码的条件下，被监听者的通话则易如反掌。如果提供的是基站的通信密码则可以对该基站范围的语音进行监听。

![LoRa4](http://img.blog.csdn.net/20180206113914486?watermark/2/text/aHR0cDovL2Jsb2cuY3Nkbi5uZXQvWlMxMjNaUzEyM1pT/font/5a6L5L2T/fontsize/400/fill/I0JBQkFCMA==/dissolve/70/gravity/SouthEast)

在网关设备上需要精确的时间同步。每一个上行的帧(frame)都被网关打上了一个精确的时间戳，这个时间戳被作为帧的元数据的一部分转发到网络服务器，帧中同时也会包含信号强度(signal level)、信噪比(signal-to-noise ratio)、频率误差(frequency error)。网络服务器将同一个帧的信息分类，将包括时间戳在内的归属于这个帧的所有的元数据整合起来，	向geolocation server请求位置计算服务。

### Impact of Propagation(传播) Errors

​	在多径环境当中，LoRaWAN定位的性能被网关的始终精度所限制。Conductive geolocation testing(导航地理定位测试) 在信号电平高于灵敏度25dB通常能够获得3m的精度。