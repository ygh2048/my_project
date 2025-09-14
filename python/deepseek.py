import requests
import json
from datetime import datetime
from dataclasses import dataclass
from typing import List, Dict, Any

# 定义数据类
@dataclass
class Person:
    name: str
    birthdate: datetime
    time: str
    gender: str

@dataclass
class FortuneResult:
    fateAnalysis: str
    careerSuggestion: str
    loveAdvice: str
    healthWarning: str
    luckyNumbers: List[int]
    luckyColors: List[str]
    overallScore: int

class FortuneService:
    FORTUNE_API_URL = "https://api.deepseek.com/v1/chat/completions"
    NETWORK_TIMEOUT = 15
    MODEL_NAME = "deepseek-chat"
    API_KEY = "sk-221bcc39bf464acaa60f2c21642f0a74"

    @staticmethod
    def format_date(date: datetime) -> str:
        return date.strftime("%Y-%m-%d")

    @classmethod
    def get_fortune(cls, person: Person) -> FortuneResult:
        try:
            messages = [
                {
                    "role": "system",
                    "content": """你是一个专业的算命师，根据用户的个人信息进行运势分析。
                    请严格按照以下 JSON 格式返回结果，不要包含任何额外信息：
                    {
                        "fateAnalysis": "命运分析",
                        "careerSuggestion": "事业建议",
                        "loveAdvice": "爱情建议",
                        "healthWarning": "健康提醒",
                        "luckyNumbers": [幸运数字],
                        "luckyColors": [幸运颜色],
                        "overallScore": 综合评分
                    }"""
                },
                {
                    "role": "user",
                    "content": f"姓名：{person.name}\n出生日期：{cls.format_date(person.birthdate)}\n时间：{person.time}\n性别：{person.gender}"
                }
            ]

            headers = {
                "Content-Type": "application/json",
                "Authorization": f"Bearer {cls.API_KEY}"
            }

            data = {
                "model": cls.MODEL_NAME,
                "messages": messages,
                "temperature": 0.7,
                "max_tokens": 2048,
                "response_format": {"type": "json"}
            }

            response = requests.post(
                cls.FORTUNE_API_URL,
                headers=headers,
                json=data,
                timeout=cls.NETWORK_TIMEOUT
            )

            if response.status_code == 200:
                result = response.json()
                content = result["choices"][0]["message"]["content"]
                return FortuneResult(**json.loads(content))
            else:
                raise Exception(f"API错误: {response.status_code}")

        except Exception as e:
            raise Exception(f"网络请求失败: {str(e)}")

# 测试代码
if __name__ == "__main__":
    # 创建测试用例
    test_person = Person(
        name="张三",
        birthdate=datetime(1990, 1, 1),
        time="12:00",
        gender="男"
    )

    try:
        # 获取运势分析
        fortune_result = FortuneService.get_fortune(test_person)
        print("运势分析结果:")
        print(f"命运分析: {fortune_result.fateAnalysis}")
        print(f"事业建议: {fortune_result.careerSuggestion}")
        print(f"爱情建议: {fortune_result.loveAdvice}")
        print(f"健康提醒: {fortune_result.healthWarning}")
        print(f"幸运数字: {fortune_result.luckyNumbers}")
        print(f"幸运颜色: {fortune_result.luckyColors}")
        print(f"综合评分: {fortune_result.overallScore}")
    except Exception as e:
        print(f"错误: {str(e)}")