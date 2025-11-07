from CSIKit.filters.passband import lowpass
from CSIKit.filters.statistical import running_mean
from CSIKit.util.filters import hampel
from CSIKit.reader import get_reader
from CSIKit.tools.batch_graph import BatchGraph
from CSIKit.util import csitools
import numpy as np
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D
import codecs
import os
import pywt

def plot_heatmap_save(csi_matrix, timestamps,str):

    csi_matrix = np.transpose(csi_matrix)

    x_label = "Time (s)"
    try:
        x = timestamps
        x = [timestamp - x[0] for timestamp in x]
    except AttributeError as e:
        # No timestamp in frame. Likely an IWL entry.
        # Will be moving timestamps to CSIData to account for this.
        x = [0]

    if sum(x) == 0:
            # Some files have invalid timestamp_low values which means we can't plot based on timestamps.
            # Instead we'll just plot by frame count.

        xlim = csi_matrix.shape[1]

        x_label = "Frame No."
    else:
        xlim = max(x)   

    limits = [0, xlim, 1, csi_matrix.shape[0]]

    _, ax = plt.subplots()
    im = ax.imshow(csi_matrix, cmap="jet", extent=limits, aspect="auto")

    cbar = ax.figure.colorbar(im, ax=ax)
    cbar.ax.set_ylabel("Amplitude (dBm)")

    plt.xlabel(x_label)
    plt.ylabel("Subcarrier Index")

    plt.title("CSI Amplitude Heatmap Plot")

    plt.savefig(str)

def plot_3d_surface(csi_matrix, timestamps, file_name):
    csi_matrix = np.transpose(csi_matrix)

    try:
        x = [timestamp - timestamps[0] for timestamp in timestamps]
    except AttributeError:
        x = [0] * len(csi_matrix[0])  # Use frame count if timestamps are missing

    if len(set(x)) <= 1:
        x = np.arange(len(csi_matrix[0]))  # If all timestamps are the same, use frame count

    X, Y = np.meshgrid(x, np.arange(csi_matrix.shape[0]))
    Z = csi_matrix

    fig = plt.figure()
    ax = fig.add_subplot(111, projection='3d')
    ax.plot_surface(X, Y, Z, cmap='viridis')

    ax.set_xlabel('Time (s)' if len(set(x)) > 1 else 'Frame No.')
    ax.set_ylabel('Subcarrier')
    ax.set_zlabel('CSI Value')

    plt.savefig(file_name)
    plt.show()


root_name = "csi"

file_name="wjj1_z1"
my_reader = get_reader(f"{root_name}\{file_name}\{file_name}.pcap")
csi_data = my_reader.read_file(f"{root_name}\{file_name}\{file_name}.pcap", scaled=True)
csi_matrix, no_frames, no_subcarriers = csitools.get_CSI(csi_data, metric="amplitude")
# CSI matrix is now returned as (no_frames, no_subcarriers, no_rx_ant, no_tx_ant).
# First we'll select the first Rx/Tx antenna pairing.
csi_matrix_first = csi_matrix[:, :, 0, 0]
# Then we'll squeeze it to remove the singleton dimensions.
csi_matrix_squeezed = np.squeeze(csi_matrix_first)
# This example assumes CSI data is sampled at ~100Hz.
# In this example, we apply (sequentially):
#  - a lowpass filter to isolate frequencies below 10Hz (order = 5)
#  - a hampel filter to reduce high frequency noise (window size = 10, significance = 3)
#  - a running mean filter for smoothing (window size = 10)
#for x in range(no_frames):
  #csi_matrix_squeezed[x] = lowpass(csi_matrix_squeezed[x], 10, 500, 5)#低通
#  csi_matrix_squeezed[x] = hampel(csi_matrix_squeezed[x], 10, 3)
#  csi_matrix_squeezed[x] = running_mean(csi_matrix_squeezed[x], 10)


# 创建单一目录
#dir_name = file_name
#if not os.path.exists(dir_name):
#    os.mkdir(dir_name)
#    print(f"目录 '{dir_name}' 已创建")
#else:
#    print(f"目录 '{dir_name}' 已存在")


#写入txt文件
f=codecs.open(f"{root_name}\{file_name}\{file_name}.txt",'w','utf-8')
f.write('csi_matrix_squeezed:\r\n')
for i in csi_matrix_squeezed:
  f.write(str(i)+'\r\n')
f.write('csi_data.timestamps:\r\n')
for i in csi_data.timestamps:
  f.write(str(i)+'\r\n')
f.close()

#BatchGraph.plot_heatmap(csi_matrix_squeezed, csi_data.timestamps)



for x in range(no_frames):
  #si_matrix_squeezed[x] = lowpass(csi_matrix_squeezed[x], 10, 30, 5)
  csi_matrix_squeezed[x] = hampel(csi_matrix_squeezed[x], 5, 2)
  csi_matrix_squeezed[x] = running_mean(csi_matrix_squeezed[x], 10)
  wavelet_basis = 'db8'  # 此处选择db8小波基
  w = pywt.Wavelet(wavelet_basis) 
  maxlevel = pywt.dwt_max_level(len(csi_matrix_squeezed[x]), w.dec_len) # 根据数据长度计算分解层数
  threshold = 0.1  # threshold for filtering
  coeffs = pywt.wavedec(csi_matrix_squeezed[x], wavelet_basis, level=maxlevel) 
  for i in range(1, len(coeffs)):
        coeffs[i] = pywt.threshold(coeffs[i], threshold*max(coeffs[i]))   
  signalrec= pywt.waverec(coeffs, wavelet_basis)
  csi_matrix_squeezed[x] =signalrec



plot_heatmap_save(csi_matrix_squeezed, csi_data.timestamps,f"{root_name}\{file_name}\{file_name}.png")



fig = plt.figure()
plt.plot(csi_matrix_squeezed)
plt.savefig(f"{root_name}\{file_name}\{file_name}sin.png")

print('no_subcarriers:')#子载波数
print(no_subcarriers)
print('no_frames:')#帧数
print(no_frames)
print(np.shape(csi_matrix_squeezed))



timestamps = csi_data.timestamps



plot_3d_surface(csi_matrix_squeezed,timestamps,f"{root_name}\{file_name}\{file_name}3d.png")

