

1. 根据文件系统的Boot Sector头部中的内容来定位MFT的位置.由MFT所在的簇号(Logical  Cluster of $MFT)* 一个簇占的分区数 * 一个分区占据的字节数 来确定MFT的偏移位置。对应数据结构中的内容既：`mftStartLcn * SectorPerCluster * BytePerSector`,假设mftStartLcn中的内容为 00 00 0C 00 00 00 00(簇号为0xC0000,小端模式)  ，SectorPerCluster 中的内容为00 02(0x200)，BytePerSector的内容为08(0x08)，那么MFT相对于文件系统的偏移位置为 `0xC0000 * 0x200 * 0x80 = 0xC000 0000`
2. 定位到该文件系统下`0xC000 0000`的偏移位置，既为MFT的起始位置。MFT中最开始的16个条目被用来保存特殊文件(metedata：元数据)，每一个条目的大小为0x400字节，`$Root`特殊文件(根路径)在MFT中的第六个位置(5号条目)，根路径相对MFT的偏移地址为`0x400 * 5 = 0x1400`,相对该文件系统的偏移为`0xC000 1400`.
3. 定位到MFT中根路径的位置，即`0xC000 1400` 处，根目录中的文件及文件夹一般都比较多，通常其属性会是非常驻属性，关于这些文件夹的信息记录在了其它的位置。而记录在什么位置是由索引分配属性来记录的，也就是A0属性，接下来着重看一下A0属性，



**文件分配示例：**

​	假设我们创建`\dir1\file1.dat`这个文件，并且假设`dir1`这个目录是已经存在于根目录的。文件的大小是4000字节，每一个簇的大小是2048字节。

- 读取文件系统第一个扇区，处理引导扇区来确定簇的大小，MFT的起始地址以及**每一个MFT entry的大小(每一个entry的大小不是固定为0x400的吗?)**。
- 读取MFT的第一个entry，即\$MFT, 用\$DATA属性来确定MFT剩余部分的布局。
- 首先为新文件分配一个MFT entry。 为了查找一个未分配的entry，我们处理\$MFT文件的\$BITMAP属性。第一个可用的entry，假设entry 304被分配给新文件并且相应的位置被置为1(\$BITMAP中的位)。
- 定位到MFT中的MFT entry 304的位置(相对MFT的偏移为0x400 * 304)，通过清理它的内容来初始化它。\$STANDARD_INFORMATION和\$FILE_NAME属性被创建出来，时间被设置为当前的时间。MFT entry头部中的在用标志被设置
- 为文件分配两个簇，这将通过 MFT entry 6的 `$Bitmap` 文件的 \$DATA 属性来完成。这个文件需要两个簇，所以最佳适配算法找到两个连续的簇692和693。这些簇相应的位被置为1。（译注：\$BITMAP文件\$DATA属性中的位）文件的内容被写入簇中，并且将簇的地址更新到\$DATA 属性中。MFT entry 被更改了，所以文件修改时间被更新。
- 下一步给这个文件增加一个文件名入口。MFT entry 5，也就是根目录，被用于定位dir1.我们读取\$INDEX_ROOT和\$INDEX_ALLOCATION属性，遍历排序树。可以发现dir1的索引入口，并且它的MFT entry地址是200。目录的最后更新时间被更新。
- 定位到MFT entry 200并且处理它的\$INDEX_ROOT属性来确定file1.dat应该放在哪里。给这个文件创建一个新的索引入口，并且重新排序树。这可能会发生在节点的索引入口中。新的索引入口在它的文件引用地址含有MFT entry 304，并且设置相应的时间和标志。目录的最后写入，修改时间被更新。



**文件删除示例：**

假设\dir1\file1.dat文件被删除，下面看一下删除的过程。

- 读取文件系统第一个扇区，处理引导扇区来确定簇的大小，MFT的起始地址以及每一个MFT entry的大小。
- 读取第一个MFT entry即\$MFT文件，用\$DATA属性来确定MFT剩余部分的布局。
- 处理MFT entry 5根目录，遍历\$INDEX_ROOT和\$INDEX_ALLOCATION属性中的索引，找到dir1的入口，它的MFT entry地址是200，目录的最后访问时间被更新。
- 处理MFT 200 的\$INDEX_ROOT属性，查找file1.dat的入口。我们可以找到文件的MFT entry 304。
- 删除索引中的入口地址，导致节点中的入口都将被移动并覆盖掉原来的入口。目录的最后写入，修改，访问时间被更新。
- 将MFT entry 304的在用标志清楚来释放它。我们还将\$Bitmap文件的\$DATA属性中的这个MFT entry的标记清除。
- 处理MFT entry 304的非驻留属性，在\$Bitmap文件中相应的簇被标记为未分配的状态。在这个例子中，我们释放了692和693簇。

**注意，当一个文件在NTFS中被删除后，Windows并不清楚任何的指针。也就是说，MFT entry和簇之间的联系依然存在，文件和MFT entry之间的联系可能由于重排序并没有覆盖入口项而依然存在。**



**文件恢复：**

​	当一个文件被删除以后，名字被从父目录索引中删除，MFT entry被释放，所使用的簇也被释放。但是并没有清除任何的指针。

​	NTFS最大的不利之处是，当一个文件名被从父目录的索引中删除以后，索引被重新排序，名字信息可能会丢失。也就是说，你可能在文件所在的原目录中看不到它的名字了。不过，这一缺点由于所有的MFT entry都在一个表里，所有的未被分配的entry都可以很容易的被找到而抵消。进一步说，每一个entry都有一个\$FILE_NAME属性含有其父目录的引用地址。也就是说，当发现一个未分配的entry，我们通常可以获得其完整的路劲，除非其父目录中有一些被重新分配到了新文件或者目录。

​	恢复已经删除了的NTFS文件的另外一个考虑因素是寻找额外的\$DATA属性。为了恢复NTFS中所有的已删除文件，应该要检查MFT中的未分配的entry。当发现这种entry时，他们的名字可以通过\$FILE_NAME属性和父目录引用来获得。簇指针应该还存在，这样数据在没有被覆盖的情况下还可以恢复。即使文件非常的碎片化，被恢复也是很有可行的。如果属性值是常驻的，数据不会被覆盖，除非MFT entry被重新分配。如果文件需要多于一个MFT entry来保存它的属性，其他的MFT entry也需要进行恢复，Windows使用第一个可用的分配策略来分配MFT entry，所以低序号的MFT entry比高序号的更经常被分配。

​	当恢复文件或者是检查已删除内容的时候，文件系统日志或者是变更日志可能对于新近的删除比较有用。变更日志不是始终打开的，但是它显示了文件是何时被创建的以及最后编辑的时间。





# 使用winhex在NTFS中手工定位一个文件的位置

## 1. MBR

​	假设文件的存储位置为`F:\git\myDocument\lara.pptx`,接下来我们一层层的定位这个文件，首先使用winhex打开F盘。

![MBR](http://img.blog.csdn.net/20180129105140093?watermark/2/text/aHR0cDovL2Jsb2cuY3Nkbi5uZXQvWlMxMjNaUzEyM1pT/font/5a6L5L2T/fontsize/400/fill/I0JBQkFCMA==/dissolve/70/gravity/SouthEast)

如图所示为处于磁盘偏移量为0x0000000000的位置的内容是NTFS Boot Sector，`4E 54 46 53`为磁盘标识"NTFS",`0x0200` 表示每一个扇区的字节数为512字节，`0x08`表示每簇有8个扇区(每簇4096字节)，`0x00 00 00 00 26 3C EF FF`表示该分区总共的扇区数为641527807个(即305G)。查看一下F盘的属性。如下图所示，此时我们就可以通过修改这个位置的数据来修改磁盘的容量，达到使系统报告虚假容量的目的。可以直接使用winhex来修改(winhex要注册才能修改数据)，也可以自己写个小程序改这个位置。`0x00 00 00 00 00 0C 00 00`表示的是MFT的起始簇号，`0x00 00 00 00 00 00 00 02`表示MFT备份的起始簇号，`0x F6 00 00 00`表示每个MFT记录包含的246个簇(既984KB,每个条目1KB的话就包含984个条目)。 `0x00 00 00 01`表示每一个索引块的簇数为1个簇(8个扇区)。`0x36 F0 9D 9F F0 9D 65 C1`表示的是该磁盘卷的卷序列号。

![F盘信息](http://img.blog.csdn.net/20180129103129432?watermark/2/text/aHR0cDovL2Jsb2cuY3Nkbi5uZXQvWlMxMjNaUzEyM1pT/font/5a6L5L2T/fontsize/400/fill/I0JBQkFCMA==/dissolve/70/gravity/SouthEast)

## 2. MFT

​	从MBR中我们得到MFT的起始簇号为`0x00 00 00 00 00 0C 00 00`，接下来我们在winhex中转到MFT的起始位置786432簇(786432\*8\*512 = 3221225472 = 0xC0 00 00 00)，既相对文件系统的偏移量为`0xC0 00 00 00`的位置，如下图所示，

- `46 49 4C 45`是MFT标准头部的类型，值为FILE，表示这是一个文件。
- `0x 00 38` 表示第一个属性的偏移，即从`0x00 C0 00 00 00 +0x38`处开始是第一个属性的开始位置
- 跳到第一个属性处`0x00 C0 00 00 38`，属性类型为`0x00 00 00 10 ` 表示这是一个标准信息属性，包含内容为文件创建时间、修改时间等其他信息，这些时间和30属性中的是一样的，从下图中你可以发现这些时间都是`2C 4E 9F 9C 34 F5 D2 01`.
- `0x00 00 00 60`表示这个属性的长度为0x60，由于我们不关心10H属性，于是我们跳过这个属性到下一个属性的位置，0x00 C0 00 00 38 + 0x60 = 0x00 C0 00 00 98。
- 转到第二个属性为位置`0x00 C0 00 00 98`处， 属性类型为`0x00 00 00 30`，属性类型是FILENAME，属性长度(头部+属性体)是0x00 00 00 68，属性体的长度是0x00 00 00 48， 属性体开始的偏移值为0x00 18，既属性体开始的位置为 0x... 98+0x18 = 0x... B0
- `0x00 C0 00 00 B0`位置开始是属性体的内容，FILENAME属性的属性体的详细定义可以见文档，`05 00 00 00 00 00 05 00 `表示的是父目录的文件参考号(前6个字节48位为父目录的文件记录号，此处为0x05，即根目录，所以$MFT文件的父目录为根目录，后两个字节为序列号)，后面几个相同的`2C 4E 9F 9C 34 F5 D2 01`表示的分别是文件的创建、修改、MFT最后更新、最后访问时间(此处相同，其他处可能不同)。再后面的两个`0x40 00`分别是文件的分配大小和实际大小(此处相同，其他处可能不同)。
- `0x04`表示以字符计的文件名长度，每字符占用字节数由下一个字节的命名空间确定。`0x03`表示的是文件的命名空间(具体什么意思没弄明白)，接下来是文件名`24 00 4D 00 46 00 54 00 = $ M F T `，每个字符占用了两个空间推测为0x04 * （0x03-1）= 8。
- MFT起始位置为第一个条目(Entry)\$MFT,既MFT本身，我们此处知识借这个文件来分析一下MFT entry的结构，我们需要关注的不是这个entry，而是root entry，我们跳转到第五个entry----root entry的位置。每个entry的大小为0x400(1KB)，所以我们跳过5个entry的位置。`0xC0 00 00 00 +0x400*5 = 0xC0 00 14 00` 

![MFT本身](http://img.blog.csdn.net/20180129152147179?watermark/2/text/aHR0cDovL2Jsb2cuY3Nkbi5uZXQvWlMxMjNaUzEyM1pT/font/5a6L5L2T/fontsize/400/fill/I0JBQkFCMA==/dissolve/70/gravity/SouthEast)

### MFT root entry 

​	从上面可知MFT root entry的位置在`0x00 C0 00 00 14 00`的位置处，其内容如下图所示：

![根目录的前两个属性](http://img.blog.csdn.net/20180129164410828?watermark/2/text/aHR0cDovL2Jsb2cuY3Nkbi5uZXQvWlMxMjNaUzEyM1pT/font/5a6L5L2T/fontsize/400/fill/I0JBQkFCMA==/dissolve/70/gravity/SouthEast)

- `46 49 4C 45`为类型标准，表示FILE。
- `0x00 38`表示第一属性的偏移量， `0x00 03`表示这是一个目录。
- `0x3 B8`表示MFT的逻辑长度，即实际上使用了多少空间。 `0x4 00`表示MFT实际分配了多少空间。
- `0x00 0B`表示下一个被添加的属性的属性ID
- `0x00 00 00 05`表示该MFT的记录编号是第五个记录，上一个\$MFT文件中这个位置的值为`0x00`表示是第0个记录
- 跳到第一个属性的位置`0x00 C0 00 00 1438`处，属性类型为`0x10`,是标准信息属性，属性长度是`0x48`,我们跳过这个属性。查找下一个属性的位置，0x...1438 + 0x48 = 0x...1480.
- 转到1480处，属性类型为`0x30`,长度为`0x60`,`00`表示是常驻属性，`00`表示属性名的长度为0，`0x18`表示属性名的偏移值为0x18。`0x0001`是属性ID，表示这是第二个属性，因为前一个属性是`0x10`属性,其ID为`0x0000`
- `0x 00 00 00 44`表示属性体的长度为0x44，属性体的偏移量为0x18，剩下的位置处与上面的那个相同就不在分析，直接跳转到文件名处.
- `0x01`表示文件名的长度为1，`0x03`表示文件名的命名空间为0x03，所以接下来的2 Byte表示的是文件名，即`2E 00`,文件名为`. `,即表示根目录。

#### 40属性

​	接下来看root entry中30属性之后的下一个属性，我们此处的是40属性，如下图：

![40属性](http://img.blog.csdn.net/20180129172607516?watermark/2/text/aHR0cDovL2Jsb2cuY3Nkbi5uZXQvWlMxMjNaUzEyM1pT/font/5a6L5L2T/fontsize/400/fill/I0JBQkFCMA==/dissolve/70/gravity/SouthEast)

​	可以看到40属性的属性长度为0x28，是常驻属性，属性体的内容为`26 E0 0A AE 1E 61 E7 11 A0 9B 80 C1 6E F7 26 5F`。我们不关心这个属性体的具体内容，于是我们直接转到下一属性的位置0x...14E0 + 0x28 =  0x...1508 .

#### 50属性

​	在`0x...1508`的位置我们可以看到这个属性的类型是50，长度为0x01 00，如在40属性中的图所示，50属性记录的内容是和security相关的，我们不关心这部分的内容，所以直接跳到下一个属性的位置，0x...1508+0x100 = 0x...1608.

#### 90属性

​	转到0x00 C0 00 16 08的位置，在此处我们可以看到属性的类型是0x90 ，属性的长度是0xC8，90H属性是索引根属性，该属性是实现NTFS的B+树索引的根节点，它总是常驻属性。如下图所示：

![90属性](http://img.blog.csdn.net/20180129184938691?watermark/2/text/aHR0cDovL2Jsb2cuY3Nkbi5uZXQvWlMxMjNaUzEyM1pT/font/5a6L5L2T/fontsize/400/fill/I0JBQkFCMA==/dissolve/70/gravity/SouthEast)

- `0x04`表示属性名的长度为0x04*2=8Byte. `0x18`表示属性名的偏移地址为0x18.

- `0xA8 00 00 00`表示属性体的长度为0xA8，`0x00 20`表示属性体的偏移量为0x20。

- `24 00 49 00 33 00 30 00`表示属性名为$I30.接下来是90属性的属性体。

- `0x00 00 00 30`表示索引根的属性类型，`0x00 00 00 01`为排序规则。

- `0x00 00 10 00`表示的是索引项分配的大小(单位为字节)为4096字节，`0x01`表示每个索引记录的簇数为1个，正好对应索引项的大小为4096字节。

- `0x00 00 00 10`表示第一个索引项的偏移量为16字节。

- `0x00 00 00 98`表示索引项的总大小为0x98=152. 接下来的`0x00 00 00 98`表示索引项的分配大小。

- `0x01`为索引标志， 00表示Small directory，01表示Large directory。

- `57 BB 00 00 00 00 03 00`为文件的MFT参考号，前24位`0x00 00 00 00 BB 57`为该MFT的记录编号，可以用来定位这个文件。在该MFT处(0xC0 00 00 00 + 0x400*0xBB57 = 0xC2 ED 5C 00)可以看到内容如下图所示。

- `0x00 70`表示的是索引项的大小为112. `0x00 58`为索引标志，`0x00 01`表示此索引包含一个子节点，若为0表示此为最后一项。

- `05 00 00 00 00 00 05 00`为父目录的MFT、参考号，前6B为0x05表示此文件的父目录为根目录

- 接下来的四项分别是创建、文件修改、MFT修改、最后访问时间。

- `0x1D 40 00`为分配的空间，`0x1D 30 92`为实际使用的空间。

- `20 00 00 00 00 00 00 00`为文件标识，`0B`为文件名长度为11*2 字节，`00`为文件名命名空间。

- 接下来的22个字节是文件名的UNICODE编码，`4300 0768 C651 935E 906E E34E 0178 2E00 7A00 6900 7000`对应的正好是我的F盘下的 “C标准库源代码.zip” 这个文件的UNICODE编码。也就是说这个文件是NTFS的B+数的根节点。

  ![90](http://img.blog.csdn.net/20180129191139622?watermark/2/text/aHR0cDovL2Jsb2cuY3Nkbi5uZXQvWlMxMjNaUzEyM1pT/font/5a6L5L2T/fontsize/400/fill/I0JBQkFCMA==/dissolve/70/gravity/SouthEast)



#### A0属性

​	此处90属性之后是A0属性，90属性存储的是索引根节点的信息，而A0属性存储着组成索引的B+树目录索引子节点的定位信息。A0属性由属性头和运行列表组成，一般指向一个或者是多个目录(INDEX文件，即4K的缓存)，它总是常驻属性。A0属性和90属性共同描述了磁盘文件和目录的MFT记录的位置。第五项MFT的A0属性记录根目录的位置。

![A0属性](http://img.blog.csdn.net/20180130141644960?watermark/2/text/aHR0cDovL2Jsb2cuY3Nkbi5uZXQvWlMxMjNaUzEyM1pT/font/5a6L5L2T/fontsize/400/fill/I0JBQkFCMA==/dissolve/70/gravity/SouthEast)

- `0x00 00 00 50`表示该属性的长度为0x50。`0x01`表示该属性是非常驻属性，`0x04`表示属性名的长度为4*2=8Byte, `0x00 40`表示属性名的偏移。`0x00 08`是属性的ID

- `00 00 00 00 00 00 00 00`为起始的VCN，`01 00 00 00 00 00 00 00 `为结束的VCN，即占用两个簇。

- `48 00`表示run list的偏移为0x48，接下来的三个`00 20 00 00 00 00 00 00 `分别是分配的大小，实际使用的大小，原始的大小。

- `24 00 49 00 33 00 30 00`为属性名，表示`$ I 3 0 `

- `31 02 DF 51 09`为run list。根据run list 的特殊计算方式，31代表后面的3+1个字节是run list的内容，第一个字节的低位1代表后面的一个字节表示簇数，即`02`代表簇数为2个。第一个字节的高位3代表后三字节为簇偏移量，即`DF 51 09`代表起始的簇号为`0x09 51 DF = 610783(簇) =2501767168(Byte)=0x951D F000 `,即相对文件系统的偏移为0x951D F000的位置。下面我们转到这个位置。




####索引的位置

​	根目录的索引节点由索引头和一个个的索引项构成，**其中一个个索引项和根目录中的每一个文件或者目录相对应**。这些索引项的文件名就是根目录中的文件或者目录的名称。我们现在要找的是根目录下目录名为git的索引项。

​	由之前的A0属性中我们得知了索引子节点的run list占用两个簇，因此我们需要遍历这两个簇来查找目录git的索引项，先从0x951D F000(第一个簇的位置)的位置开始遍历，没找到这个目录，因此这一个簇的图就不放出，接下来遍历第二个簇的位置0x951D E000开始的位置，如下图所示：

![INDEX](http://img.blog.csdn.net/20180130163054482?watermark/2/text/aHR0cDovL2Jsb2cuY3Nkbi5uZXQvWlMxMjNaUzEyM1pT/font/5a6L5L2T/fontsize/400/fill/I0JBQkFCMA==/dissolve/70/gravity/SouthEast)

- 从E000-E030的内容是标准的索引头部，剩下的每一个颜色块代表一个索引项。
- 关注黄色的git目录的索引块的内容，`7D 05 01 00 00 00 06 00`是git目录的MFT参考号，其中`0x1057D`是MFT记录号，跳到这个记录的位置，0xC000 000 + 0x1057D*0x400 = 0xC755 F400.

#### git目录的MFT文件

![gitMFT](http://img.blog.csdn.net/20180130171055776?watermark/2/text/aHR0cDovL2Jsb2cuY3Nkbi5uZXQvWlMxMjNaUzEyM1pT/font/5a6L5L2T/fontsize/400/fill/I0JBQkFCMA==/dissolve/70/gravity/SouthEast)

如图所示为git目录的MFT的内容，400-438为位置的内容为MFT标准头部的信息，不在介绍。剩下的每一个颜色块的内容为一个属性。我们关心的是他的90属性的内容。重点讲解这个。

![git90属性](http://img.blog.csdn.net/20180130173819806?watermark/2/text/aHR0cDovL2Jsb2cuY3Nkbi5uZXQvWlMxMjNaUzEyM1pT/font/5a6L5L2T/fontsize/400/fill/I0JBQkFCMA==/dissolve/70/gravity/SouthEast)

- 从520-540的部分为标准属性头部，540-560为索引根和索引头的结构。
- 560-5C8是第一个索引项的内容，5C8-628是第二个索引项的内容，因为我的git目录下只有两个目录，所以只有这两个索引项。
- 下面关注的就是myDocument这个索引项，从上图我们可以看到这个文件的MFT参考号为`7E D5 01 00 00 00 06 00`，所以MFT号为0x1D57E。定位到这个MFT号的位置，0xC000 000 + 0x1 D57E*0x400 = 0xC755 F800.

#### myDocument目录的MFT

![myDocumentMFT](http://img.blog.csdn.net/20180130184844062?watermark/2/text/aHR0cDovL2Jsb2cuY3Nkbi5uZXQvWlMxMjNaUzEyM1pT/font/5a6L5L2T/fontsize/400/fill/I0JBQkFCMA==/dissolve/70/gravity/SouthEast)

从上图我们可以看到myDocument的MFT中有6个属性，我们关心的是其90属性和A0属性。

***此处的90属性没有记录什么有用的信息，我们什么时候应该看90属性，什么时候看A0属性?***



![A0属性](http://img.blog.csdn.net/20180130190405986?watermark/2/text/aHR0cDovL2Jsb2cuY3Nkbi5uZXQvWlMxMjNaUzEyM1pT/font/5a6L5L2T/fontsize/400/fill/I0JBQkFCMA==/dissolve/70/gravity/SouthEast)

​	查看A0属性，此处的A0属性是非常驻属性。属性大小为0x50，属性名为`24 00 49 00 33 00 30 00`即“\$ I 3 0 ”. run list为`41 01 70 C9 A2 00 00 00`,根据run list的规则，占用的空间为1簇， 起始的簇号为0xA2C970。定位到这个簇的位置。即offset的位置为：0xA2 C970 *0x8 * 0x200(512B) = 0xA 2C97 0000.

#### myDocument目录下文件索引的位置

![Index](http://img.blog.csdn.net/20180130192906832?watermark/2/text/aHR0cDovL2Jsb2cuY3Nkbi5uZXQvWlMxMjNaUzEyM1pT/font/5a6L5L2T/fontsize/400/fill/I0JBQkFCMA==/dissolve/70/gravity/SouthEast)

如上图所示，每一个颜色块代表的是一个索引项，由上一个MFT中的内容我们知道这个索引块占用一个簇，我们需要遍历这个簇来找到lora.pptx文件的索引，它不在上面这幅图中，我们找到这个索引的位置如下。

![Index](http://img.blog.csdn.net/20180130194952452?watermark/2/text/aHR0cDovL2Jsb2cuY3Nkbi5uZXQvWlMxMjNaUzEyM1pT/font/5a6L5L2T/fontsize/400/fill/I0JBQkFCMA==/dissolve/70/gravity/SouthEast)

其中黄色的区域即为lora.pptx文件的索引，从这块区域中我们可以得到lora.pptx文件的MFT参考号为`17 F2 05 00 00 00 01 00`,父目录的参考号为`7E D5 01 00 00 00 06 00`.定位到"lora.pptx"文件的位置:0xC000 0000 + 0x5F217 * 0x400 = 0xD7C8 5C00。

在此位置没有找到记录？？？？？







