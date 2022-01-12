## numDL

### 1.前言

之前它的名字叫`matrixCpp`，为什么这么叫呢？因为里面有很多矩阵运算，而且语言也是cpp写的，所以就这么叫了。

其实我们参考的是`numPy`，本来想叫`numCPP`的，结果已经有库这么叫了。

现在，我又想到了一个好名字：`numDL`，为什么这么叫呢？首先，我们参考的是`numPy`，所以取`num`，其次，我们主要将它用于深度学习，嵌入式人工智能，且会用到相关的库，比如：`cmsisNN`。因此，我觉得：`numDL`更好。

我希望，`numDL`，她一定是相对操作系统独立的个体。放在哪里都能用才好。

1. 文件前缀为`nd`
2. 宏使用`NUMDL`



### 2.之前

这里面的代码均采用很多一个内部项目`UAI`的源码，在此基础上进行改动。

```
├─common
├─feature  
├─library                       //神经网络库
│  ├─CMSIS                      //目前只有cmsis，以后会支持nmsis
│  └─kissfft                    //简单高效的fft库
└─test                          //用于测试的代码
```

可见：之前就有该四个目录：

1. `common`是矩阵创建和销毁，以及`base64`
2. `feature`也是相关`numPy`的一些接口实现
3. `library`则是神经网络加速运算的库
4. `test`则是相关的测试用例

现在新增了一个目录：`numdl`，用于新增代码的放置

### 3.列出接口

#### 3.1 uai_dsp.cc

| numPy | numCpp | numDL                                                        | 类型               |
| ----- | ------ | ------------------------------------------------------------ | ------------------ |
|       |        | static float sum(float*     input, <br/>                            os_size_t size); | 矩阵求和           |
|       |        | static int dot(uai_mat_t* mat1, <br/>                       uai_mat_t* mat2, <br/>                       uai_mat_t* output); | 数学函数：点积     |
|       |        | static int dot_by_row(os_size_t mat1_row,<br/>                          float* mat1_row_data,<br/>                          os_size_t mat1_cols,<br/>                          uai_mat_t* mat2,<br/>                          uai_mat_t* output); | 数学函数：点积     |
|       |        | static int int16_to_float(const os_int16_t* src,<br/>                              float* dst,<br/>                              os_size_t size); |                    |
|       |        | static float log(float a);                                   | 数学函数：log      |
|       |        | static float log2(float a);                                  | 数学函数：log2     |
|       |        | static float log10(float a);                                 | 数学函数：log10    |
|       |        | static int log(uai_mat_t* matrix);                           | 数学函数：log      |
|       |        | static int log10(uai_mat_t* matrix);                         | 数学函数：log10    |
|       |        | static int linspace(float start, float stop, <br/>                               os_size_t num, float* out); | 初始化：序列生成器 |
|       |        | static int linspace(os_int16_t start,<br/>                        os_int16_t stop,<br/>                        os_size_t num,<br/>                        os_int16_t* out); | 初始化：序列生成器 |
|       |        | static int linspace(os_int32_t start,<br/>                        os_int32_t stop,<br/>                        os_size_t num,<br/>                        os_int32_t* out); | 初始化：序列生成器 |
|       |        | static int rfft(const float* src,<br/>                    os_size_t src_len,<br/>                    float* out,<br/>                    os_size_t out_len,<br/>                    os_size_t fft_len); | 无                 |
|       |        | static int dct2(uai_mat_t* mat, dct_norm_t mode);            | 无                 |

#### 3.2 uai_matrix.c

| numPy | numCpp | numDL                                                      |
| ----- | ------ | ---------------------------------------------------------- |
|       |        | uai_mat_t* uai_mat_create(os_size_t rows, os_size_t cols); |
|       |        | void uai_mat_destroy(uai_mat_t* mat);                      |
|       |        |                                                            |

### 4.numCpp

1. 矩阵的初始化
2. 矩阵的切片
3. 生成随机数组
4. 矩阵的连接
5. 举证的翻转，变形！
6. 矩阵的逻辑和比较，排序运算
7. 数学函数！

numCpp有个特色，就是全是hpp文件，全是头文件，直接可以用。有点意思哦。

模板template让它的代码变得简洁。

```
#include "NumCpp/Coordinates.hpp"    //坐标
#include "NumCpp/Core.hpp"           //基础功能！
#include "NumCpp/Filter.hpp"         //过滤器
#include "NumCpp/Functions.hpp"      //数学函数！
#include "NumCpp/ImageProcessing.hpp"    //图像处理
#include "NumCpp/Integrate.hpp"      //积分
#include "NumCpp/Linalg.hpp"         //线性代数
#include "NumCpp/NdArray.hpp"        //常用！
#include "NumCpp/Polynomial.hpp"     //多项式
#include "NumCpp/PythonInterface.hpp"     //python接口
#include "NumCpp/Random.hpp"         //随机值
#include "NumCpp/Roots.hpp"          //求根法
#include "NumCpp/Rotations.hpp"      //矩阵旋转
#include "NumCpp/Special.hpp"        //特殊函数
#include "NumCpp/Utils.hpp"			//基础功能！
#include "NumCpp/Vector.hpp"        //向量函数！
```

#### 4.1 先做啥？

从上面的细分来说：先做基础功能，数学函数，常用。

```
#include "NumCpp/Core.hpp"           //基础功能！
#include "NumCpp/NdArray.hpp"        //常用！
#include "NumCpp/Utils.hpp"			//基础功能！
#include "NumCpp/Functions.hpp"      //数学函数！
```

其中common目录实现了：

1. 断言，统一使用通用的printf，不和某一rtos绑定
2. 错误码
3. 打印日志信息，统一使用通用的printf，不和某一rtos绑定
4. 版本信息

































