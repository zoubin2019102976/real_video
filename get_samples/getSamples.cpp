#include <iostream>
#include <cstring>
#include <string>
#include <sys/types.h>
#include <vector>
#include <opencv4/opencv2/videoio.hpp>
#include <opencv4/opencv2/core.hpp>
#include <opencv4/opencv2/highgui.hpp>

#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

using namespace std;
using namespace cv;

//#define BUF_SIZE 300000
const char *ip_address = "192.168.191.4";
u_short port = 1234;
//u_char buffer[BUF_SIZE];

int main(int argc, char** argv){

	/*需要连接的服务器的地址*/
	struct sockaddr_in serv_addr;
	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = inet_addr(ip_address);
	serv_addr.sin_port = htons(port);

	/*捕获摄像头 opencv*/
	Mat src;

	VideoCapture captureCam;
	int deviceID = 0;
	int apiID = cv::CAP_ANY;
	captureCam.open(deviceID, apiID);

	if(!captureCam.isOpened()){
		cerr << "Error: Unable to open camera" << endl;
		return -1;
	}
	
	captureCam >> src;
	if(src.empty()){
		cerr << "ERROR! blank frame grabbed" << endl;
		return -2;
	}

	vector<u_char> data_encode;
	while(1){
		/*创建套接字*/
		int clnt_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		connect(clnt_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr));

		captureCam.read(src);
		
		/*test*/
		//cout << "test" << endl;
		
		if(src.empty()){
			cerr << "ERROR! blank frame grabbed" << endl;
			return -2;
		}

		imencode(".jpg", src, data_encode);

		int len_encode = data_encode.size();
		string len = to_string(len_encode);
		int length = len.length();

		for(int i = 0; i < 16 - length; ++i){
			len += " ";
		}
		write(clnt_sock, len.c_str(), strlen(len.c_str())); //边界

		char send_char[1] = {0};
		for(int i = 0; i < len_encode; ++i){
			send_char[0] = data_encode[i];
			write(clnt_sock, send_char, sizeof(send_char));
		}

		/*memcpy(buffer, &data_encode[0], len_encode);
		write(clnt_sock, buffer, len_encode);*/

		/*接受回传数据*/
		char bufRecv[100] = {0};
		read(clnt_sock, bufRecv, sizeof(bufRecv) -1);

		string recv = bufRecv;
		cout << recv << endl;
		close(clnt_sock);

		/*测试代码,获取图片详细信息*/
		/*
		cout << "Size: " << data_encode.size() << endl;
		cout << "Width: " << captureCam.get(CAP_PROP_FRAME_WIDTH) << endl;
		cout << "Height: " << captureCam.get(CAP_PROP_FRAME_HEIGHT) << endl;
		cout << "Channels: " << src.channels() << endl;
		cout << "Depth: " << src.depth() << endl;
		cout << "Is continuous? " << src.isContinuous() << endl;
		cout << "Mat row: " << src.rows << " ,cols: " << src.cols << endl << endl;
		imshow("Test", src);
		char c = (char)waitKey(5);
		if(c == 27) break;
		*/
	}

	cout << "Bye..." << endl;
	return 0;
}
