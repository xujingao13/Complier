# Simple compiler from C to JavaScript

## 使用说明
* linux mac 均可
* 提前确保安装了flex、bison及g++
* 在项目根目录下执行`make`生成编译器c2js
* 执行`./c2js filename.c`完成C到JavaScript的转换，输出结果为out.js

## LLVM分支
* C -> LLVM语言
* 实现了回文检测以及KMP，但难度有点高
* 详情进入分支看
