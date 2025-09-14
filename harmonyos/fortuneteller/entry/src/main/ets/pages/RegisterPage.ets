import { router } from '@kit.ArkUI';
import {http} from '@kit.NetworkKit'
import MySQL from '../config/mySQL';

interface RegistData{
  username: string;
  password: string;
  email: string;
}

@Entry
@Component
struct RegisterPage {
  @State message: string = 'Hello';
  @State username: string = ''
  @State password: string = ''
  @State comfirmPassword: string = ''
  @State email: string = ''
  @State errorMessage: string = ''

  build() {
    Column(){
      Text('用户注册')
        .fontSize(30)
        .margin(20)
      TextInput({placeholder:'请输入用户名'})
        .width('80%')
        .height(50)
        .margin(10)
        .fontSize(20)
        .onChange((value:string)=>{
          this.username=value
        })
      TextInput({placeholder:'请输入密码'})
        .width('80%')
        .height(50)
        .margin(10)
        .type(InputType.Password)
        .onChange((value:string)=>{
          this.password=value
        })
      TextInput({placeholder:'请确认密码'})
        .width('80%')
        .height(50)
        .margin(10)
        .type(InputType.Password)
        .onChange((value:string)=>{
          this.comfirmPassword=value
        })
      TextInput({placeholder:'请输入邮箱'})
        .width('80%')
        .height(50)
        .margin(10)
        .fontSize(20)
        .onChange((value:string)=>{
          this.email=value
        })
      if (this.errorMessage){
        Text(this.errorMessage)
          .fontSize(14)
          .margin(10)
          .fontColor('#FF0000')
      }

      Button('注册')
        .width('80%')
        .height(50)
        .margin(20)
        .onClick(()=>this.handleRegist())
      Button('返回登录')
        .width('80%')
        .height(50)
        .margin(5)
        .onClick(()=>{
          router.back()
        })
    }
    .width('100%')
    .height('100%')
    .justifyContent(FlexAlign.Center)
  }

  async handleRegist(): Promise<void> {
    if (!this.username||!this.password||!this.comfirmPassword||!this.email) {
      this.errorMessage='所有输入框均不能为空'
      return
    }
    if (this.password!==this.comfirmPassword) {
      this.errorMessage='密码和确认密码不一致'
      return
    }
    let registData:RegistData={
      username:this.username,
      password: this.password,
      email: this.email
    }
    let httpRequest = http.createHttp()
    const response = await httpRequest.request(
      MySQL.RegisterUrl,
      {
        method: http.RequestMethod.POST,
        header: {
          'Content-Type': 'application/json'
        },
        extraData: JSON.stringify(registData) //将登录参数转化为JSON格式
      }
    )
    console.log(response.result.toString())
    let result=response.result.toString()
    if (result.includes('注册成功')) {
      router.pushUrl({url:'pages/LoginPage'})
    }
    if (result.includes('用户名已经存在')){
      this.errorMessage='用户名已经存在'
    }
    else {
      this.errorMessage= result
    }
  }
}