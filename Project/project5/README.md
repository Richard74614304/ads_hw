## 编译和运行

!!! Note 编译
    在`project5`文件夹下使用`make`命令，生成`shopping.exe`文件
!!! Note 运行
    输入下面的命令：
    ```
    .\shopping.exe .\testdata\<name of the test file>

    # 比如:

    .\shopping.exe .\testdata\standard_input.txt
    ```
    由于本人不是特别会用`Windows`系统下的`makefile`，所以里面没有`clean`命令，需要手动删除冗余文件

---

## 测试文件说明

!!! Note 说明
    测试文件分为两类：
    **正确性验证：** 
    `standard_input`: 老师给出的范例
    预期结果：8 2
    `min_scale.txt`: 最小规模的数据
    预期结果：1 85
    `zero_budget`: 零预算
    预期结果：0 0
    `strategy_contradictory`: 价格递增但优惠券递减
    预期结果：21 4
    `one_purchase`: 预算 = 单次购买
    预期结果：1 0
    `no_purchase`: 预算 < 单次购买
    预期结果：0 5
    **算法运行速度测试：(这部分数据由DataGen.c生成)**
    `small`: N = 100, Budget = 10000
    测试结果：0.000132s
    `medium`: N = 10000, Budget = 10000_0000
    测试结果：1.649381s
    `huge`: 最大情况：N = 100000, Budget = 10_0000_0000
    测试结果：162.457435s