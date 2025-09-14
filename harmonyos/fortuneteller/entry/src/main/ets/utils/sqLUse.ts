import { http } from '@kit.NetworkKit';
import { Person, FortuneResult, HttpError } from '../model/Person';
import MySQL from '../config/mySQL';
import preferences from '@ohos.data.preferences';

/**
 * 保存Person信息
 * @param person Person对象
 * @returns 保存后的Person对象
 * @throws HttpError 网络错误
 */
export async function savePerson(cookie:string,person: Omit<Person, 'id' | 'userId'>): Promise<{ personId: number;}> {
  try {
    const httpRequest = http.createHttp();
    console.info('cookie: ' + cookie)
    const match = cookie.match(/JSESSIONID=[^;]+/);
    console.info('jessionid ' + match)
    // 准备请求数据
    const requestData = {
      name: person.name,
      birthdate: person.birthdate.toISOString(), // 转换为ISO格式
      time: person.birthTime,
      gender: person.gender
    };

    // 发送POST请求
    const response = await httpRequest.request(
      MySQL.AddPersonUrl,
      {
        method: http.RequestMethod.POST,
        header: {
          'Content-Type': 'application/json',
          'Cookie': match[0]
        },
        extraData: JSON.stringify(requestData)
      }
    );
  console.info('person information ',requestData.name)

    console.info('cookie: ' + cookie)
    console.info('Person save:  ' + response.result.toString())
    // 处理响应
    if (response.result.toString().includes('成功')) {
      const idStr = response.result.toString().split(':')[1];
      const personId = parseInt(idStr, 10);
      console.info('personId: ' + personId)
      return {personId};
    } else {
      {
        return {personId: -1};
      };
    }
  }
  catch (error) {
    return {personId: -1};
  }
}

/**
 * 保存Fortune结果
 * @param fortuneResult FortuneResult对象
 * @returns 保存后的FortuneResult对象
 * @throws HttpError 网络错误
 */
export async function saveFortuneResult(cookie : string,fortuneResult: Omit<FortuneResult, 'id' | 'createdAt'>): Promise<boolean> {
  try {

    const httpRequest = http.createHttp();
    const match = cookie.match(/JSESSIONID=[^;]+/);
    // 准备请求数据
    const requestData = {
      personId: fortuneResult.personId,
      fateAnalysis: fortuneResult.fateAnalysis,
      careerSuggestion: fortuneResult.careerSuggestion,
      loveAdvice: fortuneResult.loveAdvice,
      healthWarning: fortuneResult.healthWarning,
      luckyNumbers: fortuneResult.luckyNumbers,
      luckyColors: fortuneResult.luckyColors,
      overallScore: fortuneResult.overallScore
    };

    console.log(String(requestData))
    // 发送POST请求
    const response = await httpRequest.request(
      MySQL.AddFortuneUrl,
      {
        method: http.RequestMethod.POST,
        header: {
          'Content-Type': 'application/json' ,
          'Cookie':  match[0]
        },
        extraData: JSON.stringify(requestData)
      }
    );

    console.info('saveFortuneResult   '+  response.result.toString())
    // 处理响应
    if (response.result.toString().includes('成功')) {
      return true;
    } else {
      return false;
    }
  }
  catch (error) {
    return false
  }
}
