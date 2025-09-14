// 被算命人实体类型
export interface Person {
  id: number;            // 新增ID字段
  userId: number;        // 新增用户ID
  name: string;
  birthdate: Date;
  birthTime: string;          // 时辰，如"子时"
  gender: string;        // 'male' 或 'female'
}

// 算命结果类型
export interface FortuneResult {
  id: number;            // 新增ID字段
  personId: number;      // 新增人物ID
  fateAnalysis: string;        // 命运分析
  careerSuggestion: string;    // 事业建议
  loveAdvice: string;          // 爱情建议
  healthWarning: string;       // 健康提醒
  luckyNumbers: number[] | string;      // 幸运数字
  luckyColors: string[] | string;       // 幸运颜色
  overallScore: number;        // 综合评分(1-100)
  createdAt: Date;             // 新增创建时间
}


// 网络错误类型
export interface HttpError {
  code: number;
  message: string;
}


//sk-221bcc39bf464acaa60f2c21642f0a74