# GDB Script for paozhu debugging
# 这是一个用于调试paozhu项目的GDB脚本示例

# 设置断点的函数
define set_common_breakpoints
    echo Setting common breakpoints...\n
    
    # 在main函数设置断点
    break main
    
    # 在HTTP处理相关函数设置断点（根据项目结构推测）
    # break httphook.cpp:somefunction
    
    echo Breakpoints set!\n
end

# 打印程序状态的函数
define print_status
    echo ===== Program Status =====\n
    
    # 打印当前栈帧
    backtrace 5
    
    # 打印局部变量
    info locals
    
    # 打印寄存器状态
    info registers
    
    echo ==========================\n
end

# 智能继续执行的函数
define smart_continue
    printf "Current location: "
    where
    
    printf "Continue? (y/n): "
    # 在实际使用中，可以添加交互逻辑
    continue
end

# 内存泄漏检查辅助函数
define check_memory
    echo Checking memory usage...\n
    
    # 打印堆信息（需要相应的符号）
    # info heap
    
    # 打印内存映射
    info proc mappings
end

# 调试网络连接的函数
define debug_network
    echo Network debugging info...\n
    
    # 打印套接字相关信息
    # 这里需要根据具体的网络代码添加相应的打印语句
    printf "Add network-specific debugging here\n"
end

# 启动脚本 - 当脚本加载时自动执行
define startup
    echo Loading paozhu debugging environment...\n
    
    # 设置基本选项
    set pagination off
    set print pretty on
    set print elements 0
    set print repeats 0
    
    # 设置断点
    set_common_breakpoints
    
    echo GDB environment ready!\n
    echo Available commands:\n
    echo   set_common_breakpoints - Set common debugging breakpoints\n
    echo   print_status          - Print current program status\n
    echo   smart_continue        - Intelligent continue with status\n
    echo   check_memory          - Check memory usage\n
    echo   debug_network         - Network debugging utilities\n
    echo Type 'run' to start the program\n
end

# 自动启动
startup