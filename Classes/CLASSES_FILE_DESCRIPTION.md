# Classes 目录文件说明

本文档说明 `tests/cpp-empty-test/Classes` 目录下当前各个文件的职责，便于后续维护和继续开发。

## 目录总览

当前 `Classes` 目录包含以下文件：

- `AppDelegate.cpp`
- `AppDelegate.h`
- `AppMacros.h`
- `CardGameManager.cpp`
- `CardGameManager.h`
- `CardTypes.h`
- `HelloWorldScene.cpp`
- `HelloWorldScene.h`

---

## 1. AppDelegate.h / AppDelegate.cpp

### 作用
这两个文件负责应用启动流程，是 cocos2d-x 程序的入口控制层。

### 主要职责
- 初始化应用对象 `AppDelegate`
- 设置 OpenGL 上下文参数
- 创建窗口与设计分辨率
- 配置资源搜索路径
- 启动第一个场景 `HelloWorld::scene()`
- 处理程序切到后台/回到前台时的逻辑

### 在本项目中的实际作用
它们不负责卡牌规则本身，只负责把程序正常启动起来，并进入当前卡牌演示场景。

---

## 2. AppMacros.h

### 作用
这是 cocos2d-x 示例工程常见的配置头文件，用于放应用级宏定义或平台适配参数。

### 在本项目中的定位
当前项目里它属于模板工程保留文件，主要是基础配置支持文件，不承担核心卡牌逻辑。

---

## 3. CardTypes.h

### 作用
这个文件定义了整个卡牌系统使用的基础数据结构和枚举，是卡牌规则层的“类型基础”。

### 主要内容
- `CardSuit`
  - 花色枚举：梅花、方块、红桃、黑桃
- `CardZone`
  - 牌所在区域：主牌区 `Table`、备用牌区 `Stock`、底牌区 `Hand`
- `MoveType`
  - 移动类型：主牌到底牌、备用牌到底牌等
- `CardData`
  - 单张牌的数据结构
- `MoveRecord`
  - 历史记录，用于撤回
- `MoveResult`
  - 一次移动操作的返回结果
- `UndoResult`
  - 一次撤回操作的返回结果

### 在本项目中的定位
它不写具体规则，只定义“规则运行时需要哪些数据”。

---

## 4. CardGameManager.h / CardGameManager.cpp

### 作用
这两个文件负责整个卡牌玩法的核心规则，是项目里最重要的“游戏逻辑层”。

### 主要职责
- 初始化当前演示牌局
- 保存所有牌的数据状态
- 管理三大区域的数据变化：
  - 主牌区
  - 备用牌区
  - 底牌区
- 判断主牌是否可点击、是否满足相邻规则
- 处理主牌移到底牌区
- 处理备用牌翻到底牌区
- 处理撤回操作
- 处理主牌翻面逻辑

### 当前规则对应关系
在当前项目里，它实现的是以下机制：

#### 主牌区
- 上方三列主牌是主要攻略目标
- 玩家需要把主牌区全部移到底牌区
- 被上层压住的主牌不能点击
- 只有点数和当前底牌区顶牌相邻时才能移动

#### 底牌区
- 底牌区是当前过牌依据
- 新来的牌总是成为顶部牌
- 无论牌来自主牌区还是备用牌区，都会影响下一步可匹配数字

#### 备用牌区
- 备用牌区用于主牌区卡住时换数字
- 点击备用牌区可直接翻一张到底牌区
- 备用牌区本身不要求点数相邻

### 重要函数说明
- `setupDemo()`
  - 构造当前演示牌局
- `canMoveTableCardToHand()`
  - 判断主牌能否移到底牌区
- `moveTableCardToHand()`
  - 执行主牌到底牌区的移动
- `canDrawFromStock()`
  - 判断备用牌区是否还有牌
- `drawFromStock()`
  - 从备用牌区翻一张到底牌区
- `undo()`
  - 撤回上一步

### 在本项目中的定位
如果以后要改玩法规则，优先改这里。

---

## 5. HelloWorldScene.h / HelloWorldScene.cpp

### 作用
这两个文件负责当前卡牌玩法的界面展示和交互响应，是项目的“表现层 + 交互层”。

### 主要职责
- 创建 `HelloWorld` 场景
- 绘制主牌区、备用牌区、底牌区
- 创建每一张牌对应的可视节点
- 根据牌当前所在区域刷新正反面显示
- 响应玩家点击：
  - 点击主牌
  - 点击备用牌堆
  - 点击撤回按钮
- 将 `CardGameManager` 的数据状态同步到界面上

### 当前界面结构
- 上方：主牌区（三列）
- 左下：备用牌区
- 右下：底牌区
- 右下上方：撤回按钮和状态文字

### 当前显示规则
- 在备用牌区的牌：背面朝上
- 在底牌区的牌：正面朝上
- 在主牌区的牌：根据是否已翻开决定显示正反面
- 底牌区只显示当前顶部牌

### 重要函数说明
- `init()`
  - 初始化整个场景与基础 UI
- `createCardNode()`
  - 创建一张牌的节点和点击事件
- `rebuildAllCards()`
  - 重建所有牌节点
- `refreshCardVisual()`
  - 刷新单张牌的显示状态
- `refreshHandPile()`
  - 刷新底牌区
- `refreshStockPile()`
  - 刷新备用牌区
- `tryMoveTableCard()`
  - 处理主牌点击逻辑
- `drawFromStock()`
  - 处理备用牌点击逻辑
- `undoLastMove()`
  - 处理撤回逻辑

### 在本项目中的定位
如果以后要改界面布局、点击区域、牌面显示方式，优先改这里。

---

## 文件之间的关系

可以把当前 `Classes` 目录理解成三层：

### 1. 启动层
- `AppDelegate.h/.cpp`
- `AppMacros.h`

负责启动程序和创建场景。

### 2. 数据与规则层
- `CardTypes.h`
- `CardGameManager.h/.cpp`

负责牌的数据结构和核心游戏规则。

### 3. 界面与交互层
- `HelloWorldScene.h/.cpp`

负责把规则结果显示给玩家，并把玩家点击传回规则层。

---

## 后续维护建议

### 如果要改游戏规则
优先检查：
- `CardTypes.h`
- `CardGameManager.h`
- `CardGameManager.cpp`

### 如果要改界面效果或点击行为
优先检查：
- `HelloWorldScene.h`
- `HelloWorldScene.cpp`

### 如果程序打不开或窗口异常
优先检查：
- `AppDelegate.cpp`
- `AppDelegate.h`

---

## 当前项目的一句话总结

当前 `Classes` 目录实现的是一个基于 cocos2d-x 的简化卡牌演示项目：
- `CardGameManager` 负责规则
- `HelloWorldScene` 负责显示与点击
- `AppDelegate` 负责启动整个应用
