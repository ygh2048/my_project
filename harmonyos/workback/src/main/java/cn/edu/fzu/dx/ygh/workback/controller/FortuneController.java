package cn.edu.fzu.dx.ygh.workback.controller;

import cn.edu.fzu.dx.ygh.workback.dto.FortuneResultDTO;
import cn.edu.fzu.dx.ygh.workback.dto.PersonDTO;
import cn.edu.fzu.dx.ygh.workback.entity.FortuneResult;
import cn.edu.fzu.dx.ygh.workback.entity.Person;
import cn.edu.fzu.dx.ygh.workback.service.FortuneService;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.web.bind.annotation.*;

import jakarta.servlet.http.HttpServletRequest;
import java.util.List;

@RestController
@RequestMapping("/api/fortune")
public class FortuneController {

    @Autowired
    private FortuneService fortuneService;

    @PostMapping("/person")
    public String savePerson(@RequestBody PersonDTO personDTO, HttpServletRequest request) {
        // 获取会话中的用户ID
        Integer userId = (Integer) request.getSession().getAttribute("userId");
        if (userId == null) {
            return "用户未登录";
        }
        return fortuneService.savePerson(userId, personDTO);
    }

    @PostMapping("/result")
    public String saveFortuneResult(@RequestBody FortuneResultDTO fortuneResultDTO) {
        return fortuneService.saveFortuneResult(fortuneResultDTO);
    }

    @GetMapping("/persons")
    public List<Person> getUserPersons(HttpServletRequest request) {
        // 获取会话中的用户ID
        Integer userId = (Integer) request.getSession().getAttribute("userId");
        if (userId == null) {
            return null;
        }
        return fortuneService.getUserPersons(userId);
    }

    // 新增接口：获取用户的算命记录（每个personId只返回一个最新结果）
    @GetMapping("/results")
    public List<FortuneResult> getUserFortuneResults(HttpServletRequest request) {
        // 获取会话中的用户ID
        Integer userId = (Integer) request.getSession().getAttribute("userId");
        if (userId == null) {
            return null;
        }
        return fortuneService.getUserFortuneResults(userId);
    }
}
