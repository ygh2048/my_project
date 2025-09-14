package cn.edu.fzu.dx.ygh.workback.service.impl;

import cn.edu.fzu.dx.ygh.workback.dto.FortuneResultDTO;
import cn.edu.fzu.dx.ygh.workback.dto.PersonDTO;
import cn.edu.fzu.dx.ygh.workback.entity.FortuneResult;
import cn.edu.fzu.dx.ygh.workback.entity.Person;
import cn.edu.fzu.dx.ygh.workback.mapper.FortuneResultMapper;
import cn.edu.fzu.dx.ygh.workback.mapper.PersonMapper;
import cn.edu.fzu.dx.ygh.workback.service.FortuneService;
import org.springframework.beans.BeanUtils;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;

import java.util.List;
import java.util.stream.Collectors;

@Service
public class FortuneServiceImpl implements FortuneService {

    @Autowired
    private PersonMapper personMapper;

    @Autowired
    private FortuneResultMapper fortuneResultMapper;

    @Override
    public String savePerson(Integer userId, PersonDTO personDTO) {
        Person person = new Person();
        BeanUtils.copyProperties(personDTO, person);
        person.setUserId(userId);
        person.setBirthTime(personDTO.getTime()); // 设置时辰

        int result = personMapper.insertPerson(person);
        return result > 0 ? "个人信息保存成功:" +person.getId(): "URL保存失败";
    }

    @Override
    public String saveFortuneResult(FortuneResultDTO fortuneResultDTO) {
        FortuneResult fortuneResult = new FortuneResult();
        BeanUtils.copyProperties(fortuneResultDTO, fortuneResult);

        // 转换列表为逗号分隔的字符串
        if (fortuneResultDTO.getLuckyNumbers() != null) {
            String numbersStr = fortuneResultDTO.getLuckyNumbers().stream()
                    .map(String::valueOf)
                    .collect(Collectors.joining(","));
            fortuneResult.setLuckyNumbers(numbersStr);
        }

        if (fortuneResultDTO.getLuckyColors() != null) {
            String colorsStr = String.join(",", fortuneResultDTO.getLuckyColors());
            fortuneResult.setLuckyColors(colorsStr);
        }

        int result = fortuneResultMapper.insertFortuneResult(fortuneResult);
        return result > 0 ? "算命结果保存成功" : "URL保存失败";
    }

    @Override
    public List<Person> getUserPersons(Integer userId) {
        return personMapper.selectByUserId(userId);
    }

    @Override
    public List<FortuneResult> getUserFortuneResults(Integer userId) {
        // 1. 获取用户的所有Person
        List<Person> persons = personMapper.selectByUserId(userId);
        List<Integer> personIds = persons.stream()
                .map(Person::getId)
                .collect(Collectors.toList());

        // 2. 如果没有Person，直接返回空列表
        if (personIds.isEmpty()) {
            return List.of();
        }

        // 3. 获取每个personId的最新算命结果
        return fortuneResultMapper.selectLatestByPersonIds(personIds);
    }
}
