//#include <iostream>
//#include <opencv2/opencv.hpp>  
//#include<opencv2\highgui\highgui.hpp>
//#include<opencv2\imgproc\imgproc.hpp>
////#include <opencv2/xfeatures2d.hpp>
//
//
//using namespace cv; 
//using namespace std;
////using namespace xfeatures2d;
//
//
//int main() {
//
//	system("color 2E");
//	//载入图片
//	Mat src1 = imread("D:/Visual Studio2019(64bit)/Projects/Opencv/1.jpg", 1);
//	Mat src2 = imread("D:/Visual Studio2019(64bit)/Projects/Opencv/2.jpg", 1);
//
//	//显示原图
//	imshow("原图1", src1);
//	imshow("原图2", src2);
//
//	//定义变量
//	vector<KeyPoint> keypoints1, keypoints2;//定义检测的特征点存储容器
//	Mat descriptors1, descriptors2;//定义特征点描述信息为Mat类型
//	Mat result_img;//匹配结果图片
//
//	//创建sift特征检测器实例
//	//将SIFT可以换位SURF、ORB
//	Ptr<SIFT>detector = SIFT::create();
//	//提取特征点
//	detector->detect(src1, keypoints1, noArray());
//	detector->detect(src2, keypoints2, Mat());
//
//
//	//获取特征点的描述信息=>特征向量
//	detector->compute(src1, keypoints1, descriptors1);
//	detector->compute(src2, keypoints2, descriptors2);
//
//
//	//定义匹配器的实例化=>方法为暴力匹配法
//	Ptr<DescriptorMatcher> matcher = DescriptorMatcher::create(DescriptorMatcher::BRUTEFORCE);//create中的参数可以填string FlannBased等匹配方法
//
//	//第二种实例化方法
//	//BFMatcher matcher;
//
//	//进行暴力匹配
//	vector<DMatch> matches;
//
//	//第一个参数为queryDescription为目标，第二个参数为trainDescription模板
//	matcher->match(descriptors1, descriptors2, matches);
//
//
//	//限制特征点匹配数量=》只匹配前20个较好的特征点
//	int num = 20;
//	nth_element(matches.begin(), matches.begin() + num, matches.end());
//	//vector去除20以后的元素
//	matches.erase(matches.begin() + num, matches.end());
//
//
//	//输出关键点和匹配结果
//	//其中右侧图为trainDescription模板，左侧图为queryDescription目标
//	//左图中的点与右图中进行匹配对应
//	drawMatches(src1, keypoints1, src2, keypoints2, matches, result_img);
//	drawKeypoints(src1, keypoints1, src1);
//	drawKeypoints(src2, keypoints2, src2);
//
//	imshow("匹配结果", result_img);
//	imshow("特征点1", src1);
//	imshow("特征点2", src2);
//
//	waitKey(0);
//	system("pause");
//	return 0;
//}
