# GDB脚本语言快速入门指南

## 1. GDB脚本语言概述

GDB脚本语言是GNU调试器的内置脚本语言，用于自动化调试过程。它结合了：
- GDB的调试命令
- 简单的编程结构（变量、条件、循环）
- 用户自定义函数

## 2. 基本语法

### 2.1 变量
```gdb
# 定义变量（使用$前缀）
set $myvar = 10
set $ptr = &variable_name
set $result = $myvar + 5

# 打印变量
print $myvar
printf "Value: %d\n", $myvar
```

### 2.2 条件语句
```gdb
if $myvar > 10
    printf "Greater than 10\n"
else
    printf "Less than or equal to 10\n"
end
```

### 2.3 循环
```gdb
# while循环
set $i = 0
while $i < 10
    printf "Count: %d\n", $i
    set $i = $i + 1
end
```

### 2.4 函数定义
```gdb
define my_function
    if $argc != 2
        printf "Usage: my_function <arg1> <arg2>\n"
    else
        set $param1 = $arg0
        set $param2 = $arg1
        printf "Processing %d and %d\n", $param1, $param2
    end
end
```

## 3. 常用GDB命令

### 3.1 断点管理
```gdb
break main                    # 在main函数设置断点
break file.cpp:123           # 在特定行设置断点
break function_name          # 在函数设置断点
break *0x400000             # 在内存地址设置断点
info breakpoints            # 查看所有断点
delete 1                    # 删除断点1
clear                       # 删除当前行的断点
```

### 3.2 程序执行
```gdb
run                         # 启动程序
continue                    # 继续执行
step                        # 单步执行（进入函数）
next                        # 单步执行（不进入函数）
finish                      # 执行到当前函数返回
until                       # 执行到指定行
```

### 3.3 数据检查
```gdb
print variable              # 打印变量
print *pointer             # 打印指针指向的值
print array[0]@10          # 打印数组的10个元素
x/10x $sp                  # 以16进制查看栈内容
info locals                # 查看局部变量
info args                  # 查看函数参数
backtrace                  # 查看调用栈
```

## 4. 脚本文件使用

### 4.1 执行脚本
```bash
# 启动时加载脚本
gdb -x hello_world.gdb ./paozhu

# 在GDB中加载脚本
(gdb) source hello_world.gdb

# 执行特定函数
(gdb) my_function 10 20
```

### 4.2 命令别名
```gdb
# 创建别名
define pp
    print $arg0
end

# 使用别名
pp my_variable
```

## 5. 高级功能

### 5.1 条件断点
```gdb
break main if argc > 1
break function_name if variable == 100
```

### 5.2 断点命令
```gdb
break main
commands
    printf "Entered main with %d arguments\n", argc
    continue
end
```

### 5.3 监视点
```gdb
watch variable              # 变量被写入时停止
rwatch variable            # 变量被读取时停止
awatch variable            # 变量被访问时停止
```

### 5.4 捕获异常
```gdb
catch throw                # 捕获C++异常抛出
catch catch               # 捕获C++异常捕获
catch syscall             # 捕获系统调用
```

## 6. 实际项目应用示例

### 6.1 调试paozhu项目
```gdb
# 加载项目特定脚本
source hello_world.gdb

# 设置项目相关断点
set_common_breakpoints

# 启动程序
run --config conf/server.conf

# 检查程序状态
print_status
```

### 6.2 网络调试
```gdb
# 设置网络相关断点
break accept
break recv
break send

# 检查套接字状态
print_socket_info
```

## 7. 调试技巧

### 7.1 多线程调试
```gdb
info threads               # 查看所有线程
thread 2                   # 切换到线程2
set scheduler-locking on   # 锁定当前线程
```

### 7.2 内存调试
```gdb
# 检查内存泄漏
info heap
info proc mappings

# 内存模式分析
analyze_memory_pattern $addr 100
```

### 7.3 性能分析
```gdb
# 函数调用计数
break function_name
commands
    silent
    set $call_count = $call_count + 1
    continue
end
```

## 8. 最佳实践

1. **模块化脚本**：将不同功能的函数分组到不同文件
2. **错误处理**：检查参数数量和有效性
3. **清晰输出**：使用格式化的输出信息
4. **条件调试**：使用条件断点减少干扰
5. **自动化**：创建启动脚本自动设置调试环境

## 9. 常见问题解决

### 9.1 符号信息缺失
```bash
# 编译时添加调试信息
gcc -g -O0 source.cpp

# 检查符号
(gdb) info symbols
```

### 9.2 优化代码调试
```gdb
# 设置寄存器变量可见
set print frame-arguments all
```

## 10. 学习资源

- GDB官方文档：https://www.gnu.org/software/gdb/documentation/
- GDB命令参考：`help` 命令在GDB中
- 在线教程和示例

通过这些基础知识和示例脚本，您可以快速上手GDB脚本编程，提高调试效率。
