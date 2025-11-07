#!/bin/bash

# 检查是否提供了文件名参数
if [ $# -eq 0 ]; then
    echo "用法: $0 <文件夹名称>"
    exit 1
fi

# 获取文件夹名称
dirname=$1
echo "目录名称是 $dirname"

# 判断目录是否存在，如果不存在则创建，存在则输出“dir exist”
if [ ! -d "$dirname" ]; then
    mkdir "$dirname"
    echo "目录已创建"
else
    echo "目录已存在"
fi

# 设置保存路径
save_path="$dirname"

# 获取文件名参数
VIDEO_FILE="${save_path}/$1.h264"
DUMP_FILE="${save_path}/$1.pcap"

echo "开始录制"

# 开始录制视频的后台进程
ffmpeg -f video4linux -i /dev/video0 "$VIDEO_FILE" &
#raspivid -o "$VIDEO_FILE" -t 0 &
VIDEO_PID=$!

# 开始捕获数据包的后台进程
tcpdump -i wlan0 dst port 5500 -vv -w "$DUMP_FILE" -c 60 &
TCPDUMP_PID=$!

# 等待 tcpdump 命令完成
wait $TCPDUMP_PID

# tcpdump 完成后停止视频录制
kill $VIDEO_PID

# 确保视频录制完成
wait $VIDEO_PID

# 可选：将视频转换为 MP4 格式
MP4_FILE="${save_path}/$1.mp4"
MP4Box -add "$VIDEO_FILE" "$MP4_FILE"

# 删除原始 H264 文件
rm "$VIDEO_FILE"

echo "数据包捕获完成，保存为 $DUMP_FILE"
echo "视频录制完成，保存为 $MP4_FILE"