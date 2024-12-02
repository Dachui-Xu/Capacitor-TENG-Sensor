import numpy as np
import matplotlib.pyplot as plt
import pandas as pd

# 1. 读取 CSV 数据
data = pd.read_csv('data.csv')  # 读取CSV文件，假设数据在第一列
signal = data.iloc[:, 0].values  # 假设数据在第一列

# 2. 获取采样率和采样间隔
sampling_rate = 50  # 采样频率为50 Hz
T = 1.0 / sampling_rate  # 采样间隔

# 3. 进行 FFT
N = len(signal)  # 信号的长度
frequencies = np.fft.fftfreq(N, T)  # 计算频率
fft_values = np.fft.fft(signal)  # 执行FFT
fft_magnitude = np.abs(fft_values)  # 取FFT的幅度

# 4. 只保留正频率部分
positive_frequencies = frequencies[:N // 2]
positive_fft_magnitude = fft_magnitude[:N // 2]

# 5. 绘制频谱图
plt.figure(figsize=(10, 6))
plt.plot(positive_frequencies, positive_fft_magnitude)
plt.title('Frequency Spectrum')
plt.xlabel('Frequency (Hz)')
plt.ylabel('Magnitude')
plt.grid(True)
plt.show()
