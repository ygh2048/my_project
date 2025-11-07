import cv2
import numpy as np
import matplotlib.pyplot as plt

# 打开视频文件
cap = cv2.VideoCapture('cv\wjj1_c1.mp4')
# 设置要跳过的帧数
frames_to_skip = 20

# 设置视频读取位置到指定帧
cap.set(cv2.CAP_PROP_POS_FRAMES, frames_to_skip)

# 初始化图像数组
video_data = []
# 获取视频属性
fps = cap.get(cv2.CAP_PROP_FPS)  # 帧速率
frame_count = int(cap.get(cv2.CAP_PROP_FRAME_COUNT))  # 帧数s
frame_width = int(cap.get(cv2.CAP_PROP_FRAME_WIDTH))  # 帧宽度
frame_height = int(cap.get(cv2.CAP_PROP_FRAME_HEIGHT))  # 帧高度

# 打印视频属性
print("帧速率:", fps)
print("帧数:", frame_count)
print("帧宽度:", frame_width)
print("帧高度:", frame_height)

# 创建窗口并调整大小
cv2.namedWindow('contours', cv2.WINDOW_NORMAL)
cv2.resizeWindow('contours', 800, 600)  # 设置窗口大小为800x600

cv2.namedWindow('dis', cv2.WINDOW_NORMAL)
cv2.resizeWindow('dis', 800, 600)  # 设置窗口大小为800x600

scale_factor = 0.3

# 计算新的尺寸
new_width = int(frame_width * scale_factor)
new_height = int(frame_height * scale_factor)

# 结构元素
es = cv2.getStructuringElement(cv2.MORPH_ELLIPSE, (9, 4))
background = None
# 设置高亮度的阈值
high_threshold = 200  # 可以根据需要调整


while True:
    grabbed, frame = cap.read()  # 读取视频帧
    if not grabbed:
        break  # 如果没有更多帧，则退出循环
    
    # 调整图像尺寸（使用 INTER_AREA 插值方法适合下采样）
    frame = cv2.resize(frame, (new_width, new_height), interpolation=cv2.INTER_AREA)


    gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY) #灰度
    _, mask = cv2.threshold(gray, high_threshold, 255, cv2.THRESH_BINARY)
    # 反转掩模，将原图像中高亮度区域变为黑色
    mask_inv = cv2.bitwise_not(mask)
    # 将掩模应用到原图像
    gray = cv2.bitwise_and(gray, gray, mask=mask_inv)


    gray = cv2.GaussianBlur(gray, (21, 21), 0)     #高斯

 # 将第一帧设置为整个输入的背景
    if background is None:
        background = gray
        #cv2.imshow('background',background)
        continue
    # 对于每个从背景之后读取的帧都会计算其与北京之间的差异，并得到一个差分图（different map）。
    # 还需要应用阈值来得到一幅黑白图像，并通过下面代码来膨胀（dilate）图像，从而对孔（hole）和缺陷（imperfection）进行归一化处理
    diff = cv2.absdiff(background, gray)
    diff = cv2.threshold(diff, 25, 255, cv2.THRESH_BINARY)[1]
    diff = cv2.dilate(diff, es, iterations=1) #膨胀

    # 该函数计算一幅图像中目标的轮廓
    contours, _ = cv2.findContours(diff.copy(), cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
    
    for c in contours:
        # 对于矩形区域，只显示大于给定阈值的轮廓，所以一些微小的变化不会显示。对于光照不变和噪声低的摄像头可不设定轮廓最小尺寸的阈值
        if cv2.contourArea(c) < 3000:
            continue
        (x, y, w, h) = cv2.boundingRect(c)# 该函数计算矩形的边界框
        cv2.rectangle(frame, (x, y), (x+w, y+h), (0, 255, 0), 2)

    # 显示结果
    cv2.imshow('contours', frame)
    cv2.imshow('dis', diff)

    # 将二值化图像展开成一维数组
    flat_binary = diff.flatten()
    video_data.append(flat_binary)


    # 退出条件
    key = cv2.waitKey(1) & 0xFF
    if key == ord('q'):
        break

# 释放视频文件并关闭窗口
cap.release()
cv2.destroyAllWindows()

# 将图像数据转换为NumPy数组
video_data = np.array(video_data)

# 计算时间轴
time_axis = np.arange(video_data.shape[0]) / fps  # 每一帧的时间（秒）

# 创建3D图像
fig = plt.figure(figsize=(15, 10))
ax = fig.add_subplot(111, projection='3d')

# X, Y, Z 数据
x = time_axis
y = np.arange(new_height * new_width)
X, Y = np.meshgrid(x, y)
#Z = video_data.T  # 转置，以便每列对应一个帧

Z = np.zeros_like(X)  # Z 轴从零开始

# 生成一个与 Z 数据形状相同的全黑颜色数组
black_color = np.full_like(Z, 'black', dtype=object)
black_color[Z == 0] = 'white'

# 绘制3D图像
#ax.plot_surface(X, Y, Z, facecolors=black_color, edgecolor='none')

# 设置标签
ax.set_xlabel('Time (seconds)')
ax.set_ylabel('Pixel Position (Flattened)')
ax.set_zlabel('Pixel Value')
ax.set_title('Video Data in 3D')

plt.show()

