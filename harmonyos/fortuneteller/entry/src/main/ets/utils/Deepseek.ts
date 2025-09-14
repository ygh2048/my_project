import { http } from '@kit.NetworkKit';
import { Person, FortuneResult, HttpError } from '../model/Person';

const FORTUNE_API_URL = 'https://api.deepseek.com/v1/chat/completions';
const NETWORK_TIMEOUT = 20000;
const MODEL_NAME = 'deepseek-chat';

export class FortuneService {
  private static httpClient = http.createHttp();
  private static API_KEY = 'sk-221bcc39bf464acaa60f2c21642f0a74';

  static async getFortune(person: Person): Promise<FortuneResult> {
    try {
      // 构建符合 DeepSeek 要求的对话格式
      const messages = [
        {
          role: 'system',
          content: `你是一个专业的算命师，根据用户的个人信息进行运势分析。
          请严格按照以下 JSON 格式返回结果：
          {
            "fateAnalysis": "命运分析内容",
            "careerSuggestion": "事业建议内容",
            "loveAdvice": "爱情建议内容",
            "healthWarning": "健康提醒内容",
            "luckyNumbers": [1,2,3],
            "luckyColors": ["颜色1","颜色2"],
            "overallScore": 85
          }

          注意：只返回JSON格式，不要包含任何其他文本！`
        },
        {
          role: 'user',
          content: `姓名：${person.name}
出生日期：${this.formatDate(person.birthdate)}
出生时辰：${person.birthTime}
性别：${person.gender === 'male' ? '男' : '女'}`
        }
      ];

      // 发送非流式请求
      const response = await this.httpClient.request(
        FORTUNE_API_URL,
        {
          method: http.RequestMethod.POST,
          header: {
            'Content-Type': 'application/json',
            'Authorization': `Bearer ${this.API_KEY}`
          },
          extraData: JSON.stringify({
            model: MODEL_NAME,
            messages: messages,
            temperature: 0.6,
            max_tokens: 2048,
            response_format: { type: "json_object" } // 明确要求JSON格式响应
          }),
          readTimeout: NETWORK_TIMEOUT,
          connectTimeout: NETWORK_TIMEOUT
        }
      );

      // 处理响应 - 转换字符串响应为 FortuneResult 类型
      if (response.responseCode === 200) {
        const result = JSON.parse(response.result.toString());
        const content = result.choices[0].message.content;

        // 增强错误处理：确保内容不为空
        if (!content || content.trim() === '') {
          throw {
            code: 400,
            message: 'API 返回空内容'
          } as HttpError;
        }

        // 尝试从内容中提取JSON（模型可能在JSON前后添加额外文本）
        const jsonMatch = content.match(/\{[\s\S]*\}/);

        if (!jsonMatch) {
          throw {
            code: 400,
            message: '无法在响应中找到JSON内容'
          } as HttpError;
        }

        // 尝试解析提取到的JSON
        let parsed: any;
        try {
          parsed = JSON.parse(jsonMatch[0]);
        } catch (e) {
          throw {
            code: 400,
            message: `JSON 解析失败: ${(e as Error).message}`
          } as HttpError;
        }

        // 验证结构
        if (!this.validateFortuneResult(parsed)) {
          throw {
            code: 400,
            message: 'API 返回格式错误：缺少必要字段或格式不正确'
          } as HttpError;
        }

        return parsed as FortuneResult;
      } else {
        // 增强错误信息
        let errorText = '未知错误';
        try {
          errorText = JSON.parse(response.result.toString())?.error?.message || response.result.toString();
        } catch (e) {
          errorText = response.result.toString();
        }

        throw {
          code: response.responseCode,
          message: `API错误: ${response.responseCode} - ${errorText}`
        } as HttpError;
      }
    } catch (error) {
      // 统一错误处理
      if (error instanceof Object && 'code' in error && 'message' in error) {
        throw error as HttpError;
      } else {
        throw {
          code: -1,
          message: `网络请求失败: ${(error as Error).message || '未知错误'}`
        } as HttpError;
      }
    }
  }

  /**
   * 验证算命结果是否符合预期结构
   * @param result 解析后的对象
   * @returns 是否有效
   */
  private static validateFortuneResult(result: any): result is FortuneResult {
    return (
      typeof result.fateAnalysis === 'string' &&
        typeof result.careerSuggestion === 'string' &&
        typeof result.loveAdvice === 'string' &&
        typeof result.healthWarning === 'string' &&
      Array.isArray(result.luckyNumbers) &&
      result.luckyNumbers.every((n: any) => typeof n === 'number') &&
      Array.isArray(result.luckyColors) &&
      result.luckyColors.every((c: any) => typeof c === 'string') &&
        typeof result.overallScore === 'number' &&
        result.overallScore >= 0 &&
        result.overallScore <= 100
    );
  }

  private static formatDate(date: Date): string {
    const year = date.getFullYear();
    const month = (date.getMonth() + 1).toString().padStart(2, '0');
    const day = date.getDate().toString().padStart(2, '0');
    return `${year}-${month}-${day}`;
  }
}
