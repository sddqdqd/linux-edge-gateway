# 系统架构与模块划分设计

## 1. 总体架构（V1 单 Reactor + Worker 线程池）

本系统在 V1 阶段采用经典的 **单 Reactor + 业务线程池** 并发模型，严格分离 **I/O 事件调度** 与 **业务指令执行**，避免多线程并发读写同一个 Socket 导致的竞态与数据错乱问题。

```text
                    +---------------------------------------------------+
                    |              Main Thread (IO Reactor)             |
                    |                                                   |
   TCP Clients <====>  epoll_wait (non-blocking accept / read / write)  |
                    |        |                                 ^        |
                    |        v                                 |        |
                    |   RingBuffer -> FrameCodec (拆包/CRC)    | eventfd|
                    |        |                                 | 唤醒   |
                    |        v                                 |        |
                    |   Submit Task                     Pending Write   |
                    +--------|---------------------------------|--------+
                             |                                 |
                             v                                 |
                    +---------------------------------------------------+
                    |              Worker ThreadPool (N threads)        |
                    |                                                   |
                    |  Dispatch Cmd -> HAL (C Interface) -> Pack Reply  |
                    +---------------------------------------------------+
                                             |
                                             v
                    +---------------------------------------------------+
                    |          HAL Layer (include/hal/hal_device.h)     |
                    |  [V1] hal_sysfs.c   : /sys/class/thermal, leds    |
                    |  [V2] hal_driver.c  : 自研 /dev/my_i2c_sensor     |
                    +---------------------------------------------------+
```

### 关键并发约束（必须遵守）

1. **Socket 归属唯一**：所有客户端 `fd` 的 `read()`、`write()` 和 `close()` **仅允许在主 Reactor 线程中执行**。
2. **跨线程回包机制**：Worker 线程执行完业务并封装好响应帧后，将 `(fd, response_bytes)` 投递到线程安全的发送响应队列，然后向 `eventfd` 写入一个 `uint64_t` 数值，触发主线程 `epoll_wait` 唤醒并统一写回对应连接（妥善处理 `EAGAIN` 半写场景）。
3. **心跳超时清理**：主线程在 `epoll_wait` 中设置合理超时（如 1000 ms），每次唤醒周期性扫描连接表，主动关闭超过 `heartbeat_timeout_sec` 未收到有效帧的连接。

---

## 2. 核心模块职责划分

| 目录 | 模块 | 语言 | 核心职责 |
| :--- | :--- | :--- | :--- |
| `common/` | `NonCopyable`, `Config` | C++11 | 禁用拷贝语义；通过 `cJSON` 加载 `gateway.json` 运行参数 |
| `protocol/` | `RingBuffer`, `FrameCodec` | C++11 | 字节流环形缓存；二进制帧打包/拆包、大端序转换、CRC16-MODBUS 校验、错误帧滑窗重同步 |
| `concurrency/` | `ThreadPool` | C++11 | 固定线程数池化管理；基于 `mutex` + `condition_variable` 的任务队列与安全停机 |
| `net/` | `Connection`, `ReactorServer` | C++11 | 单连接上下文管理（收/发缓冲、活跃时间）；非阻塞 Socket、`epoll` 事件循环、`eventfd` 唤醒、心跳巡检 |
| `hal/` | `hal_device` | C11 | 纯 C 硬件抽象层，屏蔽底层文件节点或内核驱动差异 |
| `main.cpp` | 启动与生命周期管理 | C++11 | 命令行参数解析、守护进程化（可选）、`SIGINT`/`SIGTERM` 信号捕获与优雅退出 |

---

## 3. V2 演进预留设计（弱网缓存 + MQTT + 自研驱动）

为了保证 V1 代码在 V2 阶段无需推倒重写，架构中预留了三个干净的扩展边界：

1. **南向硬件插槽（`hal_device.h`）**：
   - V1 编译链接 `src/hal/hal_sysfs.c`。
   - V2 阶段只需新增 `src/hal/hal_driver.c`（调用自研 I2C/SPI 驱动的 `open`/`read`/`ioctl`/`poll`）并在 `CMakeLists.txt` 中切换源文件，上层网络与并发代码零修改。
2. **本地容灾存储插槽（规划目录 `src/storage/`）**：
   - 新增 `OfflineCache` 类，封装 SQLite 数据库，按最大条数/最大磁盘占用执行 FIFO 环形淘汰，防止嵌入式 Flash 写满。
3. **北向云端上报插槽（规划目录 `src/cloud/`）**：
   - 新增 `MqttClient` 类（封装 `paho.mqtt.c`），独立线程定时通过 `hal_device` 采集数据并上报；检测到断网或发布失败时自动写入 `OfflineCache`，恢复连接后异步按序补传。
