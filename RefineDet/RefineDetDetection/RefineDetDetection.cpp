/*
 * Copyright(C) 2021. Huawei Technologies Co.,Ltd. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "opencv2/opencv.hpp"
#include "RefineDetDetection.h"
#include "MxBase/DeviceManager/DeviceManager.h"
#include "MxBase/Log/Log.h"
#include <unistd.h>
#include <sys/stat.h>
#include "boost/filesystem.hpp"
#include <boost/filesystem.hpp>
namespace fs = boost::filesystem;
using namespace MxBase;

namespace {
    const uint32_t YUV_BYTE_NU = 3;
    const uint32_t YUV_BYTE_DE = 2;
    const uint32_t VPC_H_ALIGN = 2;
    const uint32_t IMAGE_SIZE_HEIGHT = 576;
    const uint32_t IMAGE_SIZE_WIDTH = 768;
    const uint32_t PRE_WIDTH = 576;
    const uint32_t PRE_HEIGHT = 576;
    const int IS_TEST = 0;
    const int TEST_NUM = 9963;
    const int TEST_NAME_LEN = 6;
}

// 加载标签文件
APP_ERROR RefineDetDetection::LoadLabels(const std::string &labelPath, std::map<int, std::string> &labelMap) {
    std::ifstream infile;
    // open label file
    infile.open(labelPath, std::ios_base::in);
    std::string s;
    // check label file validity
    if (infile.fail()) {
        LogError << "Failed to open label file: " << labelPath << ".";
        return APP_ERR_COMM_OPEN_FAIL;
    }
    labelMap.clear();
    // construct label map
    int count = 0;
    while (std::getline(infile, s)) {
        if (s.find('#') <= 1) {
            continue;
        }
        size_t eraseIndex = s.find_last_not_of("\r\n\t");
        if (eraseIndex != std::string::npos) {
            s.erase(eraseIndex + 1, s.size() - eraseIndex);
        }
        labelMap.insert(std::pair<int, std::string>(count, s));
        count++;
    }
    infile.close();
    return APP_ERR_OK;
}

// 设置配置参数
void RefineDetDetection::SetRefineDetPostProcessConfig(const InitParam &initParam,
                                                       std::map<std::string, std::shared_ptr<void>> &config) {
    MxBase::ConfigData configData;
    const std::string checkTensor = initParam.checkTensor ? "true" : "false";
    configData.SetJsonValue("CLASS_NUM", std::to_string(initParam.classNum));
    configData.SetJsonValue("BIASES_NUM", std::to_string(initParam.biasesNum));
    configData.SetJsonValue("BIASES", initParam.biases);
    configData.SetJsonValue("OBJECTNESS_THRESH", initParam.objectnessThresh);
    configData.SetJsonValue("IOU_THRESH", initParam.iouThresh);
    configData.SetJsonValue("SCORE_THRESH", initParam.scoreThresh);
    configData.SetJsonValue("YOLO_TYPE", std::to_string(initParam.yoloType));
    configData.SetJsonValue("MODEL_TYPE", std::to_string(initParam.modelType));
    configData.SetJsonValue("INPUT_TYPE", std::to_string(initParam.inputType));
    configData.SetJsonValue("ANCHOR_DIM", std::to_string(initParam.anchorDim));
    configData.SetJsonValue("CHECK_MODEL", checkTensor);

    auto jsonStr = configData.GetCfgJson().serialize();
    config["postProcessConfigContent"] = std::make_shared<std::string>(jsonStr);
    config["labelPath"] = std::make_shared<std::string>(initParam.labelPath);
}

APP_ERROR RefineDetDetection::Init(const InitParam &initParam) {
    deviceId_ = initParam.deviceId;
    APP_ERROR ret = MxBase::DeviceManager::GetInstance()->InitDevices();
    if (ret != APP_ERR_OK) {
        LogError << "Init devices failed, ret=" << ret << ".";
        return ret;
    }
    ret = MxBase::TensorContext::GetInstance()->SetContext(initParam.deviceId);
    if (ret != APP_ERR_OK) {
        LogError << "Set context failed, ret=" << ret << ".";
        return ret;
    }
    dvppWrapper_ = std::make_shared<MxBase::DvppWrapper>();
    ret = dvppWrapper_->Init();
    if (ret != APP_ERR_OK) {
        LogError << "DvppWrapper init failed, ret=" << ret << ".";
        return ret;
    }
    model_ = std::make_shared<MxBase::ModelInferenceProcessor>();
    ret = model_->Init(initParam.modelPath, modelDesc_);
    if (ret != APP_ERR_OK) {
        LogError << "ModelInferenceProcessor init failed, ret=" << ret << ".";
        return ret;
    }

    std::map<std::string, std::shared_ptr<void>> config;
    SetRefineDetPostProcessConfig(initParam, config);
    //  初始化RefineDet后处理对象
    post_ = std::make_shared<RefineDetPostProcess>();
    ret = post_->Init(config);
    if (ret != APP_ERR_OK) {
        LogError << "RefineDetPostProcess init failed, ret=" << ret << ".";
        return ret;
    }
    // load labels from file
    ret = LoadLabels(initParam.labelPath, labelMap_);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to load labels, ret=" << ret << ".";
        return ret;
    }
    return APP_ERR_OK;
}

APP_ERROR RefineDetDetection::DeInit() {
    dvppWrapper_->DeInit();
    model_->DeInit();
    post_->DeInit();
    MxBase::DeviceManager::GetInstance()->DestroyDevices();
    return APP_ERR_OK;
}

// 获取图像数据，将数据存入TensorBase中
APP_ERROR RefineDetDetection::ReadImage(const std::string &imgPath, MxBase::TensorBase &tensor) {
    MxBase::DvppDataInfo inputDataInfo = {};
    MxBase::DvppDataInfo output = {};
    std::ifstream file(imgPath, std::ios::binary);
    if (!file) {
        LogError <<"Invalid file.";
    }
    long fileSize = fs::file_size(imgPath);
    std::vector<char> buffer;
    buffer.resize(fileSize);
    file.read(buffer.data(), fileSize);
    file.close();
    std::string fileStr(buffer.data(), fileSize);
    MxBase::MemoryData hostMemory((void*)fileStr.c_str(), (size_t)fileStr.size(), MemoryData::MEMORY_HOST, 0);
    MxBase::MemoryData dvppMemory(nullptr, (size_t)fileStr.size(), MemoryData::MEMORY_DVPP, 0);
    APP_ERROR ret = MemoryHelper::MxbsMallocAndCopy(dvppMemory, hostMemory);
    // 将数据转为到DEVICE侧，以便后续处理
    ret = dvppWrapper_->DvppJpegPredictDecSize(hostMemory.ptrData, hostMemory.size, inputDataInfo.format, output.dataSize);
    inputDataInfo.dataSize = dvppMemory.size;
    inputDataInfo.data=(uint8_t *)dvppMemory.ptrData;
    ret = dvppWrapper_->DvppJpegDecode(inputDataInfo, output);
    ret = MemoryHelper::Free(dvppMemory);

    MxBase::MemoryData memoryData((void*)output.data, output.dataSize,
                                MxBase::MemoryData::MemoryType::MEMORY_DEVICE, deviceId_);
    // 对解码后图像对齐尺寸进行判定
    if (output.heightStride % VPC_H_ALIGN != 0) {
        LogError << "Output data height(" << output.heightStride << ") can't be divided by " << VPC_H_ALIGN << ".";
        MxBase::MemoryHelper::MxbsFree(memoryData);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    std::vector<uint32_t> shape = {output.heightStride * YUV_BYTE_NU / YUV_BYTE_DE, output.widthStride};
    tensor = MxBase::TensorBase(memoryData, false, shape, MxBase::TENSOR_DTYPE_UINT8);
    return APP_ERR_OK;
}

APP_ERROR RefineDetDetection::Resize(const MxBase::TensorBase &inputTensor, MxBase::TensorBase &outputTensor) {
    auto shape = inputTensor.GetShape();
    MxBase::DvppDataInfo input = {};
    // 还原为原始尺寸
    input.height = (uint32_t)shape[0] * YUV_BYTE_DE / YUV_BYTE_NU;
    input.width = shape[1];
    input.heightStride = (uint32_t)shape[0] * YUV_BYTE_DE / YUV_BYTE_NU;
    input.widthStride = shape[1];
    input.dataSize = inputTensor.GetByteSize();
    input.data = (uint8_t*)inputTensor.GetBuffer();
    const uint32_t resizeHeight = IMAGE_SIZE_HEIGHT;
    const uint32_t resizeWidth = IMAGE_SIZE_WIDTH;
    MxBase::ResizeConfig resize = {};
    resize.height = resizeHeight;
    resize.width = resizeWidth;
    MxBase::DvppDataInfo output = {};
    // 图像缩放
    APP_ERROR ret = dvppWrapper_->VpcResize(input, output, resize);
    if (ret != APP_ERR_OK) {
        LogError << "VpcResize failed, ret=" << ret << ".";
        return ret;
    }
    MxBase::MemoryData memoryData((void*)output.data, output.dataSize,
                                MxBase::MemoryData::MemoryType::MEMORY_DEVICE, deviceId_);
    // 对缩放后图像对齐尺寸进行判定
    if (output.heightStride % VPC_H_ALIGN != 0) {
        LogError << "Output data height(" << output.heightStride << ") can't be divided by " << VPC_H_ALIGN << ".";
        MxBase::MemoryHelper::MxbsFree(memoryData);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    shape = {output.heightStride * YUV_BYTE_NU / YUV_BYTE_DE, output.widthStride};
    outputTensor = MxBase::TensorBase(memoryData, false, shape, MxBase::TENSOR_DTYPE_UINT8);
    return APP_ERR_OK;
}

// 模型推理
APP_ERROR RefineDetDetection::Inference(const std::vector<MxBase::TensorBase> &inputs,
                                        std::vector<MxBase::TensorBase> &outputs) {
    auto dtypes = model_->GetOutputDataType();
    for (size_t i = 0; i < modelDesc_.outputTensors.size(); ++i) {
        std::vector<uint32_t> shape = {};
        for (size_t j = 0; j < modelDesc_.outputTensors[i].tensorDims.size(); ++j) {
            shape.push_back((uint32_t)modelDesc_.outputTensors[i].tensorDims[j]);
        }
        MxBase::TensorBase tensor(shape, dtypes[i], MxBase::MemoryData::MemoryType::MEMORY_DEVICE, deviceId_);
        APP_ERROR ret = MxBase::TensorBase::TensorBaseMalloc(tensor);
        if (ret != APP_ERR_OK) {
            LogError << "TensorBaseMalloc failed, ret=" << ret << ".";
            return ret;
        }
        // 将tensor存入outputs中
        outputs.push_back(tensor);
    }
    MxBase::DynamicInfo dynamicInfo = {};
    // 设置类型为静态batch
    dynamicInfo.dynamicType = MxBase::DynamicType::STATIC_BATCH;
    APP_ERROR ret = model_->ModelInference(inputs, outputs, dynamicInfo);
    if (ret != APP_ERR_OK) {
        LogError << "ModelInference failed, ret=" << ret << ".";
        return ret;
    }
    return APP_ERR_OK;
}

// 后处理
APP_ERROR RefineDetDetection::PostProcess(const MxBase::TensorBase &tensor,
                                          const std::vector<MxBase::TensorBase> &outputs,
                                          std::vector<std::vector<MxBase::ObjectInfo>> &objInfos)
{
    // 通过原始图像tensor构建ResizedImageInfo
    auto shape = tensor.GetShape();
    MxBase::ResizedImageInfo imgInfo;
    imgInfo.widthOriginal = shape[1];
    imgInfo.heightOriginal = shape[0] * YUV_BYTE_DE / YUV_BYTE_NU;
    imgInfo.widthResize = IMAGE_SIZE_HEIGHT;
    imgInfo.heightResize = IMAGE_SIZE_WIDTH;
    imgInfo.resizeType = MxBase::RESIZER_STRETCHING;
    std::vector<MxBase::ResizedImageInfo> imageInfoVec = {};
    imageInfoVec.push_back(imgInfo);

    APP_ERROR ret = post_->Process(outputs, objInfos, imageInfoVec);
    if (ret != APP_ERR_OK) {
        LogError << "Process failed, ret=" << ret << ".";
        return ret;
    }

    ret = post_->DeInit();
    if (ret != APP_ERR_OK) {
        LogError << "RefineDetPostProcess DeInit failed";
        return ret;
    }
    return APP_ERR_OK;
}

APP_ERROR RefineDetDetection::WriteResult(MxBase::TensorBase &tensor,
                                          const std::vector<std::vector<MxBase::ObjectInfo>> &objInfos,
                                          FILE* fp, int imageId, cv::Mat originImage)
{
    APP_ERROR ret = tensor.ToHost();
    if (ret != APP_ERR_OK) {
        LogError << "ToHost faile";
        return ret;
    }
    auto shape = tensor.GetShape();
    cv::Mat imgBgr = originImage.clone();

    uint32_t batchSize = objInfos.size();
    std::vector<MxBase::ObjectInfo> resultInfo;
    for (uint32_t i = 0; i < batchSize; i++) {
        float maxConfidence = 0;
        uint32_t  index;
        for (uint32_t j = 0; j < objInfos[i].size(); j++) {
            if (objInfos[i][j].confidence > maxConfidence) {
                maxConfidence = objInfos[i][j].confidence;
                index = j;
            }
            index = j;

            resultInfo.push_back(objInfos[i][index]);

            // 打印置信度最大推理结果
            int k = resultInfo.size() - 1;
            LogInfo << "id: " << resultInfo[k].classId << "; label: " << resultInfo[k].className
                    << "; confidence: " << resultInfo[k].confidence
                    << "; box: [ (" << resultInfo[k].x0 << "," << resultInfo[k].y0 << ") "
                    << "(" << resultInfo[k].x1 << "," << resultInfo[k].y1 << ") ]" ;

            const cv::Scalar green = cv::Scalar(0, 255, 0);
            const cv::Scalar black = cv::Scalar(0, 0, 0);
            const uint32_t thickness = 1;
            const uint32_t lineType = 8;
            const float fontScale = 0.3333;
            int id = resultInfo[k].classId;
            int conf = int(resultInfo[k].confidence*100);
            int baseline = 0;
            const int offset1 = 15;
            const int offset2 = 3;
            const int offset3 = 2;

            const uint32_t fontFace = cv::FONT_HERSHEY_SCRIPT_COMPLEX;
            cv::Point2i c1((int)resultInfo[k].x0, (int)resultInfo[k].y0);
            cv::Point2i c2((int)resultInfo[k].x1, (int)resultInfo[k].y1);
            cv::Size sSize = cv::getTextSize(std::to_string(conf) + "%", fontFace,
                                             fontScale, thickness, &baseline);
            cv::Size textSize = cv::getTextSize(labelMap_[((int)resultInfo[k].classId)], fontFace,
                                                fontScale, thickness, &baseline);
            cv::rectangle(imgBgr, c1, cv::Point(c1.x + textSize.width + offset1 + sSize.width,
                                                c1.y - textSize.height - offset2),
                          green, -1);
            // 在图像上绘制文字
            cv::putText(imgBgr, labelMap_[((int)resultInfo[k].classId)] + ": " + std::to_string(conf) + "%",
                        cv::Point(resultInfo[k].x0, resultInfo[k].y0 - offset3),
                        cv::FONT_HERSHEY_SIMPLEX, fontScale, black, thickness,
                        lineType);
            // 绘制矩形
            cv::rectangle(imgBgr, cv::Rect(resultInfo[k].x0, resultInfo[k].y0,
                                           resultInfo[k].x1 - resultInfo[k].x0,
                                           resultInfo[k].y1 - resultInfo[k].y0),
                          green, thickness);
            std::cout << resultInfo[k].classId << " " << resultInfo[k].confidence
                    << " " << resultInfo[k].x0 << " " << resultInfo[k].y0
                    << " " << resultInfo[k].x1 << " " << resultInfo[k].y1 << "\n" ;

            if (IS_TEST)
            {
                fprintf(fp, "{'bbox': [%.3f, %.3f, %.3f, %.3f], 'score': %.6f, 'image_id': %d, 'category_id': %d}",
                    resultInfo[k].x0, resultInfo[k].y0, resultInfo[k].x1, resultInfo[k].y1,
                    resultInfo[k].confidence, imageId, id);
                if (imageId != TEST_NUM || i != batchSize-1 || j != objInfos[i].size()-1)
                    fprintf(fp, ",");
            }
        }
    }
    // 把Mat类型的图像矩阵保存为图像到指定位置。
    cv::imwrite("./result.jpg", imgBgr);
    return APP_ERR_OK;
}

APP_ERROR RefineDetDetection::BasicProcess(const std::string &imgPath, MxBase::TensorBase &inTensor,
                                           std::vector<std::vector<MxBase::ObjectInfo>> &objInfos,
                                           cv::Mat &img1, cv::Mat &img2)
{
    APP_ERROR ret = ReadImage(imgPath, inTensor);
    if (ret != APP_ERR_OK) {
        LogError << "ReadImage failed, ret=" << ret << ".";
        return ret;
    }

    MxBase::TensorBase outTensor;
    ret = Resize(inTensor, outTensor);
    if (ret != APP_ERR_OK) {
        LogError << "Resize failed, ret=" << ret << ".";
        return ret;
    }

    std::vector<MxBase::TensorBase> inputs = {};
    std::vector<MxBase::TensorBase> outputs = {};
    inputs.push_back(outTensor);

    ret = Inference(inputs, outputs);
    if (ret != APP_ERR_OK) {
        LogError << "Inference failed, ret=" << ret << ".";
        return ret;
    }

    ret = PostProcess(inTensor, outputs, objInfos);
    if (ret != APP_ERR_OK) {
        LogError << "PostProcess failed, ret=" << ret << ".";
        return ret;
    }
    
    for (uint32_t j = 0; j < objInfos.size(); j++)
        for (uint32_t k = 0; k < objInfos[j].size(); k++)
        {
            objInfos[j][k].x0 *= 1.0 * img1.size().width / img2.size().width;
            objInfos[j][k].x1 *= 1.0 * img1.size().width / img2.size().width;
            objInfos[j][k].y0 *= 1.0 * img1.size().height / img2.size().height;
            objInfos[j][k].y1 *= 1.0 * img1.size().height / img2.size().height;
        }

    return APP_ERR_OK;
}

APP_ERROR RefineDetDetection::Process(const std::string &imgPath) {
    if (IS_TEST) {
        FILE *fp = fopen("test.json", "w+");
        fprintf(fp, "[");
        for (int i = 1; i <= TEST_NUM; i++)
        {
            MxBase::TensorBase inTensor;
            std::string image_num = "000000";
            image_num = (image_num + std::to_string(i));
            image_num = image_num.substr(image_num.size()-TEST_NAME_LEN, image_num.size());
            std::string newPath = "./VOCdevkit/VOC2007/JPEGImages/" + image_num + ".jpg";
            cv::Mat img1 = cv::imread(newPath), img2;
            cv::resize(img1, img2, cv::Size(PRE_WIDTH, PRE_HEIGHT), 0, 0);
            newPath = "./inferImage.jpg";
            cv::imwrite(newPath, img2);
        
            std::vector<std::vector<MxBase::ObjectInfo>> objInfos;
            APP_ERROR ret = BasicProcess(newPath, inTensor, objInfos, img1, img2);
            ret = WriteResult(inTensor, objInfos, fp, i, img1);
            if (ret != APP_ERR_OK) {
                LogError << "Save result failed, ret=" << ret << ".";
                return ret;
            }
        }
        fprintf(fp, "]");
        fclose(fp);
        LogInfo << "Generate test.json successfully.";
        return APP_ERR_OK;
    }
    else {
        MxBase::TensorBase inTensor;
        cv::Mat img1 = cv::imread(imgPath), img2;
        cv::resize(img1, img2, cv::Size(PRE_WIDTH, PRE_HEIGHT), 0, 0);
        std::string newPath = "./inferImage.jpg";
        cv::imwrite(newPath, img2);

        std::vector<std::vector<MxBase::ObjectInfo>> objInfos;
        APP_ERROR ret = BasicProcess(newPath, inTensor, objInfos, img1, img2);
        ret = WriteResult(inTensor, objInfos, NULL, 0, img1);
        if (ret != APP_ERR_OK) {
            LogError << "Save result failed, ret=" << ret << ".";
            return ret;
        }
        return APP_ERR_OK;
    }
}