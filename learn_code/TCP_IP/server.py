import socket

def start_server():
    # 创建 TCP/IP 套接字
    server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    
    # 获取本地主机名和端口
    host = '127.0.0.1'  # 本地回环地址
    port = 12345        # 使用大于1024的端口
    
    # 绑定套接字到地址和端口
    server_socket.bind((host, port))
    
    # 开始监听传入连接
    server_socket.listen(1)
    print(f"服务器启动，在 {host}:{port} 等待连接...")
    
    # 等待客户端连接
    client_socket, client_address = server_socket.accept()
    print(f"接收到来自 {client_address} 的连接")
    
    try:
        while True:
            # 接收客户端发送的数据
            data = client_socket.recv(1024).decode('utf-8')
            if not data:
                break
            print(f"收到客户端消息: {data}")
            
            # 发送响应回客户端
            response = f"服务器已收到你的消息: {data}"
            client_socket.send(response.encode('utf-8'))
            
    except Exception as e:
        print(f"发生错误: {e}")
    finally:
        # 关闭连接
        client_socket.close()
        server_socket.close()
        print("连接已关闭")

if __name__ == "__main__":
    start_server()