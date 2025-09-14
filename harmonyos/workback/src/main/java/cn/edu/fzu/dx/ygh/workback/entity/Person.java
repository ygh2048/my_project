package cn.edu.fzu.dx.ygh.workback.entity;
import java.util.Date;
import lombok.Data;

@Data
public class Person {
    private Integer id;
    private Integer userId;
    private String name;
    private Date birthdate;
    private String birthTime;
    private String gender;
    private Date createdAt;
}