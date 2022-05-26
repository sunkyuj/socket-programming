/* client.c
cd /mnt/c/Users/sunky/socket-programming
gcc client.c -o client
./client 127.0.0.1 8000
./client 208.67.222.222 8000

*/

#include <stdio.h>
#include <stdlib.h>	   // atoi 사용
#include <string.h>	   // memset 사용
#include <unistd.h>	   // sockaddr_in, read, write 등
#include <arpa/inet.h> // htnol, htons, INADDR_ANY, sockaddr_in 등
#include <sys/socket.h>
#include <time.h> // 시간 측정을 위한 라이브러리
//#include <WinSock2.h>

#define BUFFER_SIZE 2048 // 버퍼 크기 설정

// 에러 핸들링 함수 선언
void error_handling(char *error_message);

int main(int argc, char *argv[])
{

	clock_t start, finish; // 시간 측정 함수

	int sock; // 소켓 생성
			  // sockaddr_in: 소켓 주소의 틀을 형성해주는 구조체(AF_INET일 경우 사용)
	struct sockaddr_in serv_addr;

	FILE *fp;
	char buffer[BUFFER_SIZE];
	int read_size = 0; // read size
	int byte_size = 0; // 파일의 크기

	char message_from_server[30];

	char error_message[] = "Error!"; // 에러 메시지

	if (argc != 3) // argc: 메인 함수에 전달되는 정보의 개수
	{
		printf("Usage: %s <IP> <port>\n", argv[0]); // argv[]: 메인 함수에 전달되는 실질적 정보(문자열의 배열)
		exit(1);
	}

	// [socket 함수] 소켓 생성
	sock = socket(PF_INET, SOCK_STREAM, 0); // ipv4, TCP, protocol
	if (sock == -1)
		error_handling("socket error!");
	printf("client socket created\n");

	memset(&serv_addr, 0, sizeof(serv_addr));		// serv_addr 크기만큼 0으로 초기화
	serv_addr.sin_family = AF_INET;					// IPv4 주소 체계
	serv_addr.sin_addr.s_addr = inet_addr(argv[1]); // IP 주소 저장
	serv_addr.sin_port = htons(atoi(argv[2]));		// Port 저장

	// [connect 함수] 서버와 연결
	if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == 1)
		error_handling("connect error!");
	printf("client connected to server\n");

	// [write 함수] 서버에게 인사 메시지 전송
	char message_to_server[] = "Hello Server!\n";
	write(sock, message_to_server, sizeof(message_to_server));

	// [read 함수] 서버로 부터 인사 메시지 확인
	if (read(sock, message_from_server, sizeof(message_from_server) - 1) == -1)
		error_handling("read error!");
	printf("Message from server: %s\n", message_from_server);

	printf("Downloading...\n");

	// [fopen 함수] 전송받고 저장할 파일 열기
	fp = fopen("./recieved_video.avi", "wb"); // 옵션: write binary

	// 시간 측정 시작
	start = clock();

	// 파일 수신
	while (read_size = read(sock, buffer, BUFFER_SIZE))
	{
		byte_size += read_size;
		fwrite((void *)buffer, 1, read_size, fp);
	}

	// 시간 측정 종료!
	finish = clock();

	printf("Download Delay: %f(seconds)\n", (float)(finish - start) / CLOCKS_PER_SEC);
	printf("Complete Downloading, total %d (bytes)\n", byte_size);

	// [fclose 함수] 파일 닫기
	fclose(fp);

	// [close 함수] 소켓 해제
	close(sock);
	// 파일 같은지 확인\vscode-scm
	printf("checking if the two files are the same...\n");
	if (system("cmp -s video.avi recieved_video.avi"))
		printf("the two files are different\n");
	else
		printf("the two files are the same\n");

	return 0;
}

// 에러 핸들링 함수 정의
void error_handling(char *error_message)
{
	fputs(error_message, stderr);
	fputc('\n', stderr);
	exit(1);
}
