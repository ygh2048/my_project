import requests
import json
import random
import datetime
import unittest

# 基础配置
BASE_URL = "http://localhost:8080"  # 根据您的实际服务地址修改
SESSION = requests.Session()  # 使用 Session 保持登录状态

def print_response(response, operation_name):
    """打印响应结果"""
    print(f"\n{operation_name} 响应状态码: {response.status_code}")
    try:
        print(f"响应内容: {response.json()}")
        return response.json()
    except:
        print(f"响应文本: {response.text}")
        return None

def test_register(username=None, email=None):
    """测试用户注册"""
    url = f"{BASE_URL}/regist"
    if not username:
        username = f"testuser{random.randint(1000, 9999)}"
    if not email:
        email = f"test{random.randint(1000, 9999)}@example.com"
    
    data = {
        "username": username,
        "password": "testpassword",
        "email": email
    }
    response = SESSION.post(url, json=data)
    result = print_response(response, "用户注册")
    return result, data["username"]

def test_login(username, password):
    """测试用户登录"""
    url = f"{BASE_URL}/login"
    data = {
        "username": username,
        "password": password
    }
    response = SESSION.post(url, json=data)
    result = print_response(response, "用户登录")
    return result

def test_save_person():
    """测试保存个人信息"""
    url = f"{BASE_URL}/api/fortune/person"
    
    # 生成随机出生日期（1900-2020之间）
    birth_year = random.randint(1900, 2020)
    birth_month = random.randint(1, 12)
    birth_day = random.randint(1, 28)
    
    data = {
        "name": random.choice(["张", "李", "王", "刘", "陈", "杨"]) + "测试",
        "birthdate": f"{birth_year}-{birth_month:02d}-{birth_day:02d}T00:00:00.000Z",
        "time": random.choice(["子时", "丑时", "寅时", "卯时", "辰时", "巳时", "午时", "未时", "申时", "酉时", "戌时", "亥时"]),
        "gender": random.choice(["male", "female"])
    }
    response = SESSION.post(url, json=data)
    result = print_response(response, "保存个人信息")
    return result, data

def test_save_fortune_result(person_id):
    """测试保存算命结果"""
    url = f"{BASE_URL}/api/fortune/result"
    
    # 生成随机算命结果
    lucky_numbers = [random.randint(1, 100) for _ in range(random.randint(3, 5))]
    lucky_colors = random.sample(["红色", "蓝色", "绿色", "金色", "紫色", "白色", "黑色"], random.randint(2, 4))
    
    data = {
        "personId": person_id,
        "fateAnalysis": f"{random.choice(['大吉', '中吉', '小吉'])}之相，{random.choice(['前途光明', '事业有成', '家庭和睦', '财运亨通'])}",
        "careerSuggestion": random.choice([
            "宜积极进取，把握机会"
        ]),
        "loveAdvice": random.choice([
            "珍惜眼前人，多沟通交流"
        ]),
        "healthWarning": random.choice([
            "注意饮食健康，多运动"
        ]),
        "luckyNumbers": lucky_numbers,
        "luckyColors": lucky_colors,
        "overallScore": random.randint(60, 100)
    }
    response = SESSION.post(url, json=data)
    result = print_response(response, "保存算命结果")
    return result

def test_get_persons():
    """测试获取用户所有个人信息"""
    url = f"{BASE_URL}/api/fortune/persons"
    response = SESSION.get(url)
    result = print_response(response, "获取用户所有个人信息")
    
    # 返回第一个人的ID用于保存算命结果
    if response.status_code == 200 and result and isinstance(result, list) and len(result) > 0:
        return result[0]["id"]
    return None

def test_get_fortune_results():
    """测试获取用户的算命记录"""
    url = f"{BASE_URL}/api/fortune/results"
    response = SESSION.get(url)
    result = print_response(response, "获取用户的算命记录")
    return result

def test_full_workflow():
    """完整测试流程"""
    print("\n" + "="*50)
    print("开始完整接口测试流程")
    print("="*50)
    
    # 1. 测试用户注册
    print("\n>>> 步骤1: 注册新用户")
    reg_result, username = test_register()
    if not reg_result or "注册成功" not in reg_result.get("message", ""):
        print("注册失败，无法继续测试")
        return
    
    # 2. 测试用户登录
    print("\n>>> 步骤2: 用户登录")
    login_result = test_login(username, "testpassword")
    if not login_result or "登录成功" not in login_result.get("message", ""):
        print("登录失败，无法继续测试")
        return
    
    # 3. 测试保存个人信息
    print("\n>>> 步骤3: 保存个人信息")
    save_person_result, person_data = test_save_person()
    if not save_person_result or "个人信息保存成功" not in save_person_result:
        print("保存个人信息失败")
        return
    
    # 4. 测试获取个人信息
    print("\n>>> 步骤4: 获取个人信息")
    person_id = test_get_persons()
    if not person_id:
        print("获取个人信息失败")
        return
    
    # 5. 测试保存算命结果
    print("\n>>> 步骤5: 保存算命结果")
    save_fortune_result = test_save_fortune_result(person_id)
    if not save_fortune_result or "算命结果保存成功" not in save_fortune_result:
        print("保存算命结果失败")
        return
    
    # 6. 再次保存算命结果（测试同一personId多个结果）
    print("\n>>> 步骤6: 再次保存算命结果（同一personId）")
    test_save_fortune_result(person_id)
    
    # 7. 测试获取算命记录
    print("\n>>> 步骤7: 获取算命记录")
    fortune_results = test_get_fortune_results()
    if not fortune_results or not isinstance(fortune_results, list):
        print("获取算命记录失败")
        return
    
    # 验证每个personId只有一个最新结果
    person_ids = set()
    for result in fortune_results:
        if result["personId"] in person_ids:
            print(f"错误: personId {result['personId']} 有多个结果")
        else:
            person_ids.add(result["personId"])
    
    print(f"获取到 {len(fortune_results)} 条算命记录，涉及 {len(person_ids)} 个不同人物")
    
    # 8. 添加第二个人物
    print("\n>>> 步骤8: 保存第二个人物信息")
    save_person_result, person_data2 = test_save_person()
    
    # 9. 获取第二个人物ID
    print("\n>>> 步骤9: 获取第二个人物ID")
    persons = SESSION.get(f"{BASE_URL}/api/fortune/persons").json()
    person_id2 = None
    for person in persons:
        if person["name"] == person_data2["name"]:
            person_id2 = person["id"]
            break
    
    if not person_id2:
        print("获取第二个人物ID失败")
        return
    
    # 10. 为第二个人物保存算命结果
    print("\n>>> 步骤10: 为第二个人物保存算命结果")
    test_save_fortune_result(person_id2)
    
    # 11. 再次获取算命记录
    print("\n>>> 步骤11: 再次获取算命记录")
    fortune_results2 = test_get_fortune_results()
    if not fortune_results2 or len(fortune_results2) != 2:
        print(f"预期2条记录，实际获取{len(fortune_results2)}条")
    
    print("\n" + "="*50)
    print("完整接口测试流程结束")
    print("="*50)

def test_error_cases():
    """测试错误情况"""
    print("\n" + "="*50)
    print("开始错误情况测试")
    print("="*50)
    
    # 1. 未登录访问需要登录的接口
    print("\n>>> 测试1: 未登录访问需要认证的接口")
    SESSION.cookies.clear()  # 清除会话
    response = SESSION.get(f"{BASE_URL}/api/fortune/persons")
    print_response(response, "未登录访问 /api/fortune/persons")
    
    # 2. 注册重复用户名
    print("\n>>> 测试2: 注册重复用户名")
    username = f"dupuser{random.randint(1000, 9999)}"
    test_register(username, "email1@test.com")
    test_register(username, "email2@test.com")  # 重复用户名
    
    # 3. 登录错误密码
    print("\n>>> 测试3: 登录错误密码")
    test_login(username, "wrongpassword")
    
    # 4. 保存个人信息 - 无效数据
    print("\n>>> 测试4: 保存个人信息 - 无效数据")
    url = f"{BASE_URL}/api/fortune/person"
    invalid_data = [
        {"name": "测试", "birthdate": "invalid-date", "time": "子时", "gender": "male"},  # 无效日期
        {"birthdate": "2000-01-01T00:00:00.000Z", "time": "子时", "gender": "male"},     # 缺少姓名
        {"name": "测试", "birthdate": "2000-01-01T00:00:00.000Z", "gender": "male"},     # 缺少时辰
        {"name": "测试", "birthdate": "2000-01-01T00:00:00.000Z", "time": "子时"},        # 缺少性别
        {"name": "测试", "birthdate": "2000-01-01T00:00:00.000Z", "time": "无效时辰", "gender": "male"}  # 无效时辰
    ]
    
    for i, data in enumerate(invalid_data):
        print(f"\n无效数据测试 {i+1}")
        response = SESSION.post(url, json=data)
        print_response(response, f"保存个人信息 - 无效数据 {i+1}")
    
    # 5. 保存算命结果 - 无效数据
    print("\n>>> 测试5: 保存算命结果 - 无效数据")
    url = f"{BASE_URL}/api/fortune/result"
    invalid_data = [
        {"personId": 999999, "fateAnalysis": "测试"},  # 无效personId
        {"fateAnalysis": "测试"},                      # 缺少personId
        {"personId": 1},                              # 缺少必要字段
        {"personId": 1, "overallScore": 150}          # 分数超出范围
    ]
    
    for i, data in enumerate(invalid_data):
        print(f"\n无效数据测试 {i+1}")
        response = SESSION.post(url, json=data)
        print_response(response, f"保存算命结果 - 无效数据 {i+1}")
    
    # 6. 获取不存在的算命记录
    print("\n>>> 测试6: 获取不存在的算命记录")
    response = SESSION.get(f"{BASE_URL}/api/fortune/results")
    results = print_response(response, "获取算命记录")
    if results and len(results) > 0:
        max_id = max(r["id"] for r in results)
        response = SESSION.get(f"{BASE_URL}/api/fortune/results/{max_id + 1000}")
        print_response(response, "获取不存在的算命记录")
    
    print("\n" + "="*50)
    print("错误情况测试结束")
    print("="*50)

if __name__ == "__main__":
    # 创建测试用户
    test_username = "testuser_" + datetime.datetime.now().strftime("%H%M%S")
    _, username = test_register(test_username)
    test_login(username, "testpassword")
    
    # 执行完整流程测试
    test_full_workflow()
    
    # 执行错误情况测试
    test_error_cases()
    
    # 执行并发测试
    print("\n" + "="*50)
    print("开始并发测试")
    print("="*50)
    
    # 创建多个用户并行操作
    import threading
    
    def user_workflow(user_num):
        """单个用户的测试流程"""
        print(f"\n>>> 开始用户 {user_num} 测试")
        username = f"concurrent_user_{user_num}"
        test_register(username)
        test_login(username, "testpassword")
        
        # 创建3个人物
        for i in range(3):
            test_save_person()
        
        # 获取人物并保存算命结果
        persons = SESSION.get(f"{BASE_URL}/api/fortune/persons").json()
        for person in persons:
            for j in range(2):  # 每个人物保存2个结果
                test_save_fortune_result(person["id"])
        
        # 获取算命记录
        results = test_get_fortune_results()
        print(f"用户 {user_num} 获取到 {len(results)} 条记录")
    
    # 创建5个线程模拟并发用户
    threads = []
    for i in range(1, 6):
        t = threading.Thread(target=user_workflow, args=(i,))
        threads.append(t)
        t.start()
    
    # 等待所有线程完成
    for t in threads:
        t.join()
    
    print("\n" + "="*50)
    print("并发测试结束")
    print("="*50)
    
    # 性能测试
    print("\n" + "="*50)
    print("开始性能测试")
    print("="*50)
    
    import time
    
    # 测试获取个人信息接口性能
    start_time = time.time()
    for _ in range(10):
        SESSION.get(f"{BASE_URL}/api/fortune/persons")
    duration = time.time() - start_time
    print(f"10次获取个人信息请求耗时: {duration:.2f}秒, 平均: {duration/10:.3f}秒/次")
    
    # 测试获取算命记录接口性能
    start_time = time.time()
    for _ in range(10):
        SESSION.get(f"{BASE_URL}/api/fortune/results")
    duration = time.time() - start_time
    print(f"10次获取算命记录请求耗时: {duration:.2f}秒, 平均: {duration/10:.3f}秒/次")
    
    print("\n" + "="*50)
    print("所有测试完成")
    print("="*50)
