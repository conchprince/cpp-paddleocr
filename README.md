# Windows10 C++下部署PaddleOCR(gpu)完整过程
由于实习的任务,需要在c++下部署paddleocr,大致流程参考文档:[https://github.com/PaddlePaddle/PaddleOCR/blob/main/deploy/cpp_infer/docs/windows_vs2019_build.md] 由于平常一般都是用python,尽管很久以前学过c++但是也忘得差不多了,几乎可以算是纯新手,过程中遇到很多坑,故在此记录完整流程以便后续使用或其他小白们参考,写的会比较详细   
因为自己也是初次部署,怕出错所以按照官方文档的工作目录D:\projects\cpp来做的,如果你使用的其他工作目录,以下涉及到的很多路径需要自行修改

# 环境准备
文档中给出的环境是  
Visual Studio 2019  
CUDA 10.2，cudnn 7+ （仅在使用GPU版本的预测库时需要）  
CMake 3.22+  
实际使用时可以参考[https://www.paddlepaddle.org.cn/inference/master/guides/install/download_lib.html] 根据机器情况选择,我使用的Windows 10系统,Visual Studio 2022,并且此前为了用pytorch等使用的是cuda11.8,故选择此处第三个.如何准备这些环境网上有很多教程,此处就不在赘述(安装过程中踩的坑和解决方法全部基于本环境,如果想跟着本文档一次性部署好建议准备上述环境)
![QQ_1721281966173](https://github.com/user-attachments/assets/5bb2d1bd-f8d5-4185-a001-f99b66a24022)   
此外,还需要下载PaddlePaddleC++和OpenCV,其中PaddlePaddleC++在上面的链接里,下载好zip后解压到D:\projects下  

然后在[https://github.com/opencv/opencv/releases] 处下载OpenCV,下载windows的.exe文件后运行,将OpenCV解压至D:\projects\cpp  

最后下载PaddleOCR代码,直接在[https://github.com/PaddlePaddle/PaddleOCR/tree/main] code处下载zip后解压至D:\projects\cpp  

至此已经完成了环境准备,请确保你的文件结构如下:
```bash
D:\projects\
    ├── cpp
    |    ├──opencv
    |    └──PaddleOCR
    └── paddle_inference
```

# 开始运行
打开cmake-gui,按下图选择路径
![QQ_1721283285074](https://github.com/user-attachments/assets/b73ed7f3-94d5-4668-b0cc-b99355f4acc5)
点击下方的Configure,在弹出的界面中第一栏选择Visual Studio 17 2022,第二栏填x64,剩下的不用管点finish即可  
第一次运行会报错,接下来按如下图补充相关路径(如果你的路径跟图中的不一样请按实际填写)
![QQ_1721283602421](https://github.com/user-attachments/assets/a5d4c0ed-bb74-40cf-9981-b002eaf83d39)
需要补充的路径为:  
CUDA_LIB和CUDNN_LIB(二者相同),OPENCV_DIR和OpnCV_DIR(二者相同),PADDLE_LIB,勾选WITH_GPU  
然后依次点击Configure,Generate和Open Project即可  
此时已经在Visual Studio中打开了项目,在生成解决方案之前,先把Debug改成Release,然后下载[https://paddleocr.bj.bcebos.com/deploy/cpp_infer/cpp_files/dirent.h] 并复制到Visual Studio的include文件夹下(在我的电脑上路径为D:\vscode\VC\Auxiliary\VS\include)  
然后点击生成-生成解决方案,此时我遇到了setlocal报错,解决方法如下:  
在右侧的解决方案资源管理器中选中ppocr,然后在项目-属性-配置属性-生存事件-生成后事件-命令行,选中后点击右边的下拉栏,点击编辑后,删掉上方有关mkldnn的几行,最后保留的如图
![QQ_1721284514821](https://github.com/user-attachments/assets/d49d4cd2-80ca-4b02-8794-1284480936d8)
点击确定-应用后重新点生成-生成解决方案即可正常生成

之后.将需要的.dll文件复制进D:\projects\cpp\PaddleOCR\deploy\cpp_infer\build\Release路径下,注意




