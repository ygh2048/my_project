import socket

def start_client():
    # 创建 TCP/IP 套接字
    client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    
    # 服务器地址和端口
    host = '127.0.0.1'  # 本地回环地址
    port = 12345
    
    try:
        # 连接到服务器
        client_socket.connect((host, port))
        print(f"已连接到服务器 {host}:{port}")
        
        while True:
            # 获取用户输入
            message = input("请输入要发送的消息 (输入 'quit' 退出): ")
            
            if message.lower() == 'quit':
                break
            
            # 发送消息到服务器
            client_socket.send(message.encode('utf-8'))
            
            # 接收服务器响应
            response = client_socket.recv(1024).decode('utf-8')
            print(f"服务器响应: {response}")
            
    except Exception as e:
        print(f"发生错误: {e}")
    finally:
        # 关闭连接
        client_socket.close()
        print("连接已关闭")

if __name__ == "__main__":
    start_client()