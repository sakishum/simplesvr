# [simplesvr](http://github.com/simplejia/simplesvr) (simple udp server)
## Functions：
* Very simple c/c++ service, multiple processes, udp communication, without event driven  model complexity, without data sharing problems caused by multiple threads (the effect on performance caused by lock), simple code structure and reaching business directly.
* Applicable situations: with heavy business logic, pursuing high throughout and tolerance to unreliability caused by udp. (already having c lib and not convenient to use golang to implement)
* Newbie of C can get started quickly

## Features
* The code structure is simple with only one .cpp file: main/main.cpp. All other files are .h files.
* Protocol is simple, using '\x00' to separate fields
* Multiple processes, starting multiple business child process at the same time. Any process(including parent process) exits, all other processes will exit.
* Support configuration files with json format
* Able to record logs and alarm via clog
* Providing many useful small components, including:
  * Simple and efficient http get and post operation components
  * Local cache components similar to go lc(supporting lru, able to return to old data even the data is expired which is very useful when it fails to get new data)
* Providing some small library functions, such as: timer, getting inner ip address


## Notice
* While adding new dependencies, we just need add header file in main/main.cpp and revise Makefile files.
* Api directory provides api.go and api.php example code which is used to communicate with simplesvr service.

## Starting
* simplesvr prod
  * Prod is an optional parameter, standing for starting the designated running environment in configuration files, default: prod
  * Recommending using [cmonitor](http://github.com/simplejia/cmonitor) to manage

---

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
  * 简单高效的http get及post操作组件
  * 类似go lc的本地缓存组件（支持lru, 支持过期后还能返回旧数据，这个在获取新数据失败时尤其有用）
* 提供些小的库函数，如：定时器，获取本机内网ip等


## 注意
* 加入新依赖库时，只需要在main/main.cpp里加入库头文件，修改Makefile文件
* api目录提供api.go，api.php示例代码用于和simplesvr服务通信

## 启动方式
* simplesvr prod
  * prod是可选参数，代表启动配置文件里指定的运行环境，默认: prod
  * 建议通过[cmonitor](http://github.com/simplejia/cmonitor)来启动管理
