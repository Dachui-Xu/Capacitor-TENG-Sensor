from scipy.signal import firwin
import numpy as np

# 设置滤波器参数
cutoff_frequency = 1.6       # 截止频率为1.6 Hz
sample_rate = 500          # 采样频率为500 Hz
num_taps = 51              # 滤波器阶数，决定系数个数

# 使用 firwin 设计FIR滤波器
fir_coeffs = firwin(num_taps, cutoff_frequency / (0.5 * sample_rate), pass_zero=True)

# 打印系数并将其格式化为C语言数组形式
print("float fir_coeffs[FILTER_TAPS] = {{".format(num_taps))
print(", ".join("{:.8f}".format(coeff) for coeff in fir_coeffs))
print("};")


