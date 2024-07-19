# Windows10 C++下部署PaddleOCR(gpu)完整过程
由于实习需要,故尝试在c++下部署paddleocr,以下是完整过程,大致流程参考官方文档:
[https://github.com/PaddlePaddle/PaddleOCR/blob/main/deploy/cpp_infer/docs/windows_vs2019_build.md](https://github.com/PaddlePaddle/PaddleOCR/blob/main/deploy/cpp_infer/docs/windows_vs2019_build.md)   
由于平常一般都是用python,尽管很久以前学过c++但是也忘得差不多了,几乎可以算是纯新手,过程中遇到很多坑,故在此记录完整过程以便后续使用或其他小白们参考,所以很多地方会写的比较详细   
因为自己也是初次部署,为了尽量避免出错,所以按照官方文档的工作目录D:\projects\cpp来做的,如果你使用的其他工作目录,以下涉及到的很多路径需要自行修改

## 环境准备
文档中给出的环境是  
Visual Studio 2019  
CUDA 10.2，cudnn 7+ （仅在使用GPU版本的预测库时需要）  
CMake 3.22+  
实际使用时可以参考[https://www.paddlepaddle.org.cn/inference/master/guides/install/download_lib.html](https://www.paddlepaddle.org.cn/inference/master/guides/install/download_lib.html)根据机器情况选择,我使用的Windows 10系统,Visual Studio 2022,并且此前为了用pytorch等使用的是cuda11.8,故选择此处第三个.如何准备这些环境网上有很多教程,此处就不在赘述(安装过程中踩的坑和解决方法全部基于本环境,如果想跟着本文档一次性部署好建议准备上述环境) 

![QQ_1721286450622](https://github.com/user-attachments/assets/ae2bf0b9-881c-4476-87bc-5921c54de160)

此外,还需要下载PaddlePaddleC++和OpenCV,其中PaddlePaddleC++在上面的链接里,下载好zip后解压至D:\projects  

然后在[https://github.com/opencv/opencv/releases](https://github.com/opencv/opencv/releases)处下载OpenCV,下载windows的.exe文件后运行,将OpenCV解压至D:\projects\cpp  

最后下载PaddleOCR代码,直接在[https://github.com/PaddlePaddle/PaddleOCR/tree/main](https://github.com/PaddlePaddle/PaddleOCR/tree/main)code处下载zip后解压至D:\projects\cpp  

至此已经完成了环境准备,请确保你的文件结构如下:
```bash
D:\projects\
    ├── cpp
    |    ├──opencv
    |    └──PaddleOCR
    └── paddle_inference
```

## 生成解决方案 

打开cmake-gui,按下图选择路径  

![QQ_1721286526047](https://github.com/user-attachments/assets/e1631d12-9264-437a-ac12-fcdf8398ce7f)

点击下方的Configure,在弹出的界面中第一栏选择Visual Studio 17 2022,第二栏填x64,剩下的不用管点finish即可  
第一次运行会报错,接下来按如下图补充相关路径(如果你的路径跟图中的不一样请按实际填写)

![QQ_1721286625135](https://github.com/user-attachments/assets/76590388-2a28-4633-996e-2946166c1a26)
注:需要补充的路径为:CUDA_LIB和CUDNN_LIB(二者相同),OPENCV_DIR和OpnCV_DIR(二者相同),PADDLE_LIB,勾选WITH_GPU

然后依次点击Configure,Generate和Open Project即可  
此时已经在Visual Studio中打开了项目,在生成解决方案之前,先把Debug改成Release,然后下载[https://paddleocr.bj.bcebos.com/deploy/cpp_infer/cpp_files/dirent.h](https://paddleocr.bj.bcebos.com/deploy/cpp_infer/cpp_files/dirent.h)并复制到Visual Studio的include文件夹下(在我的电脑上路径为D:\vscode\VC\Auxiliary\VS\include)  

点击生成-生成解决方案,此时我遇到了setlocal报错,解决方法如下:  
在右侧的解决方案资源管理器中选中ppocr,然后在项目-属性-配置属性-生存事件-生成后事件-命令行,选中后点击右边的下拉栏,点击编辑后,删掉上方有关mkldnn的几行,最后保留的如图

![QQ_1721286754440](https://github.com/user-attachments/assets/88116c6b-f060-4151-bc1d-a30802ee75c9)

点击确定-应用后重新点生成-生成解决方案即可正常生成解决方案

## 开始运行

首先,将运行所需的.dll文件复制进D:\projects\cpp\PaddleOCR\deploy\cpp_infer\build\Release路径下,官方文档中的好像不全,之后运行的时候会报错,以下是我实际使用时复制进去的所有.dll文件:  
D:\projects\paddle_inference\paddle\lib下的common.dll和paddle_inference.dll  
D:\projects\paddle_inference\third_party\install\mklml\lib下的libiomp5md.dll和mklml.dll  
D:\projects\paddle_inference\third_party\install\onednn\lib下的mkldnn.dll  
D:\projects\cpp\opencv\build\x64\vc16\bin下的opencv_world4100.dll  
最终Release文件夹下应有6个.dll文件和一个.exe文件

然后需要下载ppocr模型文件,在[https://github.com/PaddlePaddle/PaddleOCR](https://github.com/PaddlePaddle/PaddleOCR)往下翻到下图处,我下载的如图所示

<img width="495" alt="QQ_1721284951583" src="https://github.com/user-attachments/assets/8d2bf836-b66a-42e0-9a94-750fe000f463">

下载后在D:\projects\cpp\PaddleOCR\deploy\cpp_infer下新建一个inference文件夹并解压至该文件夹下  

打开cmd运行以下命令
```bash
cd /d D:\projects\cpp\PaddleOCR\deploy\cpp_infer
CHCP 65001
.\build\Release\ppocr.exe system --use_gpu=True --det_model_dir=D:\projects\cpp\PaddleOCR\deploy\cpp_infer\inference\ch_PP-OCRv4_det_infer --rec_model_dir=D:\projects\cpp\PaddleOCR\deploy\cpp_infer\inference\ch_PP-OCRv4_rec_infer --image_dir=E:\ocr\data\img\916726333727933677110_1.jpeg
```
注意修改自己的图片路径,这里是检测的单张图,而ppocr同时支持一次性识别一个文件夹下的所有图片,将image_dir后面的路径换成文件夹路径即可
此处我遇到了第二个报错,信息如下:
```bash
PreconditionNotMetError: The third-party dynamic library (cublas64_120.dll;cublas64_12.dll) that Paddle depends on is not configured correctly. (error code is 126)
  Suggestions:
  1. Check if the third-party dynamic library (e.g. CUDA, CUDNN) is installed correctly and its version is matched with paddlepaddle you installed.
  2. Configure third-party dynamic library environment variables as follows:
  - Linux: set LD_LIBRARY_PATH by `export LD_LIBRARY_PATH=...`
  - Windows: set PATH by `set PATH=XXX;%PATH%`
  - Mac: set  DYLD_LIBRARY_PATH by `export DYLD_LIBRARY_PATH=...` [Note: After Mac OS 10.11, using the DYLD_LIBRARY_PATH is impossible unless System Integrity Protection (SIP) is disabled.] (at C:\home\workspace\Paddle\paddle\phi\backends\dynload\dynamic_loader.cc:340)
```
解决方法为:进入C:\Program Files\NVIDIA GPU Computing Toolkit\CUDA\v11.8\bin(或你的cuda安装位置的对应路径),找到cublas64_11.dll,复制一份并且重命名为cublas64_12.dll即可解决

最后更新日期:2024.7.18
