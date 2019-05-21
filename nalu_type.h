#pragma once

#include <string.h>

const char* nalu_type[32] = {	"未使用",
								"未使用Data Paritioning、非IDR图像的Slice",
								"使用Data Paritioning、且为Sice A",
								"使用Data Partitioning、且为Slice B",
								"使用Data Partitioning、且为Slice C",
								"IDR图像中的Slice",
								"补充增强信息单元(SEI)",
								"序列参数集(Sequence Parameter Set, SPS)",
								"图像参数集(Picture Parameter Set, PPS)",
								"分界符",
								"序列結束",
								"码流結束",
								"填充",
};
