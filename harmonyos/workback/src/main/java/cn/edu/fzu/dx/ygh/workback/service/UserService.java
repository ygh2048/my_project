package cn.edu.fzu.dx.ygh.workback.service;

import cn.edu.fzu.dx.ygh.workback.dto.LoginRequest;
import cn.edu.fzu.dx.ygh.workback.dto.RegistRequest;


public interface UserService {
    String  login(LoginRequest request);

    String  regist(RegistRequest request);

    Integer getUserIdByUsername(String username);
}
