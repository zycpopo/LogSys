# popolog — C++ 日志库

popolog 是一个用 C++11 实现、**纯头文件**的轻量级日志库。它采用模块化分层设计，贯穿了多种经典设计模式（建造者、单例、工厂、代理、模板方法、策略），支持**同步 / 异步**两种日志模式，以及**屏幕 / 文件 / 按大小滚动文件**三种落地方式，并允许自定义日志格式。

## 特性

- 纯头文件实现，零安装，`#include` 即可使用
- 同步日志器 `SyncLogger` 与异步日志器 `AsyncLogger` 可自由切换
- 异步基于**双缓冲 + 后台工作线程**，批量落地，吞吐高；支持有界（`ASYNC_SAFE`）与无界（`ASYNC_UNSAFE`）两种背压策略
- 三种落地方式：`StdoutSink`（屏幕）、`FileSink`（文件）、`RollyBySizeSink`（按大小滚动文件）
- 一个日志器可同时挂载多个 sink，多路输出
- 自定义日志格式（pattern 模板），支持时间 / 线程 ID / 日志器名 / 文件名 / 行号 / 等级 / 消息 / 缩进 / 换行
- 全局门面接口 + 宏代理，自动填充文件名与行号
- 日志器统一由单例 `LoggerManager` 管理，内置 root 日志器

## 环境要求

- 目标平台：**Linux / 类 POSIX**（代码依赖 `vasprintf`、`localtime_r`、`unistd.h`、`pthread`）
- 编译器：支持 C++11 的 g++ / clang++（makefile 默认 `g++ -Wall -g`）
- 注意：当前代码在 **Windows / MinGW 下不能直接编译**（`mkdir` 双参数调用与 `vasprintf` 均为 POSIX/GNU 接口），如需在 Windows 上运行请使用 WSL 或 MSYS2 等 POSIX 环境。

## 快速开始

### 编译运行

```bash
make        # 生成可执行文件 test
./test      # 运行测试程序
make clean  # 清理编译产物
```

或手动编译：

```bash
g++ -Wall -g test.cc -o test -pthread
```

### 最小示例

```cpp
#include "popolog.hpp"

int main()
{
    // 大写宏：直接通过 root 日志器打印
    INFO("hello %s", "popolog");

    // 小写宏：配合日志器对象使用，自动填充 __FILE__ / __LINE__
    auto logger = popolog::getLogger("async_logger");  // 需先注册，见下文
    logger->debug("debug message %d", 1);
    return 0;
}
```

## 架构

### 模块分层

```
门面层   popolog.hpp         全局接口 + 宏代理
业务层   logger.hpp          日志器（同步/异步）、建造者、管理器
功能层   format/sink/looper  格式化、落地、异步工作器
基础层   util/level/message/buffer  工具、等级、消息、缓冲区
```

依赖单向向下、无循环：

```
util.hpp（工具，最底层）
  ├── level.hpp    日志等级
  ├── message.hpp  日志消息（依赖 util/level）
  ├── buffer.hpp   异步缓冲区（依赖 util）
  ├── format.hpp   格式化（依赖 util/message/level）
  ├── sink.hpp     落地（依赖 util）
  └── looper.hpp   异步工作器（依赖 buffer）
logger.hpp（日志器核心，依赖以上全部）
popolog.hpp（门面，依赖 logger）
```

### 各模块职责

| 文件 | 职责 |
|---|---|
| `util.hpp` | 工具类：`Date::getTime()` 取时间戳；`File::exists/path/createDirectory` 判断文件、取父目录、递归建目录 |
| `level.hpp` | 日志等级枚举 `UNKNOW ~ OFF`，及 `tostring()` 等级转字符串 |
| `message.hpp` | `LogMsg` 结构体，一条日志的全部信息：时间、等级、行号、线程 ID、文件名、日志器名、消息正文 |
| `buffer.hpp` | 异步缓冲区：双指针（读/写下标）的 `vector<char>`，自动扩容（先指数增长，超 80MB 阈值后线性增长），支持 `push/swap/reset/empty` |
| `looper.hpp` | 异步工作线程（生产者-消费者）：双缓冲 `_pro_buf/_con_buf` 整体 swap 后批量落地；`ASYNC_SAFE` 有界阻塞，`ASYNC_UNSAFE` 无界 |
| `format.hpp` | 格式化：`FormatItem` 抽象基类 + 10 个子类；`Formatter` 解析 pattern 字符串并映射到各子项 |
| `sink.hpp` | 落地：`LogSink` 抽象基类；`StdoutSink` / `FileSink` / `RollyBySizeSink`；`SinkFactory` 工厂统一创建 |
| `logger.hpp` | 核心：`Logger` 抽象基类（含 5 个日志方法）、`SyncLogger`、`AsyncLogger`、`LoggerBuilder` 及其子类、`LoggerManager` 单例 |
| `popolog.hpp` | 门面：`getLogger(name)` / `rootLogger()`；小写宏自动补 `__FILE__/__LINE__`；大写宏走 root 日志器 |

### 一条日志的完整流程

```
用户调用（宏 / 接口）
  │
  ▼
Logger::debug/info/warn/error/fatal(file, line, fmt, ...)
  │  ① 等级过滤：未达到 _limit_level 直接返回（不做格式化）
  │  ② vasprintf：变参格式化 → 字符串
  ▼
serialize()
  │  ③ 构造 LogMsg（时间/线程/等级/文件/行/日志器名/消息）
  │  ④ Formatter::format() → 最终日志字符串
  ▼
log(字符串)   ← 模板方法分叉点
  ├── SyncLogger:  加锁 → 遍历所有 sink → 直接落地
  └── AsyncLogger: push 进 Buffer → 后台线程 swap → 遍历 sink 批量落地
```

## 设计模式

| 模式 | 位置 | 说明 |
|---|---|---|
| 模板方法 | `Logger::log()` | 基类定好"构造消息→格式化→落地"骨架，落地由子类实现 |
| 策略 | `LogSink` 及其子类 | 落地策略可互换、可组合（一个日志器挂多个 sink） |
| 工厂 | `SinkFactory` | 统一创建 sink 对象 |
| 建造者 | `LoggerBuilder` + 两个子类 | 屏蔽 Logger 构造的复杂参数；`GlobalLoggerBuilder` 建完自动注册进单例 |
| 单例 | `LoggerManager` | C++11 局部静态变量实现线程安全单例，内置 root 日志器 |
| 代理（宏） | `popolog.hpp` | 小写宏自动注入 `__FILE__ / __LINE__`；大写宏直达 root 日志器 |
| 生产者-消费者 / 双缓冲 | `AsyncLooper` | 生产线程写 `_pro_buf`，消费线程 swap 走 `_con_buf` 批量落地 |

## 使用指南

### 1. 全局宏（最简用法）

```cpp
#include "popolog.hpp"

// 大写宏：直接使用内置 root 日志器
DEBUG("..."); INFO("..."); WARN("..."); ERROR("..."); FATAL("...");

// 小写宏：配合 Logger 对象，自动填充文件名与行号
popolog::Logger::ptr logger = popolog::getLogger("my_logger");
logger->debug("value = %d", 42);
logger->error("%s", "something wrong");
```

### 2. 构建并注册自定义日志器（推荐）

通过 `GlobalLoggerBuilder` 链式配置，`build()` 后自动注册到 `LoggerManager`：

```cpp
#include "popolog.hpp"
#include "logger.hpp"

int main()
{
    std::unique_ptr<popolog::LoggerBuilder> builder(new popolog::GlobalLoggerBuilder());
    builder->buildLoggerName("async_logger");                                   // 日志器名
    builder->buildLoggerLevel(popolog::LogLevel::value::WARN);                   // 最低输出等级
    builder->buildFormatter("[%d{%H:%M:%S}][%t][%c][%f:%l][%p]%T%m%n");         // 自定义格式
    builder->buildLoggerType(popolog::LoggerType::LOGGER_ASYNC);                 // 同步/异步
    // builder->buildEnableUnSafeAsync();                                        // 切到无界异步模式
    builder->buildSink<popolog::FileSink>("./logfile/async.log");               // 落地到文件
    builder->buildSink<popolog::StdoutSink>();                                   // 同时落地到屏幕
    builder->build();                                                            // 构建并注册

    auto logger = popolog::getLogger("async_logger");
    logger->warn("hello %s", "async world");
    return 0;
}
```

`LocalLoggerBuilder` 用法相同，但不会注册到单例，适合创建只在局部使用的日志器。

### 3. 落地方式（Sink）配置

```cpp
builder->buildSink<popolog::StdoutSink>();                                     // 输出到屏幕
builder->buildSink<popolog::FileSink>("./logfile/test.log");                   // 输出到指定文件
builder->buildSink<popolog::RollyBySizeSink>("./logfile/roll-", 1024 * 1024);  // 超 1MB 滚动新文件
```

滚动文件命名：`basename + 年月日时分秒 + 序号 + .log`，如 `roll-20260812103015+0.log`。

### 4. 日志格式 pattern 说明

| 占位符 | 含义 | 说明 |
|---|---|---|
| `%d` | 时间 | 支持子格式，如 `%d{%Y-%m-%d %H:%M:%S}`；默认 `%H:%M:%S` |
| `%t` | 线程 ID | |
| `%c` | 日志器名称 | |
| `%f` | 源码文件名 | |
| `%l` | 源码行号 | |
| `%p` | 日志等级 | |
| `%T` | 制表符缩进 | |
| `%m` | 主体消息 | |
| `%n` | 换行 | |
| `%%` | 字面量 `%` | |

默认格式：`[%d{%H:%M:%S}][%t][%c][%f:%l][%p]%T%m%n`

### 5. 日志等级

`UNKNOW < DEBUG < INFO < WARN < ERROR < FATAL < OFF`。设置 `_limit_level` 后，低于该等级的日志会被过滤。

## 已知问题与注意事项

- **平台兼容性**：代码为 POSIX 编写，Windows/MinGW 下 `mkdir` 双参数与 `vasprintf` 无法编译，建议在 Linux/WSL 下使用。
- **宏污染**：小写宏 `debug/info/warn/error/fatal` 是全局宏，会替换所有同名标识符，注意与业务代码、第三方库的命名冲突。
- **`va_end` 缺失**：`Logger` 的日志方法在 `vasprintf` 失败提前返回时未调用 `va_end`，虽然影响极小，但严格属未定义行为。
- **异步日志析构**：`AsyncLooper` 析构时会 `join` 工作线程并排空缓冲区，但日志量极大时极端情况下可能仍有少量日志未能落盘。
- **滚动文件**：若单条日志超过滚动阈值，会生成一条内容超过阈值的独立日志文件，属可接受行为。

## 目录结构

```
Logs/
├── popolog.hpp     # 全局门面接口 + 宏代理
├── logger.hpp      # 日志器核心：Logger / 建造者 / 管理器
├── format.hpp      # 格式化模块
├── sink.hpp        # 落地模块（屏幕/文件/滚动文件）
├── looper.hpp      # 异步工作器（双缓冲）
├── buffer.hpp      # 异步缓冲区
├── message.hpp     # 日志消息结构
├── level.hpp       # 日志等级
├── util.hpp        # 工具类
├── test.cc         # 测试程序
└── makefile        # 构建脚本
```
