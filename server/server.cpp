#include <cstdlib>
#include <iostream>
#include <cstring>
#include <string>
#include <sys/types.h>
#include <type_traits>
#include <vector>
#include <cmath>
#include <ctime>

#include <opencv4/opencv2/videoio.hpp>
#include <opencv4/opencv2/core.hpp>
#include <opencv4/opencv2/highgui.hpp>

#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

//#define MAXN 300000
//uchar buffer[MAXN];

using namespace std;
using namespace cv;


const char* server_ip_addr = "192.168.191.4";
/*接收帧端口*/
unsigned short port = 1234;
/*回传帧端口*/
unsigned short port_bak = 2233;


int main(int argc, char** argv){
	/*接收帧socket*/
	int serv_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	/*接收帧addr*/
	struct sockaddr_in serv_addr;
	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = inet_addr(server_ip_addr);
	serv_addr.sin_port = htons(port);
	bind(serv_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
	listen(serv_sock, 20);

	
	/*回传帧socket*/
	int serv_sock_bak = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	/*回传帧addr*/
	struct sockaddr_in serv_addr_bak;
	memset(&serv_addr_bak, 0, sizeof(serv_addr_bak));
	serv_addr_bak.sin_family = AF_INET;
	serv_addr_bak.sin_addr.s_addr = inet_addr(server_ip_addr);
	serv_addr_bak.sin_port = htons(port_bak);
	bind(serv_sock_bak, (struct sockaddr*)&serv_addr_bak, sizeof(serv_addr_bak));
	listen(serv_sock_bak, 20);


	/*数据采集端*/
	struct sockaddr_in clnt_addr;
	socklen_t clnt_addr_len = sizeof(clnt_addr);

	/*回传客户端*/
	struct sockaddr_in clnt_addr_bak;
	socklen_t clnt_addr_bak_len = sizeof(clnt_addr_bak);

	char recvBuf[16] = {0};
	char recvBuf_1[1] = {0};
	Mat img_decode;
	vector<u_char> recv_data;

	while(1){
		/*接收数据*/
		int clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_addr, &clnt_addr_len);
		read(clnt_sock, recvBuf, 16);

		/*回传数据*/
		int clnt_sock_bak = accept(serv_sock_bak, (struct sockaddr*)&clnt_addr_bak, &clnt_addr_bak_len);
		write(clnt_sock_bak, recvBuf, 16);
		
		for(int i = 0; i < 16; ++i){
			if(recvBuf[i] < '0' || recvBuf[i] > '9'){
				recvBuf[i] = ' ';
			}
		}
		int img_len = atoi(recvBuf);
		recv_data.resize(img_len);
		for(int i = 0; i < img_len; ++i){
			read(clnt_sock, recvBuf_1, 1);
			recv_data[i] = recvBuf_1[0];
			write(clnt_sock_bak, recvBuf_1, sizeof(recvBuf_1));
		}

		cout << "Test: " <<recv_data.size() << endl;

		time_t now_time = time(NULL);
		tm* t_tm = localtime(&now_time);
		string message = asctime(t_tm);
		write(clnt_sock, message.c_str(), strlen(message.c_str()));

		/*测试*/
		img_decode = imdecode(recv_data, IMREAD_COLOR);
		if(img_decode.empty()){
			cerr << "通讯中断..." << endl;
			return -3;
		}
		imshow("RECV", img_decode);
		char c = waitKey(5);
		if(c == 27) break;
		close(clnt_sock);
		/*测试*/
	}

	return 0;
}
