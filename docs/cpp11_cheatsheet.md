# 本项目 C++11 核心语法速查（3~4 天热身清单）

无需通读厚重教材，掌握以下 5 个实战主题即可顺畅完成本项目 V1 开发，并直接覆盖嵌入式 Linux C++ 面试高频考点。

---

## 1. RAII（资源获取即初始化）与禁止拷贝

核心思想：把文件描述符（`fd`）、互斥锁、堆内存绑定到对象生命周期上，**构造函数申请，析构函数释放**，彻底避免 `return` 分支遗漏导致的泄漏。

```cpp
class ScopedFd {
public:
    explicit ScopedFd(int fd) : fd_(fd) {}
    ~ScopedFd() {
        if (fd_ >= 0) ::close(fd_);
    }
    // 禁止拷贝，防止重复 close 同一个 fd
    ScopedFd(const ScopedFd&) = delete;
    ScopedFd& operator=(const ScopedFd&) = delete;

    int get() const { return fd_; }
private:
    int fd_;
};
```

本项目应用：`Connection` 析构关闭连接、继承 `NonCopyable` 禁用误拷贝、封装 `cJSON*` 自动释放。

---

## 2. 智能指针（`unique_ptr` / `shared_ptr`）

- **`std::unique_ptr<T>`**：独占所有权，几乎零运行时开销，嵌入式首选。不能拷贝，只能通过 `std::move` 转移所有权。
- **`std::shared_ptr<T>`**：共享所有权，底层带原子引用计数，适合多个模块或跨线程安全共享同一对象。

```cpp
// C++11 注意：std::make_unique 是 C++14 才引入的，C++11 用 new 构造 unique_ptr
std::unique_ptr<Connection> conn(new Connection(client_fd));
connections_.emplace(client_fd, std::move(conn));
```

---

## 3. 线程与同步原语（`<thread>`, `<mutex>`, `<condition_variable>`, `<atomic>`）

直接替代繁琐的 `pthread_mutex_t` / `pthread_cond_t` 初始化与销毁代码：

```cpp
std::mutex mtx_;
std::condition_variable cv_;
std::queue<std::function<void()>> tasks_;
std::atomic<bool> stop_{false};

// 生产者投递任务
{
    std::lock_guard<std::mutex> lock(mtx_); // 离开作用域自动解锁
    tasks_.push(std::move(task));
}
cv_.notify_one();

// 消费者等待任务（注意传谓词 Lambda，天然防止虚假唤醒）
std::unique_lock<std::mutex> lock(mtx_);
cv_.wait(lock, [this]() { return stop_.load() || !tasks_.empty(); });
```

---

## 4. 高频 STL 容器

| 容器 | 本项目用途 | 注意事项 |
| :--- | :--- | :--- |
| `std::vector<uint8_t>` | 二进制帧载荷、发送缓冲区 | 连续内存，可用 `buf.data()` 直接传给 `read`/`write` |
| `std::unordered_map<int, std::unique_ptr<Connection>>` | 按 `fd` $O(1)$ 查找活跃客户端连接 | 遍历时删除元素必须用 `it = map.erase(it)` |
| `std::queue<T>` | 线程池任务队列、待发送响应队列 | 本身非线程安全，必须配合 `std::mutex` 使用 |
| `std::string` | 配置路径、日志文本 | 与 C 接口交互时调用 `.c_str()` |

---

## 5. Lambda 表达式与 `std::function`

用于将“处理某条请求并回包”的逻辑打包成任务扔进线程池：

```cpp
// 按值捕获 [=] 或显式捕获 [this, fd, frame]，切忌在异步任务中按引用捕获局部变量！
pool_.submit([this, fd, frame]() {
    Frame resp = handleBusiness(frame);
    enqueueResponse(fd, std::move(resp));
});
```

---

## 热身自测（建议亲手在 Ubuntu 里敲完并编译运行）

1. 写一个 30 行小程序：主线程往 `std::queue<int>` 塞 100 个数，2 个子线程用 `mutex + condition_variable` 消费并累加到 `std::atomic<int>`。
2. 写一个封装 `FILE*` 的 RAII 类，验证离开作用域后自动 `fclose`，并用 `-fsanitize=address` 确认无内存泄漏。
3. 将上述测试用 `arm-linux-gnueabihf-g++ -std=c++11 -pthread` 交叉编译并扔到 i.MX6ULL 开发板运行一次。
