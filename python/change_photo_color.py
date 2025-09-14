from PIL import Image

def change_red_to_blue(input_path, output_path, red_threshold=200):
    img = Image.open(input_path).convert("RGBA")
    datas = img.getdata()
    new_data = []
    for item in datas:
        r, g, b, a = item
        # 判断是否为红色（可根据实际情况调整阈值）
        if r > red_threshold and g < 100 and b < 100:
            # 替换为蓝色
            new_data.append((0, 0, 255, a))
        else:
            new_data.append(item)
    img.putdata(new_data)
    img.save(output_path)

if __name__ == "__main__":
    input_file = "chat0.png"    # 输入图片路径
    output_file = "output.png"  # 输出图片路径
    change_red_to_blue(input_file, output_file)