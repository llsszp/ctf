不得不说，这题的形式听有意思的，可以说是之前从没遇到过。

这题是 nc 到所给端口后，返回的是一个 ELF 文件，然后大概看了下，就是直接读入输出，读入字符串长度明显过长了，看起来似乎是个很水的栈溢出。

然而，比较奇怪的是，这个 ELF 监听的端口不是我们连上去的那个端口，并且也没有拿我们的读入啊，怎么看都觉得功能不太对劲啊……

本来已然蒙圈，扫端口也看不出个啥来，结果跟队友聊的时候，他说的程序监听的端口似乎跟我看到的不太一样，于是瞬间就明白了，这题应该是随机生成一个 ELF 文件，然后运行这个程序几秒钟，我们要在这几秒钟连上对应的端口完成溢出。

首先，多次获取 ELF 对比发现，其实一共有三个量是随机的：程序监听的端口、读入的字符串长度、字符串缓冲区长度，那么也就是说其实只有一部分情况下是有溢出的，不过无所谓了，也就是多跑几次而已了。

然后我们要做的事很显然，首先要从程序中解析出这三个量，用字符串查找或者 pwntools 的 disasm 都行。

解析完成后，由于程序是 64 位，我们无法直接传递参数，需要构造 ROP 链，我们在发过来的 binary 中可以找到修改 rdi rsi 的 gadget，从而可以调用 write 来输出 got 表，计算 libc，从而拿到更多 gadget（事实上，似乎在 ASLR 关闭的情况下，不同机器的 libc 地址也是一样的，所以可以本机关掉 ASLR 查看地址，这一点是对比本机和拿到的服务器的 libc 得出的，不是很确定）。

在这个过程中，有几点比较坑。由于程序是自己维护的 socket，所以不能直接输出的 stdout，必须用 write 发到正确的 fd 上，而且，由于 fd 正好又在缓冲区的后面，所以覆盖的时候，不能改变 fd 的值，否则就算溢出成功，我们也拿不到任何反馈。好在这里的 fd 的值每次运行的时候都是一样的，所以我们可以在一次溢出中，正好覆盖到 fd 前面，然后服务器 write 的时候，由于没有遇到 `\0`，就会把 fd 一起返回回来。

libc 拿到之后，我们可以调用 system 了，但是还是要注意，不能起 `/bin/sh`，因为会断 socket，只能 nc 或者 curl 什么的方式将 flag 传出来。为了将要执行的命令字符串传入，我们其实调用 read 读入到一个空闲区域会方便很多。而我脑抽的想通过栈传递，于是乎花了很多的时间在栈上定位输入字符串，同时还某步为图方便，掉入某深坑，以及经历服务器重启，需要重新定位栈的尴尬。

总之，这题其实用到的知识点并不难，只是形势很新颖，同时由于不是常见的 xinted 的模式，导致泄漏信息的时候会比较麻烦～～～