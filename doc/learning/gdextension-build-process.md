# GDExtension 构建过程学习笔记

## 概述

本文档记录了 GDExtension C++ 扩展的构建过程，特别是理解构建系统如何工作以及最终产物是什么。

## 构建流程

### 1. 构建入口

使用 `scripts/build/build.sh` 脚本进行构建：

```bash
./scripts/build/build.sh web
```

### 2. 构建步骤

构建过程分为两个主要阶段：

#### 阶段 1：构建 godot-cpp 静态库

当执行 `env = SConscript("godot-cpp/SConstruct")` 时：

1. **加载 godot-cpp 的构建系统**
   - 读取 `godot-cpp/SConstruct`
   - 调用 `env.GodotCPP()` 函数

2. **编译 godot-cpp 源代码**
   - 编译 `godot-cpp/src/` 下的所有 C++ 文件
   - 生成绑定代码（bindings）
   - 创建静态库：`godot-cpp/bin/libgodot-cpp.{platform}.{target}.{arch}.a`

3. **将库添加到链接列表**
   ```python
   env.AppendUnique(LIBS=[env.File("bin/%s" % library_name)])
   ```
   这会将 godot-cpp 静态库添加到 `env["LIBS"]` 中

#### 阶段 2：构建你的 GDExtension 共享库

在你的 `SConstruct` 中：

1. **收集源文件**
   ```python
   sources = Glob("src/*.cpp")
   ```
   如果 `src/` 目录不存在或为空，`sources` 将是空列表

2. **创建共享库**
   ```python
   library = env.SharedLibrary(
       "bomberman/gdextension/libbomberman{}{}".format(env["suffix"], env["SHLIBSUFFIX"]),
       source=sources,
   )
   ```

3. **链接过程**
   - 即使 `sources` 为空，SCons 仍然会：
     - 链接 `env["LIBS"]` 中的库（即 godot-cpp 静态库）
     - 生成共享库文件

## 重要发现

### 为什么没有 src/ 目录也能生成 wasm 文件？

**答案：** 因为构建过程会链接 godot-cpp 静态库。

即使你的 `sources = Glob("src/*.cpp")` 返回空列表，`SharedLibrary` 仍然会：

1. **链接 godot-cpp 静态库**
   - godot-cpp 的代码已经被编译成静态库
   - 通过 `env["LIBS"]` 自动链接

2. **生成共享库文件**
   - 即使没有你的源代码，也会创建一个共享库
   - 这个库只包含 godot-cpp 的基础代码

### 生成的库文件内容

**如果 `src/` 目录为空或不存在：**

生成的 wasm 文件只包含：
- ✅ godot-cpp 的绑定代码
- ✅ godot-cpp 的工具函数
- ❌ **没有你的 GDExtension 实现代码**

这是一个"空壳"库，可以链接成功，但没有实际功能。

**如果 `src/` 目录有你的代码：**

生成的 wasm 文件包含：
- ✅ godot-cpp 的绑定代码
- ✅ godot-cpp 的工具函数
- ✅ **你的 GDExtension 实现代码**（register_types.cpp, 你的类等）

## 最终产物

### 文件位置

构建完成后，产物位于：

```
bomberman/gdextension/
├── libbomberman.web.template_debug.wasm32.wasm
└── libbomberman.web.template_release.wasm32.wasm
```

### 文件命名规则

格式：`libbomberman{env["suffix"]}{env["SHLIBSUFFIX"]}`（本项目库名为 `libbomberman`，在 SConstruct 中由 `GDEXTENSION_LIB_NAME` 定义）

其中：
- `env["suffix"]` = `.web.template_debug.wasm32`
- `env["SHLIBSUFFIX"]` = `.wasm` (Web 平台)

完整名称：`libbomberman.web.template_debug.wasm32.wasm`

### 不同平台的产物

| 平台      | 文件扩展名   | 示例                                             |
| --------- | ------------ | ------------------------------------------------ |
| Web       | `.wasm`      | `libbomberman.web.template_debug.wasm32.wasm`    |
| Windows   | `.dll`       | `libbomberman.windows.template_debug.x86_64.dll` |
| Linux/BSD | `.so`        | `libbomberman.linux.template_debug.x86_64.so`    |
| macOS     | `.framework` | `libbomberman.macos.template_debug.framework/`   |
| Android   | `.so`        | `libbomberman.android.template_debug.arm64.so`   |

## 如何添加你的代码

### 步骤 1：创建源文件目录

```bash
mkdir -p src
```

### 步骤 2：添加必要的源文件

至少需要：

1. **`src/register_types.cpp`** - 注册你的类
   ```cpp
   #include "register_types.h"
   #include <gdextension_interface.h>
   #include <godot_cpp/core/class_db.hpp>
   #include <godot_cpp/core/defs.hpp>
   #include <godot_cpp/godot.hpp>

   using namespace godot;

   void initialize_example_module(ModuleInitializationLevel p_level) {
       if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
           return;
       }
       // 注册你的类
       ClassDB::register_class<YourClass>();
   }

   void uninitialize_example_module(ModuleInitializationLevel p_level) {
       if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
           return;
       }
   }

   extern "C" {
   GDExtensionBool GDE_EXPORT example_library_init(
       GDExtensionInterfaceGetProcAddress p_get_proc_address,
       const GDExtensionClassLibraryPtr p_library,
       GDExtensionInitialization *r_initialization
   ) {
       godot::GDExtensionBinding::InitObject init_obj(
           p_get_proc_address, p_library, r_initialization
       );
       init_obj.register_initializer(initialize_example_module);
       init_obj.register_terminator(uninitialize_example_module);
       init_obj.set_minimum_library_initialization_level(
           MODULE_INITIALIZATION_LEVEL_SCENE
       );
       return init_obj.init();
   }
   }
   ```

2. **`src/register_types.h`** - 头文件
   ```cpp
   #ifndef REGISTER_TYPES_H
   #define REGISTER_TYPES_H

   void initialize_example_module(godot::ModuleInitializationLevel p_level);
   void uninitialize_example_module(godot::ModuleInitializationLevel p_level);

   #endif // REGISTER_TYPES_H
   ```

3. **你的类文件** - 例如 `src/your_class.cpp` 和 `src/your_class.h`

### 步骤 3：重新构建

```bash
./scripts/build/build.sh web
```

现在生成的 wasm 文件会包含你的代码。

## 构建系统架构

### 依赖关系

```
你的 SConstruct
    ↓
godot-cpp/SConstruct
    ↓
构建 godot-cpp 静态库
    ↓
添加到 env["LIBS"]
    ↓
你的 SharedLibrary 链接 godot-cpp
    ↓
生成最终的共享库文件
```

### 关键代码位置

1. **godot-cpp/tools/godotcpp.py:582**
   ```python
   env.AppendUnique(LIBS=[env.File("bin/%s" % library_name)])
   ```
   这里将 godot-cpp 库添加到链接列表

2. **你的 SConstruct:18**
   ```python
   sources = Glob("src/*.cpp")
   ```
   收集你的源代码

3. **你的 SConstruct:33-35**
   ```python
   library = env.SharedLibrary(
       "{}/gdextension/{}{}{}".format(GODOT_PROJECT_NAME, GDEXTENSION_LIB_NAME, env["suffix"], env["SHLIBSUFFIX"]),
       source=sources,
   )
   ```
   创建共享库，自动链接 env["LIBS"] 中的库

## 总结

1. **godot-cpp 是静态库**：先编译成 `.a` 文件，然后链接到你的共享库中
2. **即使没有源代码也能生成库**：因为会链接 godot-cpp 的基础代码
3. **要添加功能需要源代码**：在 `src/` 目录下添加你的 `.cpp` 文件
4. **最终产物是共享库**：`.wasm`、`.dll`、`.so` 或 `.framework` 格式

## 项目阶段实现记录

项目各阶段「做了哪些事」与本地验证步骤，放在 **project** 目录下，不混入本学习笔记：

- **[Phase 1、Phase 2 实现记录](../project/phase1-phase2.md)**：阶段 1、2 的已实现内容与本地验证汇总（仅 Web）。

## 参考

- [Godot GDExtension C++ Example](https://docs.godotengine.org/en/stable/tutorials/scripting/gdextension/gdextension_cpp_example.html)
- [Godot Build System Documentation](https://docs.godotengine.org/zh-cn/4.5/engine_details/development/compiling/introduction_to_the_buildsystem.html)
