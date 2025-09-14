import requests
import json

# 基础配置
BASE_URL = "http://localhost:8080"  # 根据实际服务地址修改
LOGIN_URL = f"{BASE_URL}/login"     # 登录接口
PERSON_URL = f"{BASE_URL}/api/fortune/person"   # 保存个人信息接口

def login_and_get_cookies(username, password):
    """登录并获取 Cookies"""
    session = requests.Session()
    login_data = {"username": username, "password": password}
    
    response = session.post(LOGIN_URL, json=login_data)
    print(f"登录响应状态码: {response.status_code}")
    
    if response.status_code == 200 and "登录成功" in response.text:
        print("✅ 登录成功")
        
        # 提取并格式化 Cookies
        cookies = response.cookies
        cookies_dict = requests.utils.dict_from_cookiejar(cookies)
        cookies_str = "; ".join([f"{k}={v}" for k, v in cookies_dict.items()])
        
        print(f"🍪 获取的 Cookies: {cookies_str}")
        return cookies_dict, cookies_str
    else:
        print(f"❌ 登录失败: {response.text}")
        return None, None

def save_person_with_cookies(cookies_dict, person_data):
    """使用 Cookies 保存个人信息"""
    # 创建新的独立请求
    headers = {
        "Content-Type": "application/json",
        "Cookie": "; ".join([f"{k}={v}" for k, v in cookies_dict.items()])
    }
    
    print(f"\n🔧 请求头: {json.dumps(headers, indent=2)}")
    print(f"📤 请求数据: {json.dumps(person_data, indent=2)}")
    
    response = requests.post(
        PERSON_URL,
        json=person_data,
        headers=headers
    )
    
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
    
    # 步骤1: 登录获取 cookies
    cookies_dict, cookies_str = login_and_get_cookies(USERNAME, PASSWORD)
    if not cookies_dict:
        exit(1)
    
    # 步骤2: 使用获取的 cookies 保存个人信息
    print("\n💾 尝试保存个人信息（使用独立连接）...")
    save_person_with_cookies(cookies_dict, person_data)
    
    # 步骤3: 验证保存结果（可选）
    # 这里可以添加数据库查询验证实际存储结果
