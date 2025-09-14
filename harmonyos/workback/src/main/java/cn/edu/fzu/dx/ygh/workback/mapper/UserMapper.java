package cn.edu.fzu.dx.ygh.workback.mapper;

import cn.edu.fzu.dx.ygh.workback.dto.LoginRequest;
import cn.edu.fzu.dx.ygh.workback.entity.User;

import org.apache.ibatis.annotations.Insert;
import org.apache.ibatis.annotations.Mapper;
import org.apache.ibatis.annotations.Select;

@Mapper
public interface UserMapper {
    @Select("SELECT * FROM users WHERE username=#{username}")
    User login(LoginRequest loginRequest);

    @Insert("INSERT INTO users (username,password,email) VALUES (#{username},#{password},#{email})")
    void regist(User user);
//获取ID
    @Select("SELECT id FROM users WHERE username = #{username}")
    Integer getUserIdByUsername(String username);

    @Select("SELECT COUNT(*) FROM users WHERE username = #{username}")
    boolean existsByUsername(String username);
}
