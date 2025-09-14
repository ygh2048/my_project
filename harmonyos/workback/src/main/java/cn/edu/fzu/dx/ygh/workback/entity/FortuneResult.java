
package cn.edu.fzu.dx.ygh.workback.entity;

import lombok.Data;
import java.util.Date;

@Data
public class FortuneResult {
    private Integer id;
    private Integer personId;
    private String fateAnalysis;
    private String careerSuggestion;
    private String loveAdvice;
    private String healthWarning;
    private String luckyNumbers;
    private String luckyColors;
    private Integer overallScore;
    private Date createdAt;
}
