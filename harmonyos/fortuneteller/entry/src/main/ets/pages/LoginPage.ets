import { router } from '@kit.ArkUI';
import { http } from '@kit.NetworkKit';
import MySQL from '../config/mySQL';
import {LoginData , LoginResponse} from '../model/User'


@Entry
@Component
struct LoginPage {
  @State username: string = ''
  @State password: string = ''
  @State userID: number = -1
  @State errorMessage: string = ''


  build() {
    Column() {
      Text('用户登录')
        .fontSize(30)
        .margin(20)

      TextInput({ text: this.username, placeholder: '请输入用户名' })
        .width('80%')
        .height(50)
        .margin(10)
        .fontSize(20)
        .onChange((value: string) => {
          this.username = value
        })
      TextInput({ text: this.password, placeholder: '请输入密码' })
        .width('80%')
        .height(50)
        .margin(10)
        .type(InputType.Password)
        .onChange((value: string) => {
          this.password = value
        })
      if (this.errorMessage) {
        Text(this.errorMessage)
          .fontSize(14)
          .margin(10)
          .fontColor('#FF0000')
      }
      Button('登录')
        .width('80%')
        .height(50)
        .margin(20)
        .onClick(() => this.handleLogin())

      Button('注册')
        .width('80%')
        .height(50)
        .margin(5)
        .onClick(() => {
          router.pushUrl({ url: 'pages/RegisterPage' })
        })
    }
    .width('100%')
    .height('100%')
    .justifyContent(FlexAlign.Center)
  }

  async handleLogin(): Promise<void> {
    // 清除之前的错误消息
    this.errorMessage = ''

    // 验证输入
    if (!this.username || !this.password) {
      this.errorMessage = '用户名或密码不能为空'
      return
    }

    let loginData: LoginData = {
      username: this.username,
      password: this.password
    }

    let httpRequest = http.createHttp()

    try {
      // 发送登录请求
      const response = await httpRequest.request(
        MySQL.LoginUrl,
        {
          method: http.RequestMethod.POST,
          header: {
            'Content-Type': 'application/json'
          },
          extraData: JSON.stringify(loginData)
        }
      )
      // 解析JSON响应
      const responseStr = response.result.toString()
      console.log('登录响应:', responseStr)
      // 将响应字符串解析为JSON对象
      const result = JSON.parse(response.result.toString()) as LoginResponse

      // 根据code判断登录结果
      if (result.code === 200) {
        // 登录成功
        this.userID = result.userId // 存储用户ID
        console.log('登录成功，用户ID:', this.userID)

        // 跳转到主页
        router.pushUrl({
          url: 'pages/Index',
          params: {
            userID: this.userID,
            username : this.username,
            password : this.password,
            cookie : String(response.header['set-cookie'])
          } // 传递用户ID
        })
      } else {
        // 登录失败
        this.errorMessage = result.message || '登录失败'
      }
    } catch (error) {
      // 错误处理
      console.error('登录请求失败:', error)

      // 根据错误类型显示不同的消息
      if (error.code === 1) { // 无网络连接
        this.errorMessage = '网络不可用，请检查连接'
      } else if (error.code === 600) { // 超时
        this.errorMessage = '请求超时，请重试'
      } else {
        this.errorMessage = '网络请求失败，请重试'
      }
    }
  }
}
