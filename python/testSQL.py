import requests
import json

# 基础配置
BASE_URL = "http://localhost:8080"  # 根据实际服务地址修改
LOGIN_URL = f"{BASE_URL}/login"     # 登录接口（需要根据实际项目调整）
PERSON_URL = f"{BASE_URL}/api/fortune/person"   # 保存个人信息接口

# 1. 先登录获取 session（模拟用户登录）
def login(username, password):
    """模拟登录获取 session"""
    session = requests.Session()
    # 根据实际登录接口调整参数格式
    login_data = {"username": username, "password": password}


    
    response = session.post(LOGIN_URL, json=login_data)
    print(response)
    if response.status_code == 200 and "登录成功" in response.text:
        print("✅ 登录成功，Session 已建立")
        return session
    else:
        print(f"❌ 登录失败: {response.text}")
        return None

# 2. 保存个人信息
def save_person(session, person_data):
    """保存个人信息"""
    response = session.post(PERSON_URL, json=person_data)
    
    print(response)
    if response.status_code == 200:
        print(f"📝 保存结果: {response.text}")
    else:
        print(f"🚨 请求失败: 状态码 {response.status_code}, 响应: {response.text}")

# 测试执行
if __name__ == "__main__":
    # 替换为实际测试账号
    USERNAME = "1"
    PASSWORD = "1"
    
    # 创建测试用的 PersonDTO 数据
    person_data = {
        "name": "张三",
        "birthdate": "1990-01-01",
        "time": "子时",    # 对应 birthTime
        "gender": "男"
    }
    
    # 执行测试流程
    print("=== 开始测试个人信息保存 ===")
    
    # 步骤1: 登录获取 session
    session = login(USERNAME, PASSWORD)
    if not session:
        exit(1)
    
    # 步骤2: 保存个人信息
    print("\n💾 尝试保存个人信息...")
    save_person(session, person_data)
    
    # 步骤3: 验证保存结果（可选）
    # 这里可以添加数据库查询验证实际存储结果
