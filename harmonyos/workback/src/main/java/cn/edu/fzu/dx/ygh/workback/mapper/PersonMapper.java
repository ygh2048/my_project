package cn.edu.fzu.dx.ygh.workback.mapper;

import cn.edu.fzu.dx.ygh.workback.entity.Person;
import org.apache.ibatis.annotations.*;

import java.util.List;

@Mapper
public interface PersonMapper {
    @Insert("INSERT INTO persons (user_id, name, birthdate, birth_time, gender, created_at) " +
            "VALUES (#{userId}, #{name}, #{birthdate}, #{birthTime}, #{gender}, NOW())")
    @Options(useGeneratedKeys = true, keyProperty = "id")
    int insertPerson(Person person);

    @Select("SELECT * FROM persons WHERE user_id = #{userId}")
    List<Person> selectByUserId(Integer userId);
}
