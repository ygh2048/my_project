import { FortuneService } from '../utils/Deepseek'; // 导入算命服务
import { Person, FortuneResult,HttpError} from '../model/Person';
import { savePerson, saveFortuneResult} from '../utils/sqLUse';
import { UserInfo } from '../model/User';


@Component
export default struct Borrow{
  @Link userInfo : UserInfo;
  @Link person : Person;
  @Link result : FortuneResult;
  @Link pageID : number;
  @Link isLoading : boolean;
  @Prop cookie : string;
  //日期选择变量
  @State is_lunar: boolean = false

  //下拉菜单相关变量
  @State timeIndex:number = -1
  @State text:string = '请选择您的出生时辰'

  //有效信息
  @State truename:string = '某某某'
  @State gender:string = 'male'
  @State birthdate: Date = new Date()
  @State time: string = ''

  //显示信息
  @State age :number = 0
  @State zodiac: string = ''      // 生肖
  @State constellation: string = '' // 星座


  @State errorMessage: string | null = null;
  @State  isSuccess: boolean = false;

//黄建忠

  // 图片旋转角度
  @State angel: number = 0

  private timeArray: string[] = ['子时','丑时','寅时','卯时','辰时','巳时','午时','未时','申时','酉时','戌时','亥时'];

  // 12时辰数据
  private timePeriods:SelectOption[] = [
    { value: '子时(23点-1点)' },
    { value: '丑时(1点-3点)' },
    { value: '寅时(3点-5点)' },
    { value: '卯时(5点-7点)' },
    { value: '辰时(7点-9点)' },
    { value: '巳时(9点-11点)' },
    { value: '午时(11点-13点)' },
    { value: '未时(13点-15点)' },
    { value: '申时(15点-17点)' },
    { value: '酉时(17点-19点)' },
    { value: '戌时(19点-21点)' },
    { value: '亥时(21点-23点)' }
  ]

  private zodiacs: string[] = ['鼠', '牛', '虎', '兔', '龙', '蛇', '马', '羊', '猴', '鸡', '狗', '猪']

  private calculateZodiac(year: number): string {
    // 基于公元4年为鼠年基准计算
    return this.zodiacs[(year - 4) % 12]
  }

  // 在calculateConstellation方法内添加接口定义
  private calculateConstellation(month: number, day: number): string {
    interface ConstellationRange {
      start: [number, number]
      end: [number, number]
      name: string
    }

    const ranges: ConstellationRange[] = [
      { start: [3,21], end: [4,19], name: '白羊座' },
      { start: [4,20], end: [5,20], name: '金牛座' },
      { start: [5,21], end: [6,21], name: '双子座' },
      { start: [6,22], end: [7,22], name: '巨蟹座' },
      { start: [7,23], end: [8,22], name: '狮子座' },
      { start: [8,23], end: [9,22], name: '处女座' },
      { start: [9,23], end: [10,23], name: '天秤座' },
      { start: [10,24], end: [11,22], name: '天蝎座' },
      { start: [11,23], end: [12,21], name: '射手座' },
      { start: [12,22], end: [1,19], name: '摩羯座' },
      { start: [1,20], end: [2,18], name: '水瓶座' },
      { start: [2,19], end: [3,20], name: '双鱼座' }
    ]

  for (const range of ranges) {
    const startMonth = range.start[0];
    const startDay = range.start[1];
    const endMonth = range.end[0];
    const endDay = range.end[1];

    if ((month >= startMonth && month <= endMonth &&
        (month !== endMonth || day <= endDay)) ||
        (startMonth > endMonth &&
         (month >= startMonth || month <= endMonth) &&
         (month !== endMonth || day <= endDay))) {
      return range.name
    }
  }
  return '未知'
}


  // 设置加载状态
  setLoading(loading: boolean) {
    this.isLoading = loading;
  }

  // 设置错误信息
  setError(message: string) {
    this.errorMessage = message;
  }

  // 设置算命结果
  setFortuneResult(result: FortuneResult) {
    this.result = result;
  }
  private calculateAge(birthDate: Date): number {
    const today = new Date();
    let age = today.getFullYear() - birthDate.getFullYear();
    const monthDiff = today.getMonth() - birthDate.getMonth();

    if (monthDiff < 0 || (monthDiff === 0 && today.getDate() < birthDate.getDate())) {
      age--;
    }
    return age;
  }


  // 获取算命结果
  async getFortuneResult() {
    try {
      this.setLoading(true);
      this.setError('normal');

      const result = await FortuneService.getFortune(this.person);
      this.setFortuneResult(result);
      result.personId = this.person.id;
      console.log("获取ID:",this.person.id )
      try {
        const savedFortune = await saveFortuneResult(this.cookie, this.result);
        if(savedFortune)
        {

        }
        console.log("保存成功:", savedFortune);
      } catch (error) {
        console.error("保存失败:", error);
      }
      this.pageID = 1
    } catch (error) {
      const err = error as HttpError | Error;
      this.setError(`请求失败: ${err.message}`);
    } finally {
      this.setLoading(false);
    }
  }

  build() {
    Column(){
      Text('纯属娱乐，请勿作为事物依据')
        .fontSize(20)
        .width(300)
        .textAlign(TextAlign.Start)
        .padding(10)
        .height('8%')

      Row()
      {
        Text('请输入生辰')
          .fontSize(16)
          .fontWeight(FontWeight.Bold)
        Toggle({ type: ToggleType.Button, isOn: false}) {
          Text('使用农历')
            .fontColor('#182431')
            .fontSize(12)
            .textAlign(TextAlign.End)
      }
      .onChange((isOn:boolean) => {
            this.is_lunar = isOn
      })
      }.width(100)

      DatePicker({
        start: new Date(1900,0,1),
        end: new Date(2100,11,31),
        selected:this.birthdate
        })
        .onDateChange((date: Date) => {
          this.birthdate = date
          this.age = this.calculateAge(date)
          this.zodiac = this.calculateZodiac(date.getFullYear())
          this.constellation = this.calculateConstellation(date.getMonth()+1,date.getDate())

        })
        .margin({ top: 10 })
        .lunar(this.is_lunar)

      Select(this.timePeriods)
        .value(this.text)
        .font({size: 16 ,weight:400})
        .onSelect((index:number,text?: string|undefined) => {
          console.info('Select:' +index)
          this.timeIndex = index;
          if(text){
            this.text = text;
            this.time = this.timeArray[index]

          }
        })

      Row(){
        TextInput({text:"某某某",placeholder:'请输入您的姓名'})
          .width('70%')
          .height(50)
          .margin(10)
          .fontSize(20)
          .type(InputType.USER_NAME)
          .onChange((value:string)=>{
            this.truename=value
          })
        Column(){
          Text('男')
          Radio({value:'male',group:'gender'})
            .checked(true)
            .onChange(() =>{
              this.gender = 'male'
            })
        }
        Column(){
          Text('女')
          Radio({value:'female',group:'gender'})
            .checked(false)
            .onChange(() =>{
              this.gender = 'female'
            })
        }
      }

      Row(){
        Button('开始算命',{type: ButtonType.Normal,stateEffect:true})
          .borderRadius(8)
          .backgroundColor(0x317aff)
          .width(100)
          .height(40)
          .onClick(async() => {
            this.person.name = this.truename
            this.person.birthdate = this.birthdate
            this.person.birthTime = this.time
            this.person.gender = this.gender
            this.pageID = 0
            this.getFortuneResult()
          })

        Button('保存个人信息',{type: ButtonType.Normal,stateEffect:true})
          .borderRadius(8)
          .backgroundColor(0x317aff)
          .width(100)
          .height(40)
          .onClick(async() => {
            try{
              this.person.name = this.truename
              this.person.birthdate = this.birthdate
              this.person.birthTime = this.time
              this.person.gender = this.gender
              const savedPerson = await savePerson(this.cookie,this.person);
              if(savedPerson)
              {
                this.person.userId = this.userInfo.userID
                this.person.id = savedPerson.personId
              }
            } catch (error) {
              console.error("保存失败:", error);
            }
          })
      }
        Text('个人信息如下：')
        .fontSize(16)
        .fontColor(0x000000)
        .fontWeight(500)
        .margin({
          top: 20,
          left: 20,
          right: 20
          })

      Row(){
        Text('姓名：')
        .fontSize(16)
        .fontColor(0x000000)
        .fontWeight(500)
          .padding(10)
        Text(this.truename)
        .fontSize(16)
        .fontColor(0x000000)
        .fontWeight(500)
        .padding(10)

        Text('性别：')
          .fontSize(16)
          .fontColor(0x000000)
          .fontWeight(500)
          .padding(10)
        Text(
         (this.gender == 'male')? '男' : '女'
          )
          .fontSize(16)
          .fontColor(0x000000)
          .fontWeight(500)
          .padding(10)

      }
      Row(){
          Text('年龄：')
          .fontSize(16)
          .fontColor(0x000000)
          .fontWeight(500)
          .padding(10)
        Text(String(this.age))
          .fontSize(16)
          .fontColor(0x000000)
          .fontWeight(500)
          .padding(10)
        Text('岁')
      }
      Row(){
          Text('生肖：')
          .fontSize(16)
          .fontColor(0x000000)
          .fontWeight(500)
          .padding(10)
        Text(this.zodiac)
          .fontSize(16)
          .fontColor(0x000000)
          .fontWeight(500)
          .padding(10)
      }

      Row(){
        Text('星座：')
          .fontSize(16)
          .fontColor(0x000000)
          .fontWeight(500)
          .padding(10)
        Text(this.constellation)
          .fontSize(16)
          .fontColor(0x000000)
          .fontWeight(500)
          .padding(10)

      }
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
            .onAppear(() =>{
              animateTo({
                curve: Curve.Linear,
                playMode: PlayMode.Normal,
                iterations: -1, // 设置-1表示动画无限循环
                onFinish: () => {
                }
              }, () => {
                this.angel = 360
              })
            })
        }
      }
      if (this.errorMessage) {
        Text(this.errorMessage)
          .fontColor('#FF0000')
      }
    }
    .width('100%')
    .height('100%')
    .alignItems(HorizontalAlign.Start).width('90%').border({ width: 1 })

  }
}
