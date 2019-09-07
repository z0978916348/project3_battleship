# project3_battleship
## 環境開發選擇使用 Virtualbox + Image 開發。

## 應用C++物件導向程式技巧開發軟體                                                                                                                 
團隊合作完成此battleship project                                                                                                                       
## 使用 share library 的技巧，完成 AI 的模組                                                                                                                
## 使用 share library 的技巧，載入 AI 並且執行                                                                                                   
## 遊戲內容 。
### 1.雙方各有一個 20x20 大小的棋盤                                                                                                              
### 2.雙方各有一個 4 艘船，大小分別為 : 3x3 兩個 , 5x5 一個 , 7x7 一個
 
### 3.船隻分為三個狀態 (1)Available (2)Hit (3)Sink                                                                                       

(1)Available 如果船沒有被打中：可以攻擊、可以移動

(2)Hit 如果船被打中不是中心點：可以攻擊、不能移動

(3)Sink 如果船被打中中心點：不能攻擊、不能移動

### 4.如如果攻擊方有 K 艘沒死掉的船，就可以攻擊 K 次
每次攻擊，會給 AI 目前對手已公開的資訊，AI會回傳攻擊的座標

### 5.Board 是一個 State 的 2D-array  紀錄已公開的資訊，每一格也擁有三個狀態 (1) Unknow (2) Empty (3) Hit :

(1) Unknow : 未被攻擊的格子

(2) Empty  : 已被攻擊，沒東西

(3) Hit    : 已被攻擊，打中船了

### 6.船隻完成攻擊後，AI可以有移動船隻。

(1) 只由活著的船 (Available) 可以移動，其餘的都不能移動

(2) 每次移動只能往上下左右移動一個格子

(3) 可以不移動

(4) 移動完不能超出棋盤範圍，不能重疊

(5) 不能移動到任何已經被攻擊的格子上

(6) 不考慮移動時重疊

### share library:
![](shared_library.png)

### 遊戲階段:
![](遊戲階段.png)

### 遊戲畫面:
![](battle.png)

![](321313.jpg)


