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
//	//����ͼƬ
//	Mat src1 = imread("D:/Visual Studio2019(64bit)/Projects/Opencv/1.jpg", 1);
//	Mat src2 = imread("D:/Visual Studio2019(64bit)/Projects/Opencv/2.jpg", 1);
//
//	//��ʾԭͼ
//	imshow("ԭͼ1", src1);
//	imshow("ԭͼ2", src2);
//
//	//�������
//	vector<KeyPoint> keypoints1, keypoints2;//�������������洢����
//	Mat descriptors1, descriptors2;//����������������ϢΪMat����
//	Mat result_img;//ƥ����ͼƬ
//
//	//����sift���������ʵ��
//	//��SIFT���Ի�λSURF��ORB
//	Ptr<SIFT>detector = SIFT::create();
//	//��ȡ������
//	detector->detect(src1, keypoints1, noArray());
//	detector->detect(src2, keypoints2, Mat());
//
//
//	//��ȡ�������������Ϣ=>��������
//	detector->compute(src1, keypoints1, descriptors1);
//	detector->compute(src2, keypoints2, descriptors2);
//
//
//	//����ƥ������ʵ����=>����Ϊ����ƥ�䷨
//	Ptr<DescriptorMatcher> matcher = DescriptorMatcher::create(DescriptorMatcher::BRUTEFORCE);//create�еĲ���������string FlannBased��ƥ�䷽��
//
//	//�ڶ���ʵ��������
//	//BFMatcher matcher;
//
//	//���б���ƥ��
//	vector<DMatch> matches;
//
//	//��һ������ΪqueryDescriptionΪĿ�꣬�ڶ�������ΪtrainDescriptionģ��
//	matcher->match(descriptors1, descriptors2, matches);
//
//
//	//����������ƥ������=��ֻƥ��ǰ20���Ϻõ�������
//	int num = 20;
//	nth_element(matches.begin(), matches.begin() + num, matches.end());
//	//vectorȥ��20�Ժ��Ԫ��
//	matches.erase(matches.begin() + num, matches.end());
//
//
//	//����ؼ����ƥ����
//	//�����Ҳ�ͼΪtrainDescriptionģ�壬���ͼΪqueryDescriptionĿ��
//	//��ͼ�еĵ�����ͼ�н���ƥ���Ӧ
//	drawMatches(src1, keypoints1, src2, keypoints2, matches, result_img);
//	drawKeypoints(src1, keypoints1, src1);
//	drawKeypoints(src2, keypoints2, src2);
//
//	imshow("ƥ����", result_img);
//	imshow("������1", src1);
//	imshow("������2", src2);
//
//	waitKey(0);
//	system("pause");
//	return 0;
//}
