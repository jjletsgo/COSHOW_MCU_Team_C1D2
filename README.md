<img width="214" height="236" alt="image" src="https://github.com/user-attachments/assets/51a9b12f-ff82-4a46-a25b-277ec3235442" /># COSHOW_MCU_Team_C2D2
We’re building a treadmill with ATmega328P and 74-series logic ICs as a 3-person team.

<img width="500" height="375" alt="image" src="./images/atmega_treadmil.png"/>


# 1. Project Overview (프로젝트 개요)
- 프로젝트 이름: CHeeeeap Treadmill
- 프로젝트 설명: COSHOW MCU 응용 경진대회의 예선 작품으로, ATmega328p와 기타 주변 회로로 제작한 매우 저렴한 트레드밀입니다.
- <img src="\images\image copy.png" width="500" height="500">

<br/>
<br/>

# 2. Team Members (팀원 및 팀 소개)
+ 정재윤
+ 김민규
+ 이상민 

<br/>
<br/>

# 3. System Block Diagram
<img src="\images\image copy 7.png" width="500" height="500">

<br/>
<br/>

# 4. Circuit
- <img src="\images\image copy 4.png" width="500" height="500">
# 5. Tasks & Responsibilities (작업 및 역할 분담)
|  |  |  |
|-----------------|-----------------|-----------------|
| 정재윤    |  <img src="https://github.com/user-attachments/assets/c1c2b1e3-656d-4712-98ab-a15e91efa2da" alt="정재윤" width="100"> | <ul><li>프로젝트 계획 및 관리</li><li>팀 리딩 및 커뮤니케이션</li><li>74HC595 라이브러리 개발</li><li>4digit-FND 라이브러리 개발</li><li>속도 연동 RGB LED 제어 프로토콜 개발</li><li>UART 디버깅 라이브러리 개발</li><li>회로설계</li></ul>     |
| 김민규   |  <img src="https://github.com/user-attachments/assets/78ec4937-81bb-4637-975d-631eb3c4601e" alt="김민규" width="100">| <ul><li>메인 페이지 개발</li><li>동아리 만들기 페이지 개발</li><li>커스텀훅 개발</li></ul> |
| 이상민   |  <img src="https://github.com/user-attachments/assets/78ce1062-80a0-4edb-bf6b-5efac9dd992e" alt="이상민" width="100">    |<ul><li>홈 페이지 개발</li><li>로그인 페이지 개발</li><li>동아리 찾기 페이지 개발</li><li>동아리 프로필 페이지 개발</li><li>커스텀훅 개발</li></ul>  |
<br/>
<br/>

# 6. Technology Stack (기술 스택)
## 6.1 Language
|  |  |
|-----------------|-----------------|
| C |<img src="\images\c_logo.png" alt="C" width="100">| 
<br/>

## 6.2 Cooperation
|  |  |
|-----------------|-----------------|
| Git    |  <img src="https://github.com/user-attachments/assets/483abc38-ed4d-487c-b43a-3963b33430e6" alt="git" width="100">    |
<br/>

# 7. Project Structure (프로젝트 구조)
```plaintext
project/
├── public/
│   ├── index.html           # HTML 템플릿 파일
│   └── favicon.ico          # 아이콘 파일
├── src/
│   ├── assets/              # 이미지, 폰트 등 정적 파일
│   ├── components/          # 재사용 가능한 UI 컴포넌트
│   ├── hooks/               # 커스텀 훅 모음
│   ├── pages/               # 각 페이지별 컴포넌트
│   ├── App.js               # 메인 애플리케이션 컴포넌트
│   ├── index.js             # 엔트리 포인트 파일
│   ├── index.css            # 전역 css 파일
│   ├── firebaseConfig.js    # firebase 인스턴스 초기화 파일
│   package-lock.json    # 정확한 종속성 버전이 기록된 파일로, 일관된 빌드를 보장
│   package.json         # 프로젝트 종속성 및 스크립트 정의
├── .gitignore               # Git 무시 파일 목록
└── README.md                # 프로젝트 개요 및 사용법
```

<br/>
<br/>


# 8. 컨벤션 수행 결과
<img width="100%" alt="코드 컨벤션" src="https://github.com/user-attachments/assets/0dc218c0-369f-45d2-8c6d-cdedc81169b4">
<img width="100%" alt="깃플로우" src="https://github.com/user-attachments/assets/2a4d1332-acc2-4292-9815-d122f5aea77c">
