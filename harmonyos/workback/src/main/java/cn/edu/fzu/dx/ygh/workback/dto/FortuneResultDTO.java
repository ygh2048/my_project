package cn.edu.fzu.dx.ygh.workback.dto;

import lombok.Data;
import java.util.List;

@Data
public class FortuneResultDTO {
    private Integer personId;
    private String fateAnalysis;
    private String careerSuggestion;
    private String loveAdvice;
    private String healthWarning;
    private List<Integer> luckyNumbers;
    private List<String> luckyColors;
    private Integer overallScore;
}