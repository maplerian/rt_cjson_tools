# cJSON Tools

#### 1. 简介
------

用于 RT-Thread 的 cJSON工具库，主要提供 **解析JSON数据并根据JSON数据调用函数** 的功能

#### 1.1 目录结构

| 名称    | 说明                        |
| ------- | --------------------------- |
| src     | rt_cjson_tools 实现源码目录 |
| inc     | rt_cjson_tools 头文件目录   |
| example | 示例文件目录                |

#### 1.2 许可证

rt_cjson_tools package 遵循 Apachev2.0许可，详见 `LICENSE` 文件。

#### 1.3 依赖

- RT_Thread 3.1.3+



#### 2.  获取方式

------

###### 通过 git 克隆

```bash
git clone https://gitee.com/MapleRian/rt_cjson_tools.git
```



#### 3. 注意事项

##### 	cJSON 容易让人遗忘的点（释放内存）

- cJSON 函数返回类型是 char * 的，一定要记得使用 cJSON_free 释放内存
- cJSON 函数返回类型是 cJSON* 的，只需要用 cJSON_Delete 释放头节点即可

#### 4. 联系方式

- 维护：maplerian
- 主页：https://gitee.com/MapleRian/rt_cjson_tools

