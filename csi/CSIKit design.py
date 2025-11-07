import matplotlib.pyplot as plt
import pywt
#from gezhongmatlab import *
from matplotlib import font_manager

from CSIKit.filters.passband import lowpass
from CSIKit.filters.statistical import running_mean
from CSIKit.util.filters import hampel

from CSIKit.reader import get_reader
from CSIKit.tools.batch_graph import BatchGraph
from CSIKit.util import csitools


import numpy as np
root_name = "csi"

file_name="wjj3_z1"
my_reader = get_reader(f"{root_name}\{file_name}\{file_name}.pcap")
csi_data = my_reader.read_file(f"{root_name}\{file_name}\{file_name}.pcap", scaled=True)
csi_matrix, no_frames, no_subcarriers = csitools.get_CSI(csi_data, metric="amplitude")
# CSI matrix is now returned as (no_frames, no_subcarriers, no_rx_ant, no_tx_ant).
# First we'll select the first Rx/Tx antenna pairing.
print(no_frames)
csi_matrix_first = csi_matrix[:, :, 0, 0]
# Then we'll squeeze it to remove the singleton dimensions.
csi_matrix_squeezed = np.squeeze(csi_matrix_first)

# This example assumes CSI data is sampled at ~100Hz.
# In this example, we apply (sequentially):
#  - a lowpass filter to isolate frequencies below 10Hz (order = 5)
#  - a hampel filter to reduce high frequency noise (window size = 10, significance = 3)
#  - a running mean filter for smoothing (window size = 10)

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

fig = plt.figure()
plt.plot(csi_matrix_squeezed)
plt.savefig(f"{root_name}\{file_name}\{file_name}sin.png")
plt.show()

#BatchGraph.plot_heatmap(csi_matrix_squeezed, csi_data.timestamps)