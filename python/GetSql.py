import requests
import json
import random
import datetime

from TestNewSQL import *




# 基础配置
BASE_URL = "http://localhost:8080"  # 根据您的实际服务地址修改

def get_user_data(user_id):
    """
    获取指定用户ID的所有Person和Fortune信息
    :param user_id: 要查询的用户ID
    :return: 包含Person和Fortune信息的字典
    """
    # 创建新会话
    session = requests.Session()
    
    # 1. 模拟登录（假设有用户信息）
    login_url = f"{BASE_URL}/login"
    login_data = {
        "username": f"concurrent_user_3",
        "password": "testpassword"
    }
    login_response = session.post(login_url, json=login_data)
    
    if login_response.status_code != 200:
        return {
            "error": f"登录失败: {login_response.status_code}",
            "response": login_response.text
        }
    
    # 2. 获取用户的所有Person信息
    persons_url = f"{BASE_URL}/api/fortune/persons"
    persons_response = session.get(persons_url)
    
    if persons_response.status_code != 200:
        return {
            "error": f"获取Person信息失败: {persons_response.status_code}",
            "response": persons_response.text
        }
    
    try:
        persons_data = persons_response.json()
    except:
        return {
            "error": "解析Person信息失败",
            "response": persons_response.text
        }
    
    # 3. 获取用户的所有Fortune结果
    fortunes_url = f"{BASE_URL}/api/fortune/results"
    fortunes_response = session.get(fortunes_url)
    
    if fortunes_response.status_code != 200:
        return {
            "error": f"获取Fortune信息失败: {fortunes_response.status_code}",
            "response": fortunes_response.text
        }
    
    try:
        fortunes_data = fortunes_response.json()
    except:
        return {
            "error": "解析Fortune信息失败",
            "response": fortunes_response.text
        }
    
    # 4. 组织返回数据
    # 创建person_id到fortune结果的映射
    fortune_map = {}
    for fortune in fortunes_data:
        person_id = fortune["personId"]
        # 确保每个person只保留最新的fortune结果
        if person_id not in fortune_map:
            fortune_map[person_id] = fortune
        else:
            # 比较创建时间，保留最新的
            existing_date = datetime.datetime.fromisoformat(fortune_map[person_id]["createdAt"].replace('Z', '+00:00'))
            new_date = datetime.datetime.fromisoformat(fortune["createdAt"].replace('Z', '+00:00'))
            if new_date > existing_date:
                fortune_map[person_id] = fortune
    
    # 为每个person添加对应的fortune结果
    for person in persons_data:
        person_id = person["id"]
        if person_id in fortune_map:
            person["fortune"] = fortune_map[person_id]
        else:
            person["fortune"] = None
    
    return {
        "user_id": user_id,
        "person_count": len(persons_data),
        "persons": persons_data
    }

def print_user_data(user_data):
    """打印用户数据"""
    print(f"\n用户ID: {user_data['user_id']}")
    print(f"人物数量: {user_data['person_count']}")
    print("=" * 50)
    
    for person in user_data["persons"]:
        print(f"\n人物ID: {person['id']}")
        print(f"姓名: {person['name']}")
        print(f"出生日期: {person['birthdate']}")
        print(f"出生时辰: {person['birthTime']}")
        print(f"性别: {'男' if person['gender'] == 'male' else '女'}")
        
        if person["fortune"]:
            fortune = person["fortune"]
            print("\n算命结果:")
            print(f"  ID: {fortune['id']}")
            print(f"  综合评分: {fortune['overallScore']}/100")
            print(f"  命运分析: {fortune['fateAnalysis'][:50]}...")
            print(f"  事业建议: {fortune['careerSuggestion'][:50]}...")
            print(f"  幸运数字: {', '.join(map(str, fortune['luckyNumbers']))}")
            print(f"  幸运颜色: {', '.join(fortune['luckyColors'])}")
            print(f"  创建时间: {fortune['createdAt']}")
        else:
            print("\n算命结果: 无")
        
        print("-" * 50)

def main():
    # 测试多个用户ID
    user_ids = [1]
    
    for user_id in user_ids:
        print(f"\n{'='*50}")
        print(f"开始查询用户 {user_id} 的数据")
        print(f"{'='*50}")
        
        user_data = get_user_data(user_id)
        
        if "error" in user_data:
            print(f"查询失败: {user_data['error']}")
            print(f"响应内容: {user_data.get('response', '无')}")
        else:
            print_user_data(user_data)
    
    # 测试不存在的用户
    print(f"\n{'='*50}")
    print(f"开始查询不存在的用户 (ID: 9999)")
    print(f"{'='*50}")
    user_data = get_user_data(9999)
    if "error" in user_data:
        print(f"预期失败: {user_data['error']}")
    else:
        print("意外成功获取数据")

if __name__ == "__main__":
    #test_login('1','1')
    test_save_person()
    test_get_persons()
