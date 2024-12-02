import numpy as np
import pandas as pd
import matplotlib.pyplot as plt

# 读取CSV文件，使用逗号作为分隔符
df = pd.read_csv('TENG.csv', delimiter=',', header=0)

# 打印数据的前几行，看看是否正确加载
print("Raw data loaded:")
print(df.head())

# 确保所有数据都是数值类型，使用pandas的to_numeric()来转换
df = df.apply(pd.to_numeric, errors='coerce')

# 打印转换后的数据，看看是否出现了NaN
print("Data after conversion to numeric:")
print(df.head())

# 检查是否有NaN值，并打印行数
nan_count = df.isna().sum().sum()
print(f"Number of NaN values in the data: {nan_count}")

# 如果有NaN值，可以选择删除这些行，或者使用插值方法填补
df = df.dropna()  # 删除任何含有NaN值的行

# 转换为numpy数组
data = df.to_numpy()

# 打印最终的有效数据
print("Cleaned data after dropping NaNs:")
print(data)

# 如果数据为空，跳过FFT
if data.size == 0:
    print("No valid data to plot!")
else:
    # 对每一行数据进行FFT
    N = data.shape[1]  # 每行数据的长度
    T = 1.0 / 50.0  # 假设采样频率为800Hz，可以根据实际情况调整
    x = np.linspace(0.0, N * T, N, endpoint=False)  # 时间轴

    # 画出每一行数据的FFT结果
    plt.figure(figsize=(10, 6))

    for i in range(data.shape[0]):
        # 计算FFT
        yf = np.fft.fft(data[i])

        # 频率轴
        xf = np.fft.fftfreq(N, T)[:N // 2]

        # 幅度归一化
        yf = 2.0 / N * np.abs(yf[:N // 2])

        # 打印一些调试信息，确保数据是有效的
        print(f"Row {i + 1} FFT max amplitude: {np.max(yf)}")
        print(f"Row {i + 1} Frequency range: {xf[0]} Hz to {xf[-1]} Hz")

        # 绘制频谱
        plt.plot(xf, yf, label=f"Row {i + 1}")

    # 添加标题和标签
    plt.title('FFT of each row')
    plt.xlabel('Frequency (Hz)')
    plt.ylabel('Amplitude')
    plt.grid(True)


    # 显示图像
    plt.show()
