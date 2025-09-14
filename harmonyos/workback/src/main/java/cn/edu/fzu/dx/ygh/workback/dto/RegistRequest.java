package cn.edu.fzu.dx.ygh.workback.dto;

import lombok.Data;

@Data
public class RegistRequest {
    private String username;
    private String password;
    public String email;
}
