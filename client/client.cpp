#include <iostream>
#include <cstring>
#include <sys/types.h>
#include <vector>

#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

using namespace std;
using namespace cv;


const char* server_ip_addr = "192.168.191.4";
ushort port = 2233;

int main(int argc, char** argv){
	/*
	 *socket
	 * */
	struct sockaddr_in serv_addr;
	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = inet_addr(server_ip_addr);
	serv_addr.sin_port = htons(port);
	/**/

	/*
	 *opencv2
	 * */
	const string WIN_RECV = "RECV_BACK";
	namedWindow(WIN_RECV, WINDOW_AUTOSIZE);

	char recvBuf[16];
	char recvBuf_1[1];
	Mat img_decode;
	vector<uchar> data;

	while(1){
		int clnt_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		connect(clnt_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
		read(clnt_sock, recvBuf, 16);
		for(int i = 0; i < 16; ++i){
			if(recvBuf[i] < '0' || recvBuf[i] > '9'){
				recvBuf[i] = ' ';
			}
		}
		data.resize(std::atoi(recvBuf));
		for(int i = 0; i < std::atoi(recvBuf); ++i){
			read(clnt_sock, recvBuf_1, 1);
			data[i] = recvBuf_1[0];
		}

		img_decode = imdecode(data, IMREAD_COLOR);
		imshow(WIN_RECV, img_decode);
		char c = waitKey(5);
		if(c == 27){
			break;
		}
		close(clnt_sock);
	}
	cout << "Client: Bye..." << endl;
	return 0;
}
