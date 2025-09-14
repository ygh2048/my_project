package cn.edu.fzu.dx.ygh.workback.service.impl;

import cn.edu.fzu.dx.ygh.workback.dto.LoginRequest;
import cn.edu.fzu.dx.ygh.workback.dto.RegistRequest;
import cn.edu.fzu.dx.ygh.workback.entity.User;
import cn.edu.fzu.dx.ygh.workback.mapper.UserMapper;
import cn.edu.fzu.dx.ygh.workback.service.UserService;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;

import jakarta.servlet.http.HttpServletRequest;
import jakarta.servlet.http.HttpSession;

@Service
public class UserServiceImpl implements UserService {
    @Autowired
    private UserMapper userMapper;

    @Autowired
    private HttpServletRequest request; // 自动注入当前请求

    @Override
    public String login(LoginRequest loginRequest) {
        User user = userMapper.login(loginRequest);
        if (user == null) {
            return "用户名错误";
        } else if (user.getPassword().equals(loginRequest.getPassword())) {
            HttpSession session = request.getSession();
            session.setAttribute("userId", user.getId());
            return "登录成功";
        } else {
            return "密码错误";
        }
    }

    @Override
    public String regist(RegistRequest registRequest) {
        // 检查用户名是否已存在
        if (userMapper.existsByUsername(registRequest.getUsername())) {
            return "用户名已存在";
        }

        // 创建新用户
        User user = new User();
        user.setUsername(registRequest.getUsername());
        user.setPassword(registRequest.getPassword());
        user.setEmail(registRequest.getEmail());

        // 插入数据库
        try {
            userMapper.regist(user);
            return "注册成功";
        } catch (Exception e) {
            return "注册失败: " + e.getMessage();
        }
    }

    @Override
    public Integer getUserIdByUsername(String username) {
        // 调用Mapper获取用户ID
        return userMapper.getUserIdByUsername(username);
    }

}
