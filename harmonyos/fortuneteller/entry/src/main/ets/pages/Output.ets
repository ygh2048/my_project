import { FortuneResult } from '../model/Person';

@Component
export default struct Output {
  @Prop result: FortuneResult | null; // 从父组件接收算命结果

  build() {
    Column() {
      Text('AI专业分析')
        .fontSize(24)
        .fontWeight(FontWeight.Bold)

      if (this.result) {
        this.renderFortuneResult(this.result)
      } else {
        Column() {
          Image($r('app.media.foreground'))
            .width(150)
            .height(150)
            .margin({ bottom: 20 })
          Text('暂无算命结果')
            .fontSize(18)
            .fontColor('#999999')
          Text('请在算命页面输入信息并开始算命')
            .fontSize(14)
            .fontColor('#999999')
            .margin({ top: 10 })
        }
        .width('100%')
        .height('100%')
        .justifyContent(FlexAlign.Center)
      }
    }
    .width('100%')
    .height('100%')
    .padding(20)
  }

  // 渲染算命结果 - 添加参数确保非空
  @Builder
  renderFortuneResult(result: FortuneResult) {
    Scroll() {
      Column() {
        // 综合评分
        Row() {
          Text('综合评分:')
            .fontSize(18)
            .fontWeight(FontWeight.Bold)
          Text(`${result.overallScore}/100`)
            .fontSize(18)
            .fontColor('#FF6A00')
            .margin({ left: 10 })
        }
        .margin({ bottom: 20 })

        // 命运分析
        Text('命运分析')
          .fontSize(18)
          .fontWeight(FontWeight.Bold)
          .margin({ bottom: 10 })
        Text(result.fateAnalysis)
          .fontSize(16)
          .margin({ bottom: 20 })
          .textAlign(TextAlign.Start)

        // 事业建议
        Text('事业建议')
          .fontSize(18)
          .fontWeight(FontWeight.Bold)
          .margin({ bottom: 10 })
        Text(result.careerSuggestion)
          .fontSize(16)
          .margin({ bottom: 20 })
          .textAlign(TextAlign.Start)

        // 爱情建议
        Text('爱情建议')
          .fontSize(18)
          .fontWeight(FontWeight.Bold)
          .margin({ bottom: 10 })
        Text(result.loveAdvice)
          .fontSize(16)
          .margin({ bottom: 20 })
          .textAlign(TextAlign.Start)

        // 健康提醒
        Text('健康提醒')
          .fontSize(18)
          .fontWeight(FontWeight.Bold)
          .margin({ bottom: 10 })
        Text(result.healthWarning)
          .fontSize(16)
          .margin({ bottom: 20 })
          .textAlign(TextAlign.Start)

        // 幸运信息
        Row() {
            Text('幸运数字:')
              .fontSize(16)
              .fontWeight(FontWeight.Medium)
            Text(
              Array.isArray(result.luckyNumbers)
                ? result.luckyNumbers.join(', ')
                : result.luckyNumbers
            )
              .fontSize(16)
              .fontColor('#ffffbf00')
              .margin({ top: 5 })

            Text('幸运颜色:')
              .fontSize(16)
              .fontWeight(FontWeight.Medium)
            Text(
              Array.isArray(result.luckyColors)
                ? result.luckyColors.join(', ')
                : result.luckyColors
            )
              .fontSize(16)
              .fontColor('#ffffbf00')
              .margin({ top: 5 })
        }
      }
      .width('100%')
    }
  }
}
