# [simplesvr](http://github.com/simplejia/simplesvr) (simple udp server)
## 功能：
* 超简单c/c++服务，多进程，udp通信，没有高深复杂的事件驱动，没有多线程带来的数据共享问题（加锁对性能的影响），代码结构简单，直达业务
* 适用场景：业务逻辑重，追求高吞吐量，容忍udp带来的不可靠。（已有c lib库，不方便采用golang包装时）
* c开发新手也可以快速上手

## 特性
* 代码结构简单，仅有一个.cpp文件：main/main.cpp，其它均是.h文件。
* 调用协议简单，'\x00'分隔字段
* 多进程，同时启动多个业务子进程，任何一个进程（包括父进程）退出，所有其它进程均退出。
* 支持json格式配置文件
* 可选通过clog方式记录日志并报警
* 提供很多有用的小组件，包括：
> 简单高效的http get及post操作组件
> 类似go lc的本地缓存组件（支持lru, 支持过期后还能返回旧数据，这个在获取新数据失败时尤其有用）
* 提供些小的库函数，如：定时器，获取本机内网ip等


## 注意
* 加入新依赖库时，只需要在main/main.cpp里加入库头文件，修改Makefile文件
* api目录提供api.go，api.php示例代码用于和simplesvr服务通信

## LICENSE
simplesvr is licensed under the Apache Licence, Version 2.0
(http://www.apache.org/licenses/LICENSE-2.0.html)
