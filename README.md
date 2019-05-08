# syntax-highlight
Brighten up the code
此程序须在Linux环境下运行，且需要在与该文件同目录下存在a.c的C语言程序代码，需要保证a.c文件的语法正确，则输出的结果是a.c代码经过语法高亮着色后的代码。<br>
其中代码主要由3各部分组成：<br>
1. read_file()读取同目录下的C语言文件到text字符串中<br>
2. init_regex(), make_token(), 匹配并记录text中的字符串<br>
3. syntax_light()根据不同的token类型显示出不同的颜色<br>
显示不同的颜色运用了ANSI转义码的颜色功能。
