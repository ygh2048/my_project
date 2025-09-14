package cn.edu.fzu.dx.ygh.workback.controller;

import cn.edu.fzu.dx.ygh.workback.dto.LoginRequest;
import cn.edu.fzu.dx.ygh.workback.dto.RegistRequest;
import cn.edu.fzu.dx.ygh.workback.service.UserService;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.web.bind.annotation.RequestBody;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RestController;

import java.util.HashMap;
import java.util.Map;

@RestController
public class UserController {
    @Autowired
    private UserService userService;

    @RequestMapping("/login")
    public Map<String, Object> login(@RequestBody LoginRequest loginRequest) {
        Map<String, Object> response = new HashMap<>();
        String result = userService.login(loginRequest);

        if ("登录成功".equals(result)) {
            // 获取用户ID
            Integer userId = userService.getUserIdByUsername(loginRequest.getUsername());

            response.put("code", 200);
            response.put("message", "登录成功");
            response.put("userId", userId); // 返回用户ID
        } else {
            response.put("code", 401);
            response.put("message", result);
        }

        return response;
    }

    @RequestMapping("/regist")
    public Map<String, Object> regist(@RequestBody RegistRequest registRequest) {
        Map<String, Object> response = new HashMap<>();
        String result = userService.regist(registRequest);

        if ("注册成功".equals(result)) {
            // 获取新注册用户的ID
            Integer userId = userService.getUserIdByUsername(registRequest.getUsername());

            response.put("code", 200);
            response.put("message", "注册成功");
            response.put("userId", userId); // 返回用户ID
        } else {
            response.put("code", 400);
            response.put("message", result);
        }

        return response;
    }
}
