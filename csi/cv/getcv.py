import cv2
import numpy as np
import matplotlib.pyplot as plt

# 打开视频文件
cap = cv2.VideoCapture('F:\ygh20\code\csi\csi\wjj3_z3\wjj3_z3.mp4')
# 设置要跳过的帧数
frames_to_skip = 20

# 设置视频读取位置到指定帧
cap.set(cv2.CAP_PROP_POS_FRAMES, frames_to_skip)

# 获取视频属性
fps = cap.get(cv2.CAP_PROP_FPS)  # 帧速率
frame_count = int(cap.get(cv2.CAP_PROP_FRAME_COUNT))  # 帧数
frame_width = int(cap.get(cv2.CAP_PROP_FRAME_WIDTH))  # 帧宽度
frame_height = int(cap.get(cv2.CAP_PROP_FRAME_HEIGHT))  # 帧高度

# 打印视频属性
print("帧速率:", fps)
print("帧数:", frame_count)
print("帧宽度:", frame_width)
print("帧高度:", frame_height)

# 初始化图像数组
video_data = []

# 处理每一帧
while True:
    grabbed, frame = cap.read()
    if not grabbed:
        break
    
    gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)  # 转为灰度图
    _, binary = cv2.threshold(gray, 127, 255, cv2.THRESH_BINARY)  # 二值化处理
    
    # 将二值化图像展开成一维数组
    flat_binary = binary.flatten()
    video_data.append(flat_binary)

# 释放视频文件
cap.release()

# 将图像数据转换为NumPy数组
video_data = np.array(video_data)

# 显示视频数据的二维图像
plt.figure(figsize=(12, 6))
plt.imshow(video_data, cmap='gray', aspect='auto')
plt.colorbar(label='Pixel Value')
plt.xlabel('Pixel Position')
plt.ylabel('Frame Number')
plt.title('Video as 2D Image (Binary)')
plt.show()
