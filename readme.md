## numDL

### 1.前言

现在它的名字叫`matrixCpp`，为什么这么叫呢？因为里面有很多矩阵运算，而且语言也是cpp写的，所以就这么叫了。

其实我们参考的是`numPy`，本来想叫`numCPP`的，结果已经有库这么叫了。

现在，我又想到了一个好名字：`numDL`，为什么这么叫呢？首先，我们参考的是`numPy`，所以取`num`，其次，我们主要将它用于深度学习，嵌入式人工智能，且会用到相关的库，比如：`cmsisNN`。因此，我举得：`numDL`更好。

### 2.之前

这里面的代码均采用很多一个内部项目`UAI`的源码，在此基础上进行改动。

```
├─common
│      uai_matrix.c             //uai的矩阵创建和销毁
│      uai_utils.c              //base64
├─feature
│      uai_audio.cc
│      uai_dsp.cc               //现在就做一点dsp他模仿的是numpy，把它C化！
│      uai_functions.cc
│      uai_processing.cc
│      
├─library                       //神经网络库
│  ├─CMSIS                      //目前只有cmsis，以后会支持nmsis
│  │  ├─Core
│  │  ├─DSP
│  │  └─NN
│  └─kissfft                    //简单高效的fft库
│
└─test                          //用于测试的代码
    ├─common
    │      uai_matrix_tc.c
    └─feature
        │  uai_audio_tc.cc
        │  uai_dsp_tc.cc
        │  uai_functions_tc.cc
        │  uai_processing_tc.cc
        └─testdata
```

可见：之前就有该四个目录：

1. `common`是矩阵创建和销毁，以及`base64`
2. `feature`也是相关`numPy`的一些接口实现
3. `library`则是神经网络加速运算的库
4. `test`则是相关的测试用例

现在新增了一个目录：`numdl`，用于新增代码的放置

### 3.列出接口

#### 3.1 uai_dsp.cc

| numPy | numCpp | numDL                                                        |
| ----- | ------ | ------------------------------------------------------------ |
|       |        | static float sum(float*     input, <br/>                            os_size_t size); |
|       |        | static int dot(uai_mat_t* mat1, <br/>                       uai_mat_t* mat2, <br/>                       uai_mat_t* output); |
|       |        | static int dot_by_row(os_size_t mat1_row,<br/>                          float* mat1_row_data,<br/>                          os_size_t mat1_cols,<br/>                          uai_mat_t* mat2,<br/>                          uai_mat_t* output); |
|       |        | static int int16_to_float(const os_int16_t* src,<br/>                              float* dst,<br/>                              os_size_t size); |
|       |        | static float log(float a);                                   |
|       |        | static float log2(float a);                                  |
|       |        | static float log10(float a);                                 |
|       |        | static int log(uai_mat_t* matrix);                           |
|       |        | static int log10(uai_mat_t* matrix);                         |
|       |        | static int linspace(float start, float stop, <br/>                               os_size_t num, float* out); |
|       |        | static int linspace(os_int16_t start,<br/>                        os_int16_t stop,<br/>                        os_size_t num,<br/>                        os_int16_t* out); |
|       |        | static int linspace(os_int32_t start,<br/>                        os_int32_t stop,<br/>                        os_size_t num,<br/>                        os_int32_t* out); |
|       |        | static int rfft(const float* src,<br/>                    os_size_t src_len,<br/>                    float* out,<br/>                    os_size_t out_len,<br/>                    os_size_t fft_len); |
|       |        | static int dct2(uai_mat_t* mat, dct_norm_t mode);            |

#### 3.2 uai_matrix.c

| numPy | numCpp | numDL                                                      |
| ----- | ------ | ---------------------------------------------------------- |
|       |        | uai_mat_t* uai_mat_create(os_size_t rows, os_size_t cols); |
|       |        | void uai_mat_destroy(uai_mat_t* mat);                      |
|       |        |                                                            |























