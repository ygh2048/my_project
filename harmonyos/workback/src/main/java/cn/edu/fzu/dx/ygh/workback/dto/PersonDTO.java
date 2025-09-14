package cn.edu.fzu.dx.ygh.workback.dto;

import lombok.Data;
import java.util.Date;

@Data
public class PersonDTO {
    private String name;
    private Date birthdate;
    private String time;      // 时辰，如"子时"
    private String gender;    // 'male' 或 'female'
}
