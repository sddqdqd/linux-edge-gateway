# i.MX6ULL Edge Gateway (V1 -> V2)

基于 NXP i.MX6ULL 嵌入式 Linux 平台的轻量级边缘网关项目。  
应用层采用 **现代 C++11**（RAII、智能指针、多线程同步）实现单 Reactor + 线程池架构；底层硬件抽象层（HAL）采用 **纯 C** 接口，支持 V1 阶段通过 Linux sysfs 模拟外设，并在 V2 阶段无缝替换为自研 Linux 字符/I2C/SPI 设备驱动，同时预留弱网离线缓存（SQLite）与 MQTT 上云扩展位。

---

## 1. 目录结构

```text
imx6ull-edge-gateway/
├── CMakeLists.txt                  # 主构建脚本（支持 x86 本机调试/ASan 与 ARM 交叉编译）
├── cmake/
│   └── arm-imx6ull-toolchain.cmake # i.MX6ULL 交叉编译工具链配置
├── config/
│   └── gateway.json                # 网关默认配置文件
├── docs/
│   ├── architecture.md             # 系统架构、模块职责与 V2 演进蓝图
│   ├── protocol.md                 # 自定义二进制通信协议 V1 规格书
│   └── cpp11_cheatsheet.md         # 本项目所需 C++11 核心语法速查
├── include/
│   ├── common/                     # 公共基类与配置解析接口
│   ├── protocol/                   # 环形缓冲区与二进制帧编解码接口
│   ├── net/                        # 客户端连接与 epoll Reactor 服务端接口
│   ├── concurrency/                # 固定大小线程池接口
│   └── hal/                        # 纯 C 硬件抽象层接口（驱动插槽）
├── src/                            # 对应模块的实现文件（含分阶段 TODO 指引）
├── third_party/                    # 轻量第三方库（cJSON、log.c）接入说明
└── tests/                          # 单元测试与 Python 粘包/半包/并发压测规划
```

---

## 2. 构建与运行

### 2.1 Ubuntu x86 主机调试构建（推荐开发阶段首选）

开启 AddressSanitizer（ASan）可在运行阶段直接定位内存泄漏与越界访问：

```bash
cmake -B build-x86 -DCMAKE_BUILD_TYPE=Debug -DENABLE_ASAN=ON
cmake --build build-x86 -j$(nproc)
./build-x86/edge_gateway --config config/gateway.json
```

### 2.2 i.MX6ULL 开发板交叉编译

```bash
cmake -B build-arm -DCMAKE_TOOLCHAIN_FILE=cmake/arm-imx6ull-toolchain.cmake -DCMAKE_BUILD_TYPE=Release
cmake --build build-arm -j$(nproc)
```

将产物与配置文件通过 `scp` 或 NFS 部署到开发板：

```bash
scp build-arm/edge_gateway config/gateway.json root@<BOARD_IP>:/root/gateway/
```

---

## 3. V1 三周开发里程碑

- **热身（3~4 天）**：阅读 `docs/cpp11_cheatsheet.md`，验证 `arm-linux-gnueabihf-g++` 交叉编译环境。
- **第 1 周（协议与单连接闭环）**：
  - 实现 `RingBuffer` 与 `FrameCodec`（CRC16、网络字节序、粘包/半包拆分、脏数据滑窗恢复）。
  - 跑通基础 TCP 收发与单连接指令回包测试。
- **第 2 周（并发 Reactor + 线程池 + HAL）**：
  - 实现 `ThreadPool`（互斥锁 + 条件变量 + 任务队列）。
  - 实现 `ReactorServer`（非阻塞 `epoll` + `eventfd` 跨线程写回唤醒 + 心跳超时清理）。
  - 实现 `hal_sysfs.c`（读取板载温度、系统内存、控制 LED 亮灭）。
- **第 3 周（工程化与稳定性压测）**：
  - 集成 `cJSON` 与 `log.c`，完成 `Config` 解析与日志落盘。
  - 完善守护进程模式（`--daemon`）与信号优雅退出（`SIGINT`/`SIGTERM`）。
  - 编写 Python 自动化测试脚本，完成粘包、半包、错字节、高并发、异常断连测试与 ASan / `tcpdump` / `VmRSS` 稳定性验证。

---

## 4. V2 升级路线（学完系统移植与驱动篇后）

1. **驱动替换**：新增 `src/hal/hal_driver.c`，对接自研 I2C/SPI 传感器设备节点，替换 `hal_sysfs.c`，上层业务代码零改动。
2. **弱网容灾与上云**：新增 `storage`（SQLite 定长环形离线缓存）与 `cloud`（MQTT 客户端、断网重连、QoS、遗嘱消息）模块，完成端云协同。
