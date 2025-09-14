package cn.edu.fzu.dx.ygh.workback.service;

import cn.edu.fzu.dx.ygh.workback.dto.FortuneResultDTO;
import cn.edu.fzu.dx.ygh.workback.dto.PersonDTO;
import cn.edu.fzu.dx.ygh.workback.entity.FortuneResult;
import cn.edu.fzu.dx.ygh.workback.entity.Person;

import java.util.List;

public interface FortuneService {
    String savePerson(Integer userId, PersonDTO personDTO);
    String saveFortuneResult(FortuneResultDTO fortuneResultDTO);
    List<Person> getUserPersons(Integer userId);

    List<FortuneResult> getUserFortuneResults(Integer userId);
}
