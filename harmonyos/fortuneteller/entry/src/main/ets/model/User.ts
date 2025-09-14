export interface UserInfo {
  userID: number;
  username: string; // 可选字段
  password: string;
  cookie:  string;
}

export interface LoginData {
  username: string;
  password: string;
}

export interface LoginResponse {
  code: number
  message: string
  userId: number
}
