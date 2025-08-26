# TASK

精心设计问题中变元、文字、子句、公式等有效的物理存储结构，基于DPLL过程实现一个高效SAT求解器，对于给定的中小规模算例进行求解，输出求解结果，统计求解时间。

## 程序功能

### 1.输入输出功能：

包括程序执行参数的输入，SAT算例cnf文件的读取，执行结果的输出与文件保存等。

### 2.公式解析与验证：

读取cnf算例文件，解析文件，基于一定的物理结构，建立公式的内部表示；
并实现对解析正确性的验证功能，即遍历内部结构逐行输出与显示每个子句，与输入算例对比可人工判断解析功能的正确性。

数据结构的设计可参考文献：

1.张健著. 逻辑公式的可满足性判定—方法、工具及应用. 科学出版社，2000

2.Tanbir Ahmed. An Implementation of the DPLL Algorithm. Master thesis, Concordia University,Canada,2009

3.陈稳. 基于DPLL的SAT算法的研究与应用.硕士学位论文，电子科技大学，2011

### 3.DPLL过程：

基于**DPLL**算法框架，实现SAT算例的求解

### 4.时间性能的测量：

基于相应的时间处理函数（参考time.h），记录DPLL过程执行时间（以毫秒为单位），并作为输出信息的一部分。

### 5.程序优化

对基本DPLL的实现进行存储结构、分支变元选取策略1-3，等某一方面进行优化设计与实现，提供明确的性能优化率结果。优化率的计算公式为：[(t-to)/t]*100%,其中t 为未对DPLL优化时求解基准算例的执行时间，to则为优化DPLL实现时求解同一算例的执行时间

### 6.SAT应用：

将百分号数独游戏(%-Sudoku问题转化为SAT问题6-8，并集成到上面的求解器进行数独游戏求解，游戏可玩，具有一定的/简单交互性。应用问题归约为SAT问题的方法可参考文献3与6-8

6 Tjark Weber. A sat-based sudoku solver. In 12th International Conference on Logic for Programming, Artificial Intelligence and Reasoning, LPAR 2005:11–15

7 Ins Lynce and Jol Ouaknine. Sudoku as a sat problem. In Proceedings of the 9th International Symposium on Artificial Intelligence and Mathematics, AIMATH 2006.

8 Uwe Pfeiffer, Tomas Karnagel and Guido Scheffler. A Sudoku-Solver for Large Puzzles using SAT. LPAR-17-short (EPiC Series, vol. 13): 52–57

### 7.生成数独：

一般至少要有17个提示数，基于挖洞法生成9-11

9 Sudoku Puzzles Generating: from Easy to Evil.

http://zhangroup.aporc.org/images/files/Paper_3485.pdf

10  薛源海，蒋彪彬，李永卓. 基于“挖洞”思想的数独游戏生成算法. 数学的实践与认识,2009,39(21):1-7

11  黄祖贤. 数独游戏的问题生成及求解算法优化. 安徽工业大学学报(自然科学版), 2015,32(2):187-191

4 Carsten Sinz. Visualizing SAT Instances and Runs of the DPLL Algorithm. J Autom Reasoning, (2007) 39:219–243

## 模块划分

设计程序要求模块化，程序源代码进行模块化组织。主要模块包括如下：
1）主控、交互与显示模块（display）；
2）CNF解析模块（cnfparser）；
3）核心DPLL模块( solver)；
4）百分号数独模块，包括游戏格局生成、归约、求解(X-Sudoku)

## 函数设计

如子句有创建createClause、销毁destroyClause、增加addClause、删除removeClause、判断是否为单子句isUnitClause、评估子句的真假状evaluateClause等运算。由于每个CNF公式变元与子句数可能不同，同一个实例中子句长度也可能不等,一种基本的处理方式是将子句表示为由文字构成的链表；整个公式则是由子句构成的链表，如图2.6所示，这里仅供参考（也许并非最优结构），同学们可自行设计相应的物理存储结构并进行优化,有效支持回溯

## 输出要求

对每个算例的求解结果要求输出到一个与算例同名的文件（文件扩展名为.res），文件内容与格式要求如下：
s求解结果//1表示满足，0表示不满足，-1表示在限定时间内未完成求解
v -1 2 -3 … //满足时，每个变元的赋值序列，-1表示第一个变元1取假，2表示第二个变元取真，用空格分开，此处为示例。
t 17     //以毫秒为单位的DPLL执行时间，可增加分支规则执行次数信息

### 示例说明

123为自己设的简单例子

基准，其他用原名

abc为满足

ABC为不满足

目前L1584错认为错误
sud00009.cnf认为无解（不知道是否保真




