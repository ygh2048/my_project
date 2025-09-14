import Borrow from './Borrow';
import Home from './Home';
import Output from './Output';
import { router } from '@kit.ArkUI';

import { Person, FortuneResult,HttpError} from '../model/Person';
import { UserInfo } from '../model/User'

@Entry
@Component
struct Index {
  @State currentIndex: number = 0
  @State personData: Person = { // 存储用户输入的数据
    id: 0,            // 新增ID字段
    userId: 0,        // 新增用户ID
    name: '',
    birthdate: new Date(1970, 0, 1),
    birthTime: '',         // 时辰，如"子时"
    gender: 'male'        // 'male' 或 'female'
  }
  @State fortuneResult: FortuneResult | null = null // 存储算命结果
  @State isLoading: boolean = false

  @State params: UserInfo = router.getParams() as UserInfo;

  @State userID: number = this.params.userID ?? -1;
  @State username: string = this.params.username || '游客';
  @State password: string = this.params.password || '';
  @State cookie : string = this.params.cookie || '';

  // 更新用户数据的方法
  updatePersonData(newData: Person) {
    this.personData = newData;
  }

  // 设置算命结果的方法
  setFortuneResult(result: FortuneResult) {
    this.fortuneResult = result;
    this.currentIndex = 1; // 自动切换到分析页面
  }

  // 设置加载状态
  setLoading(loading: boolean) {
    this.isLoading = loading;
  }

  @Builder tabBarBuilder(image: Resource, activeImage: Resource, text: string, index: number) {
    Column() {
      Image(this.currentIndex === index ? activeImage : image).width(32).height(32)
      Text(text).fontSize(14).fontColor(this.currentIndex === index ? '#0000FF' : '#000000') // 修正颜色值
    }
  }

  build() {
    Stack() {
      Tabs({
        barPosition: BarPosition.End,
        index:this.currentIndex
      }) {
        TabContent() {
          // 传递状态更新方法给Borrow组件
          Borrow({
            userInfo:$params,
            person: $personData,
            result:$fortuneResult,
            pageID : $currentIndex,
            isLoading : $isLoading,
            cookie : this.cookie
          })
        }.tabBar(this.tabBarBuilder($r('app.media.borrow0'), $r('app.media.borrow1'), '算命', 0))

        TabContent() {
          // 传递算命结果给Output组件
          Output({
            result:this.fortuneResult
          })
        }.tabBar(this.tabBarBuilder($r('app.media.chat0'), $r('app.media.chat1'), '分析', 1))

        TabContent() {
          Home({
            pageID : $currentIndex,
            person: $personData,
            result:$fortuneResult,
            userID:this.userID,
            username:this.username,
            cookie : this.cookie
          })
        }.tabBar(this.tabBarBuilder($r('app.media.person0'), $r('app.media.person1'), '我的', 2))
      }
      .onChange((index: number) => {
        this.currentIndex = index
      })
    }
  }
}
