#include <iostream>
#include <fstream>
#include <regex>
#include <string>
#include <algorithm>
#include <filesystem>
#include <vector>
#include <chrono>
#include <opencv2/opencv.hpp>

// 计算wer
double wer(const std::string& word1, const std::string& word2) {
    int m = word1.length();
    int n = word2.length();
    if (m == 0) {
        return 0;
    }
    std::vector<int> prev(n + 1);
    std::vector<int> current(n + 1);

    for (int i = 0; i <= n; ++i) {
        prev[i] = i;
    }

    for (int i = 1; i <= m; ++i) {
        current[0] = i;
        for (int j = 1; j <= n; ++j) {
            if (word1[i - 1] == word2[j - 1]) {
                current[j] = prev[j - 1];
            }
            else {
                current[j] = std::min({ prev[j - 1], current[j - 1], prev[j] }) + 1;
            }
        }
        std::swap(prev, current);
    }

    return static_cast<double>(prev[n]) / m;
}

int main() {
    auto start_total = std::chrono::high_resolution_clock::now(); //记录开始时间
    double total_wer = 0;

    // 设置代码页为UTF-8
    std::string chcpCommand = "CHCP 65001";
    system(chcpCommand.c_str());
   
    std::ifstream labelFile("E:\\ocr\\data\\Label.txt"); // 打开标签文件

    // 读取标签文件到字典中
    std::unordered_map<std::string, std::string> labelDict;
    std::string line;
    while (std::getline(labelFile, line)) {
        std::string filename = line.substr(0, line.find('\t'));
        std::string label = line.substr(line.find('\t') + 1);
        labelDict[filename] = label;
    }
    labelFile.close();

    std::string imagesPath = "E:\\ocr\\data\\ai_img\\"; // 图像文件夹路径

    // 调用 OCR 模型，并获取输出
    std::string execCommand = "cd /d D:\\projects\\cpp\\PaddleOCR\\deploy\\cpp_infer && .\\build\\Release\\ppocr.exe system --det_model_dir=D:\\projects\\cpp\\PaddleOCR\\deploy\\cpp_infer\\inference\\ch_PP-OCRv4_det_infer --rec_model_dir=D:\\projects\\cpp\\PaddleOCR\\deploy\\cpp_infer\\inference\\ch_PP-OCRv4_rec_infer --image_dir=" + imagesPath;
    std::string ocrOutput = "";
    

    FILE* pipe = _popen(execCommand.c_str(), "r"); 
    char buffer[256];

    auto start = std::chrono::high_resolution_clock::now(); //记录模型推理开始时间
    // 执行命令并获取输出
    while (fgets(buffer, 256, pipe) != NULL) {
        ocrOutput += buffer;
    }
    _pclose(pipe);
    auto end = std::chrono::high_resolution_clock::now(); // 记录模型推理结束时间

    const int images_num = 338; // 测试集图片数量
    std::string ocr_results[images_num][2];

    int p = 0;
    for (int i = 0; i < images_num; i++) {
        int q = ocrOutput.find("predict", p); 
        q = ocrOutput.find("\n", q+63);  
        std::string key = ocrOutput.substr(q - 4, 4);  
        p = q;
        q -= 32;
        while (ocrOutput[q - 1] != '\\') q--;
        ocr_results[i][0] = ocrOutput.substr(q, p - q);
        int last = ocrOutput.find(key, p);
        while (1) {
            q = ocrOutput.find("rec text", q);
            if (q == ocrOutput.npos || q > last)    break;
            q += 10;
            p = ocrOutput.find("rec score", q) - 1;
            ocr_results[i][1] += ocrOutput.substr(q, p - q);
        }
        p = last;
    }

    for (int i = 0; i < images_num; i++) {
        // 获取label并拼接
        std::string labels = labelDict[ocr_results[i][0]];
        std::regex transcriptionRegex("\"transcription\":\\s*\"([^\"]+)\"");
        std::string label;
        auto label_begin = std::sregex_iterator(labels.begin(), labels.end(), transcriptionRegex);
        auto label_end = std::sregex_iterator();
        for (std::sregex_iterator i = label_begin; i != label_end; ++i) {
            std::smatch match = *i;
            std::string match_str = match.str(1);
            label += match_str;
        }
        total_wer += wer(label, ocr_results[i][1]);
    }

    auto end_total = std::chrono::high_resolution_clock::now(); // 记录结束时间
    std::chrono::duration<double> elapsed = end - start;
    std::chrono::duration<double> elapsed_total = end_total - start_total;
    total_wer /= images_num;
    double accuracy = (1 - total_wer) * 100;
    double pps = images_num / elapsed.count();
    std::cout << "wer:" << total_wer << std::endl;
    std::cout << "accuracy:" << accuracy << "%" << std::endl;
    std::cout << "inference_time: " << elapsed.count() << "s" << std::endl;
    std::cout << "total_time: " << elapsed_total.count() << "s" << std::endl;
    std::cout << "pps: " << pps << std::endl;

    return 0;
}