import { http } from '@kit.NetworkKit';
import { router } from '@kit.ArkUI';
import { Person, FortuneResult } from '../model/Person';
import MySQL from '../config/mySQL';




@Component
export default struct Home {
  @Link pageID: number;
  @Link person: Person | null;
  @Link result: FortuneResult | null;
  @Prop cookie: string;
  @Prop userID: number;
  @Prop username: string;

  // 状态管理
  @State allPersons: Person[] = [];
  @State fortuneResults: Map<number, FortuneResult> = new Map(); // personId -> latest result
  @State loading: boolean = false;
  @State errorMessage: string = '';
  @State showPersonSelector: boolean = false;
  @State isLoading: boolean = false;
  @State angel: number = 0;

  // 加载人物
  async loadPersons() {
    this.isLoading = true;
    this.errorMessage = '';
    try {
      const persons = await this.fetchPersons();
      this.allPersons = persons;
      this.fortuneResults = new Map();
      this.result = null;
    } catch (error) {
      this.errorMessage = error.message || '人物加载失败';
      console.error('人物加载失败:', error);
    } finally {
      this.isLoading = false;
    }
  }

  // 加载算命结果
  async loadFortuneResults() {
    this.isLoading = true;
    this.errorMessage = '';

    try {
      // 1. 获取算命结果
      const results = await this.fetchFortuneResults();
      console.log('获取到的算命结果:', results); // 调试用

      // 2. 创建 personId 到最新结果的映射
      const resultsMap = new Map<number, FortuneResult>();

      // 3. 处理并存储结果 - 按创建时间排序，确保取最新
      results.sort((a, b) =>
      b.createdAt.getTime() - a.createdAt.getTime()
      );

      results.forEach(result => {
        // 如果该 personId 还没有结果，或当前结果更新，则存储
        const existing = resultsMap.get(result.personId);
        if (!existing || result.createdAt > existing.createdAt) {
          resultsMap.set(result.personId, result);
        }
      });

      this.fortuneResults = resultsMap;
      console.log('映射后的算命结果:', this.fortuneResults); // 调试用

      // 4. 设置当前人物的结果（如果有）
      if (this.person) {
        this.result = resultsMap.get(this.person.id) || null;

        if (this.result) {
          console.log(`找到人物 ${this.person.name} (ID: ${this.person.id}) 的算命结果`);
        } else {
          console.warn(`未找到人物 ${this.person.name} (ID: ${this.person.id}) 的算命结果`);
          this.errorMessage = `未找到 ${this.person.name} 的算命结果`;
        }
      }
    } catch (error) {
      // 5. 增强错误处理
      let errorMsg = '结果加载失败';

      if (error instanceof Error) {
        errorMsg = error.message;
      } else if (typeof error === 'string') {
        errorMsg = error;
      }

      this.errorMessage = errorMsg;
      console.error('结果加载失败:', error);
    } finally {
      this.isLoading = false;
    }
  }


  // 获取用户的所有人物
  async fetchPersons(): Promise<Person[]> {
    const httpRequest = http.createHttp();

    // 优化cookie提取
    const sessionMatch = this.cookie.match(/JSESSIONID=([^;]+)/);
    const cookieHeader = sessionMatch ? `JSESSIONID=${sessionMatch[1]}` : '';

    const response = await httpRequest.request(
      MySQL.GetPersonUrl,
      {
        method: http.RequestMethod.GET,
        header: {
          'Content-Type': 'application/json',
          'Cookie': cookieHeader
        }
      }
    );

    console.log('响应状态码:', response.responseCode);

    if (response.responseCode !== 200) {
      throw new Error(`获取失败，状态码: ${response.responseCode}`);
    }

    try {
      const jsonString = response.result.toString();
      console.log('原始响应:', jsonString);

      const rawData = JSON.parse(jsonString) as Person[];

      if (!Array.isArray(rawData)) {
        throw new Error('返回数据不是数组');
      }

      return rawData.map((item :  Person )=> {
        // 处理可能的空值
        const name = item.name || "未命名";
        const birthTime = item.birthTime || "未知时辰";
        const gender = item.gender || "unknown";

        // 安全处理日期
        let birthdate: Date;
        try {
          // 添加时区信息避免偏移
          birthdate = new Date(`${item.birthdate}T00:00:00+08:00`);
          if (isNaN(birthdate.getTime())) throw new Error("无效日期");
        } catch (e) {
          console.warn(`无效日期: ${item.birthdate}`, e);
          birthdate = new Date(); // 默认使用当前日期
        }

        return {
          id: item.id,
          userId: item.userId,
          name,
          birthdate,
          birthTime: birthTime,
          gender
        } as Person;
      });

    } catch (error) {
      console.error('解析错误:', error);
      throw new Error('解析人物信息失败');
    }
  }


  async fetchFortuneResults(): Promise<FortuneResult[]> {
    const httpRequest = http.createHttp();

    // 优化cookie提取
    const sessionMatch = this.cookie.match(/JSESSIONID=([^;]+)/);
    const cookieHeader = sessionMatch ? `JSESSIONID=${sessionMatch[1]}` : '';

    const response = await httpRequest.request(
      MySQL.GetFortuneUrl,
      {
        method: http.RequestMethod.GET,
        header: {
          'Content-Type': 'application/json',
          'Cookie': cookieHeader
        }
      }
    );

    console.log('算命结果响应码:', response.responseCode);

    if (response.responseCode !== 200) {
      const errorDetail = response.result?.toString() || '无响应内容';
      throw new Error(`请求失败 [${response.responseCode}]: ${errorDetail}`);
    }

    try {
      const jsonString = response.result.toString();
      console.log('原始算命结果:', jsonString); // 调试用

      const rawData = JSON.parse(jsonString) as FortuneResult[];

      if (!Array.isArray(rawData)) {
        throw new Error('返回数据不是数组');
      }

      return rawData.map((item: FortuneResult ) => {
        // 处理可能的空值
        const createdAt = item.createdAt ? new Date(item.createdAt) : new Date();

        return {
          id: item.id,
          personId: item.personId,
          fateAnalysis: item.fateAnalysis || "暂无命理分析",
          careerSuggestion: item.careerSuggestion || "暂无职业建议",
          loveAdvice: item.loveAdvice || "暂无爱情建议",
          healthWarning: item.healthWarning || "暂无健康提醒",
          luckyNumbers: item.luckyNumbers || "暂无幸运数字",
          luckyColors: item.luckyColors || "暂无幸运颜色",
          overallScore: item.overallScore || 0,
          createdAt
        } as FortuneResult;
      });

    } catch (error) {
      console.error('解析算命结果失败:', error);
      throw new Error('解析数据失败');
    }
  }


  // 选择人物
  selectPerson(person: Person) {
    this.person = person;
    this.result = this.fortuneResults.get(person.id) || null;
    this.showPersonSelector = false;
  }

  // 格式化日期
  formatDate(date: Date): string {
    return `${date.getFullYear()}-${(date.getMonth() + 1).toString().padStart(2, '0')}-${date.getDate().toString().padStart(2, '0')}`;
  }

  build() {
    Stack() {
      Column() {
        // 用户信息展示区
        Row() {
          Image($r('app.media.FlatUiDude'))
            .width(40)
            .height(40)

          Text('用户名:')
            .width('25%')
            .height('25%')
          Text(this.username)
            .width('35%')
            .height('25%')

          Text('UID')
            .width('15%')
            .height('15%')
          Text(String(this.userID))
            .width('20%')
            .height('15%')
        }
        .padding(10)

        // 分隔线
        Divider()
          .strokeWidth(1)
          .color('#eeeeee')
          .margin({ top: 5, bottom: 5 })

        // 加载状态
        if (this.isLoading) {
          Column() {
            Image($r("app.media.app_icon"))
              .width(30)
              .height(30)
              .rotate({
                centerX: "50%",
                centerY: "50%",
                angle: this.angel,
              })
              .draggable(false)
              .visibility(this.isLoading ? Visibility.Visible : Visibility.None)
              .onAppear(() => {
                animateTo({
                  curve: Curve.Linear,
                  playMode: PlayMode.Normal,
                  iterations: -1,
                  onFinish: () => {}
                }, () => {
                  this.angel = 360
                })
              })
          }
        }

        // 错误提示
        else if (this.errorMessage) {
          Column() {
            Text(this.errorMessage)
              .fontSize(16)
              .fontColor('#ff0000')
            Button('重试')
              .width(100)
              .height(40)
              .margin({ top: 10 })
              .onClick(() => {
                if (this.allPersons.length === 0) {
                  this.loadPersons();
                } else {
                  this.loadFortuneResults();
                }
              })
            Button('返回')
              .width(100)
              .height(40)
              .margin({ top: 10 })
              .onClick(() => {
                this.errorMessage = '';
                this.loadPersons();
              })
          }
          .width('100%')
          .height(200)
          .justifyContent(FlexAlign.Center)
        }

        // 主要内容
        else {
          // 根据状态显示不同内容
          if (this.allPersons.length === 0) {
            // 初始状态：加载人物按钮
            Button('加载人物')
              .width('80%')
              .height(50)
              .onClick(() => this.loadPersons())
              .margin({ top: 20 })
          } else {
            // 显示人物信息和加载结果按钮
            Column() {
              // 当前选中的人物
              if (this.person) {
                Text('当前人物:')
                  .fontSize(18)
                  .fontWeight(FontWeight.Bold)
                  .margin({ bottom: 10 })

                Row() {
                  Text('姓名:')
                    .width('25%')
                  Text(this.person.name)
                    .width('75%')
                }
                .margin({ bottom: 5 })

                Row() {
                  Text('出生日期:')
                    .width('25%')
                  Text(this.formatDate(this.person.birthdate))
                    .width('75%')
                }
                .margin({ bottom: 5 })

                Row() {
                  Text('出生时辰:')
                    .width('25%')
                  Text(this.person.birthTime)
                    .width('75%')
                }
                .margin({ bottom: 5 })

                Row() {
                  Text('性别:')
                    .width('25%')
                  Text(this.person.gender === 'male' ? '男' : '女')
                    .width('75%')
                }
              } else {
                Text('无人物记录')
                  .fontSize(16)
                  .fontColor('#999')
              }

              // 选择人物按钮
              Button(this.person ? '更换人物' : '选择人物')
                .width('60%')
                .margin({ top: 15 })
                .onClick(() => {
                  this.showPersonSelector = true;
                })

              // 如果未加载结果，显示加载按钮
              if (this.fortuneResults.size === 0) {
                Button('加载结果')
                  .width('80%')
                  .height(50)
                  .margin({ top: 10 })
                  .onClick(() => this.loadFortuneResults())
              } else if (this.result) {     }
            }
            .padding(15)
            .backgroundColor('#f9f9f9')
            .borderRadius(8)
            .margin({ bottom: 20 })
          }
        }

        // 返回登录按钮
        Button("返回登录页面")
          .width('80%')
          .height(50)
          .onClick(() => {
            router.pushUrl({ url: 'pages/LoginPage' })
          })

        // 返回算命按钮
        Button("返回算命页面")
          .width('80%')
          .height(50)
          .onClick(() => {
            this.pageID = 0;
          })
      }
      .width('100%')
      .height('100%')
      .padding(15)

      // 人物选择弹窗
      if (this.showPersonSelector) {
        Column() {
          Text('选择人物')
            .fontSize(20)
            .fontWeight(FontWeight.Bold)
            .margin({ bottom: 15 })

          // 人物列表
          List({ space: 10 }) {
            if (this.allPersons.length === 0) {
              ListItem() {
                Text('无人物记录')
                  .fontSize(16)
                  .fontColor('#999')
                  .width('100%')
                  .textAlign(TextAlign.Center)
                  .padding(20)
              }
            } else {
              ForEach(this.allPersons, (person: Person) => {
                ListItem() {
                  Column() {
                    Text(person.name)
                      .fontSize(18)
                      .fontWeight(FontWeight.Bold)

                    Row() {
                      Text(`出生: ${this.formatDate(person.birthdate)}`)
                        .fontSize(14)

                      Text(`时辰: ${person.birthTime}`)
                        .fontSize(14)
                        .margin({ left: 15 })

                      Text(`性别: ${person.gender === 'male' ? '男' : '女'}`)
                        .fontSize(14)
                        .margin({ left: 15 })
                    }
                    .margin({ top: 5 })
                  }
                  .width('100%')
                  .padding(10)
                  .backgroundColor(this.person?.id === person.id ? '#e6f7ff' : '#ffffff')
                  .borderRadius(8)
                  .onClick(() => this.selectPerson(person))
                }
              })
            }
          }
          .layoutWeight(1)
          .height('60%')

          // 关闭按钮
          Button('关闭')
            .width('80%')
            .height(40)
            .margin({ top: 15 })
            .onClick(() => {
              this.showPersonSelector = false;
            })
        }
        .width('80%')
        .height('70%')
        .padding(15)
        .backgroundColor('#ffffff')
        .borderRadius(12)
        .align(Alignment.Center)
      }
    }
  }
}
