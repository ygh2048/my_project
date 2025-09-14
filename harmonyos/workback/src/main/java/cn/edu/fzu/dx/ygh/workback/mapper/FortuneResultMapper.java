package cn.edu.fzu.dx.ygh.workback.mapper;

import cn.edu.fzu.dx.ygh.workback.entity.FortuneResult;
import org.apache.ibatis.annotations.*;

import java.util.List;

@Mapper
public interface FortuneResultMapper {
    @Insert("INSERT INTO fortune_results " +
            "(person_id, fate_analysis, career_suggestion, love_advice, " +
            "health_warning, lucky_numbers, lucky_colors, overall_score, created_at) " +
            "VALUES (#{personId}, #{fateAnalysis}, #{careerSuggestion}, #{loveAdvice}, " +
            "#{healthWarning}, #{luckyNumbers}, #{luckyColors}, #{overallScore}, NOW())")
    @Options(useGeneratedKeys = true, keyProperty = "id")
    int insertFortuneResult(FortuneResult fortuneResult);

    @Select("SELECT * FROM fortune_results WHERE person_id = #{personId}")
    FortuneResult selectByPersonId(Integer personId);

    // 新增方法：查询多个personId的最新算命结果（每个personId只返回一个）
    @Select("<script>" +
            "SELECT fr.* FROM fortune_results fr " +
            "INNER JOIN (" +
            "   SELECT person_id, MAX(created_at) AS max_created_at " +
            "   FROM fortune_results " +
            "   GROUP BY person_id" +
            ") latest ON fr.person_id = latest.person_id AND fr.created_at = latest.max_created_at " +
            "WHERE fr.person_id IN " +
            "<foreach item='personId' collection='personIds' open='(' separator=',' close=')'>" +
            "   #{personId}" +
            "</foreach>" +
            "</script>")
    List<FortuneResult> selectLatestByPersonIds(@Param("personIds") List<Integer> personIds);
}
