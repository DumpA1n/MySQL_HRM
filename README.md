# MySQL_HRM
## Build On Windows

[MySQL 下载](https://dev.mysql.com/downloads/installer/)
[Connector/C++ 下载](https://dev.mysql.com/downloads/connector/cpp/)

[MySQL 安装教程](https://blog.csdn.net/qq_59636442/article/details/123058454)
[Connector/C++ 安装教程](https://blog.csdn.net/m0_71622680/article/details/125351200)

[添加路径到系统变量教程](https://learn.microsoft.com/zh-cn/previous-versions/office/developer/sharepoint-2010/ee537574(v=office.14))

安装之后依次添加下列路径到系统变量

```
C:\Program Files\MySQL\MySQL Server 8.0\bin
C:\Program Files\MySQL\MySQL Connector C++ 9.0\bin
C:\Program Files\MySQL\MySQL Connector C++ 9.0\lib64
C:\Program Files\MySQL\MySQL Connector C++ 9.0\lib64\plugin
C:\Program Files\MySQL\MySQL Connector C++ 9.0\lib64\vs14
```

安装 VisualStudio 2022
修改 `build.bat` 22行 `vcvarsall.bat` 的路径
执行 `build.bat`