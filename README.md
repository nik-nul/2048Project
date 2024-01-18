# 2048Project 报告

**via 231220103 刘赞宸**

[TOC]

### 游戏特点

- 实现了**正常的2048功能**，经测试无bug
- **(多于)两个关卡**：可自定义棋盘尺寸(包括非方形)，目标分数，24比例和是否记录历史等
- **趣味功能**：使用金币给指定格子设置为0
- **图形界面**：流程控制用户友好，格子有涂色
- **存档读档**功能和**排行榜**功能，可续玩，存档文件和排行榜可迁移

--------

### 技术特点

- 使用转置和反转以及向左合并的组合完成四个方向的合并实现，逻辑清晰，代码**高度复用**，避免重复

- **构造`Grid`Class**而非使用`int**`共享棋盘向量、避免指针传递

- **`Grid`**实例在游戏结束后自动析构，**内存安全**，连续 $1000$ 局游戏内存占用保持在`2kb`以内且没有增长趋势

- 使用**`Cereal`库**进行序列化读入读出存档和排行榜

- 使用**`regex`(正则表达式)**进行文件匹配

- 使用**模块化设计**，耦合程度低，便于调试与新功能的增加

  注：原本`Grid`Class是在一个独立的头文件内，但是为了提交方便合并成了单文件提交

- **变量命名**规则统一且有意义，逻辑缠绕的部分附有注释便于理解

- 文件开头有更新日志，使用**git**进行版本管理，出现问题便于回滚，同步方便

- **错误捕捉**完备，不会出现意外退出

  

----

### 游戏主体逻辑实现

#### 随机生成

统计 $0$的个数 $n$，生成随机数 $r\in[0,n)$ ，将第 $r$ 个 $0$设置为 $2$ 或 $4$ ，相比拒绝采样效率更高，一次随机可以保证输出

#### 合并

注意到：

```
向右合并 = 逆转 向左合并 逆转
向上合并 = 转置 向左合并 转置
向下合并 = 转置 逆转 向左合并 逆转 转置
```

故只需要实现向左合并

而向左合并可通过不断进行`向左填补空缺，再合并可能合并的相邻元素`实现，且这个操作最多执行 $c$次（ $c$是列数）

----

### 问题解决

在这里为了控制篇幅，引用代码只截取了最重要的部分

- 输入必须按回车 ：

  引入`termio.h `头文件改变终端的的输入模式

  ```C++
  #include <termio.h>
  int main(void){
      // many lines are omitted here
      termios tio;
      tcgetattr(STDIN_FILENO, &tio);
      tio.c_lflag &= (~ICANON);
      tcsetattr(STDIN_FILENO, TCSANOW, &tio);
      // many lines are omitted here
  }
  ```

- 非法输入导致读入错误：

  `cls()`宏在必要的地方清空读入读出缓存区

  ```C++
  #define cl_buf() while((xxx = getchar()) != 0x0a && xxx != 0 && xxx != -1)
  ```

- 默认序列化无法写入类对象：

  下载并使用`Cereal`库

  ```C++
  #include <cereal/archives/binary.hpp>
  #include <cereal/types/vector.hpp>
  #include <cereal/types/set.hpp>
  ```

- 不导致移动和合并的非法输入输出：

  `merge`返回`bool` 类型，如果输入没有导致效果会报错

  ```C++
  class Grid{
  	// many lines are omitted here
          bool left_merge();
          void reverse();
          bool right_merge();
          void transpose();
          bool down_merge();
          bool up_merge();
  	// many lines are omitted here
  };
  ```

  ```C++
  void play(string fname){
  	// many lines are omitted here
              case 0x1B:
                  getchar();
                  switch(getchar()){
                      case 0x43: flag = cur.right_merge(); break;
                      case 0x44: flag = cur.left_merge(); break;
                      case 0x41: flag = cur.up_merge(); break;
                      case 0x42: flag = cur.down_merge(); break;
                  }
          }
          if(!flag){
              cout<<endl<<"INVALID MOVE! TRY AGAIN!"<<endl;
              goto err;
          }
  	// many lines are omitted here
  }
  ```

- 游戏中临时保存时导致析构函数调用继而使棋盘大小恢复初始化游戏异常：

  为`Grid`类增加一个`arch`属性用来记载是否使用于存档，若用于存档则置 $1$ ，在析构函数中进行判断

  ```C++
  class Grid{
  	// many lines are omitted here
          bool arch = 0;
  	// many lines are omitted here
  };
  ```

  ```C++
  Grid::~Grid(){
      if(not self.arch){
          rank_save(score - step);
          ROW = 4; COLUMN = 4; GOAL = 2048; PROB = 10; wid = 6;
      }
      cout<<endl<<"Done!"<<endl;
  }
  ```

  ```C++
  bool save(Grid grid){
      ofstream ofs(to_string(grid.HASH)+".save", ios::binary);
      if(!ofs)
          return 0;
      cereal::BinaryOutputArchive oarchive(ofs);
      grid.arch = 1;
      oarchive(grid);
      grid.arch = 0;
      return 1;
  }
  ```

- 排行榜内容无法写入：

  以`std::ios::app`打开文件流并关闭可以做到若不存在则创建文件，如果使用默认参数且不写入内容会直接把文件清空

  ```C++
  bool rank_save(int r){
      ofstream ofs;
      ofs.open("ranklist", ios::binary | ios::app);
      if(!ofs)
          return 0;
      ofs.close();
  	// many lines are omitted here
  }
  ```

- `setw`大小确定：

  使用基准长度 $6$ 加上 $k \times \log_{10}最大可能数字$ （这是数字的位数）确定一个不太大又不过短的长度

  ```C++
  int wid = 6;
  int main(void){
      // many lines are omitted here
      wid = max(int(log10(GOAL)) + 3, 6);
      // many lines are omitted here
  }
  ```

- 涂色：

  使用`linux`下提供的`escape`控制符控制字体样式

  ```C++
  string colour_select(int n){
      switch(n){
          case 0: return "\033[30m\033[7m ";
          case 2: return "\033[31m\033[7m ";
          // and so on...
      }
  }
  ```

----

### 编译方法

代码在`Debian12`下使用`g++`编译（编译选项仅使用`-o`，`gcc`版本`12.2.0`）可以正常运行。 在编译前应当检查所在环境是否有`bits/stdc++.h`头文件(在`msvs`下似乎没有)，`dirent.h`头文件(`Windows`下似乎不适用)，`termio.h`头文件(`Windows`可能没有可操纵的终端，如果不使用`powershell`)。此外，应下载`cereal`头文件放到工作目录或`include`目录下(网站：https://uscilab.github.io/cereal/ )(这是用于序列化和反序列化实现存档读档的头文件。它是纯粹的头文件，故无需在本地编译部署，只需要下载然后粘贴进去就可以)。如果不使用终端或者`powershell`等软件提供的终端而直接使用命令行运行可能会导致`Escape屏幕控制符`(以`\033[`起始的特殊字符，用于控制输出颜色和样式等内容)加载失败而导致2048游戏图形界面失效，没有颜色。

另外，程序在编译时会提示一个警告，如下：

```
main.cpp: In function ‘std::vector<std::__cxx11::basic_string<char> > read_list()’:
main.cpp:304:25: warning: unknown escape sequence: '\.'
304 |     regex suffix_filter(".*\.save", regex::icase);
	|
```

这是正常现象，因为正则表达式里`.`是一个保留的关键字用于匹配，所以需要反斜杠转义，但是`g++`无法识别`regex`语法故而报出警告。程序运行实则符合预期且没有危险。

----

### 程序架构

`intro`函数实现首页显示和帮助输出

`play`函数进行游戏主体

`ranklist`实现排行榜显示

`intro_l`实现读取存档相关逻辑

`Grid`类包括所有跟游戏棋盘相关的内容：

- 所有全局变量的一份拷贝(以小写表示)
- `arch`属性表示是否为存档文件

- 构造函数进行初始化
- 析构函数复位全局变量
- 友元函数`Serialize`用于与`Cereal`库通讯
- 四种`merge`方法实现合并，以及辅助的`reverse`方法和`transpose`方法
- `set_zero`方法实现花金币设$0$的趣味功能
- `gen2`方法生成一个随机数字再棋盘上
- `deadQ`和`winQ`方法实现胜负判断
- `cnt_zero`方法清点棋盘上 $0$ 的个数，辅助`deadQ`、`winQ`和`gen2`
- `display`方法打印棋盘
- `step_increment`方法进行步数增加

`readlist`读取目录下的存档文件

`load`，`save`和`rank_load`，`rank_save`函数进行游戏存档和排行榜的读入读出

`print_vec`打印文件列表
