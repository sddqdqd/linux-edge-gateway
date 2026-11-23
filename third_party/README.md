# Third-Party Libraries (Week 3 Integration Guide)

为了保持仓库轻量并锻炼第三方开源组件集成能力，请在第 3 周自行从 GitHub 拉取以下两个单文件级 C 库：

## 1. cJSON（配置解析）

- 仓库：https://github.com/DaveGamble/cJSON
- 只需复制以下 2 个文件到 `third_party/cjson/`：
  - `cJSON.h`
  - `cJSON.c`

## 2. rxi/log.c（轻量日志库）

- 仓库：https://github.com/rxi/log.c
- 只需复制以下 2 个文件到 `third_party/log/`：
  - `src/log.h`
  - `src/log.c`

## 接入方式

复制完成后，在根目录 `CMakeLists.txt` 的 `GATEWAY_SOURCES` 中追加：

```cmake
    third_party/cjson/cJSON.c
    third_party/log/log.c
```

注意在 C++ 源码中包含 C 头文件时使用 `extern "C"`（`cJSON.h` 内部已自带 `extern "C"` 保护，`log.h` 若无则需在包含处加 `extern "C" { #include "log/log.h" }`）。
