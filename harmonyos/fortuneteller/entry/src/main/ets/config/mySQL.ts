const URL = 'http://192.168.204.1:8080';


class initMySQL {
  LoginUrl:string;
  RegisterUrl:string;
  GetPersonUrl:string;
  GetFortuneUrl:string;
  AddPersonUrl:string;
  AddFortuneUrl:string;

  constructor() {
    this.LoginUrl = URL+'/login'
    this.RegisterUrl = URL+'/regist'
    this.GetPersonUrl = URL+'/api/fortune/persons'
    this.AddPersonUrl = URL+'/api/fortune/person'
    this.GetFortuneUrl = URL+'/api/fortune/results'
    this.AddFortuneUrl = URL+'/api/fortune/result'

  }
}
const MySQL: initMySQL = new initMySQL();
export default MySQL;

