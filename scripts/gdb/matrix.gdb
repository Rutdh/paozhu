# Advanced GDB Script for Matrix/Complex Data Debugging
# 用于调试复杂数据结构和矩阵运算的高级GDB脚本

# 打印矩阵的函数（假设是二维数组）
define print_matrix
    if $argc != 3
        printf "Usage: print_matrix <matrix_ptr> <rows> <cols>\n"
    else
        set $matrix = $arg0
        set $rows = $arg1
        set $cols = $arg2
        set $i = 0
        
        printf "Matrix %dx%d:\n", $rows, $cols
        printf "┌"
        set $j = 0
        while $j < $cols
            printf "%8s ", ""
            set $j = $j + 1
        end
        printf "┐\n"
        
        while $i < $rows
            printf "│"
            set $j = 0
            while $j < $cols
                printf "%8.2f ", *($matrix + $i * $cols + $j)
                set $j = $j + 1
            end
            printf "│\n"
            set $i = $i + 1
        end
        
        printf "└"
        set $j = 0
        while $j < $cols
            printf "%8s ", ""
            set $j = $j + 1
        end
        printf "┘\n"
    end
end

# 打印向量的函数
define print_vector
    if $argc != 2
        printf "Usage: print_vector <vector_ptr> <size>\n"
    else
        set $vector = $arg0
        set $size = $arg1
        set $i = 0
        
        printf "Vector[%d]: [", $size
        while $i < $size
            if $i > 0
                printf ", "
            end
            printf "%.2f", *($vector + $i)
            set $i = $i + 1
        end
        printf "]\n"
    end
end

# 打印STL容器（vector）
define print_std_vector
    if $argc != 1
        printf "Usage: print_std_vector <std_vector>\n"
    else
        set $vec = $arg0
        set $size = $vec._M_impl._M_finish - $vec._M_impl._M_start
        set $i = 0
        
        printf "std::vector size: %d\n", $size
        printf "["
        while $i < $size
            if $i > 0
                printf ", "
            end
            printf "%s", *($vec._M_impl._M_start + $i)
            set $i = $i + 1
        end
        printf "]\n"
    end
end

# 内存模式分析
define analyze_memory_pattern
    if $argc != 2
        printf "Usage: analyze_memory_pattern <start_addr> <size>\n"
    else
        set $addr = $arg0
        set $size = $arg1
        set $i = 0
        
        printf "Memory pattern analysis from %p, size %d:\n", $addr, $size
        printf "Offset   Value    Hex      Pattern\n"
        printf "--------------------------------\n"
        
        while $i < $size
            set $val = *((char*)$addr + $i)
            printf "%6d: %8d  0x%02x   ", $i, $val, $val & 0xff
            
            # 分析模式
            if $val == 0
                printf "ZERO"
            else
                if $val == 0xff
                    printf "MAX "
                else
                    if $val > 32 && $val < 127
                        printf "ASCII('%c')", $val
                    else
                        printf "DATA"
                    end
                end
            end
            printf "\n"
            set $i = $i + 1
        end
    end
end

# 性能计时器
define start_timer
    # 记录开始时间（简化版本，实际需要更复杂的实现）
    printf "Timer started at current instruction\n"
    set $timer_start = $_
end

define end_timer
    printf "Timer ended. Check instruction count or use external profiling\n"
end

# 条件断点助手
define break_on_condition
    if $argc != 2
        printf "Usage: break_on_condition <location> <condition>\n"
        printf "Example: break_on_condition main 'x > 100'\n"
    else
        break $arg0 if $arg1
        printf "Conditional breakpoint set at %s when %s\n", $arg0, $arg1
    end
end

# 数据结构完整性检查
define check_data_integrity
    printf "=== Data Integrity Check ===\n"
    
    # 检查栈
    printf "Stack pointer: %p\n", $sp
    printf "Frame pointer: %p\n", $fp
    
    # 检查堆（需要根据具体内存管理器调整）
    printf "Heap status: Use 'info heap' if available\n"
    
    # 检查全局变量区域
    printf "Program sections:\n"
    info files
    
    printf "=== Check Complete ===\n"
end

# 函数调用跟踪
define trace_calls
    if $argc != 1
        printf "Usage: trace_calls <function_name>\n"
    else
        printf "Setting up call tracing for %s\n", $arg0
        break $arg0
        commands
        silent
        printf "Called %s with args: ", $arg0
        # 这里可以添加参数打印逻辑
        printf "\n"
        continue
        end
    end
end

# 高级初始化
define matrix_startup
    printf "Advanced Matrix Debugging Environment Loaded\n"
    printf "Available commands:\n"
    printf "  print_matrix <ptr> <rows> <cols> - Print 2D matrix\n"
    printf "  print_vector <ptr> <size>        - Print 1D vector\n"
    printf "  print_std_vector <vector>        - Print STL vector\n"
    printf "  analyze_memory_pattern <addr> <size> - Analyze memory\n"
    printf "  break_on_condition <loc> <cond>  - Conditional breakpoint\n"
    printf "  check_data_integrity             - Check data integrity\n"
    printf "  trace_calls <function>           - Trace function calls\n"
    printf "  start_timer / end_timer          - Simple timing\n"
    
    # 设置高级选项
    set print array on
    set print pretty on
    set print union on
    set print elements 50
end

# 自动启动高级环境
matrix_startup