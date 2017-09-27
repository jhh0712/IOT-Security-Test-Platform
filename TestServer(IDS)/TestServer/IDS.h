#pragma once

#include "pcap.h"	//pcap 헤더 추가
#include "afxwin.h"
#include "afxcmn.h"


#pragma comment (lib, "wpcap.lib")

// CIDS dialog

class CIDS : public CDialogEx
{
	DECLARE_DYNAMIC(CIDS)

public:
	CIDS(CWnd* pParent = NULL);   // standard constructor
	virtual ~CIDS();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_IDS };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL OnInitDialog();

	struct ether_addr	//이더넷 주소정보
	{
		unsigned char ether_addr_octet[6];
	};

	struct ether_header	//이더넷 헤더
	{
		struct  ether_addr ether_dhost;
		struct  ether_addr ether_shost;
		unsigned short ether_type;
	};

	struct ip_header	//IP 헤더
	{
		unsigned char ip_header_len : 4;
		unsigned char ip_version : 4;
		unsigned char ip_tos;
		unsigned short ip_total_length;
		unsigned short ip_id;
		unsigned char ip_frag_offset : 5;
		unsigned char ip_more_fragment : 1;
		unsigned char ip_dont_fragment : 1;
		unsigned char ip_reserved_zero : 1;
		unsigned char ip_frag_offset1;
		unsigned char ip_ttl;
		unsigned char ip_protocol;
		unsigned short ip_checksum;
		struct in_addr ip_srcaddr;
		struct in_addr ip_destaddr;
	};

	struct tcp_header	//TCP 헤더
	{
		unsigned short source_port;
		unsigned short dest_port;
		unsigned int sequence;
		unsigned int acknowledge;
		unsigned char ns : 1;
		unsigned char reserved_part1 : 3;
		unsigned char data_offset : 4;
		unsigned char fin : 1;
		unsigned char syn : 1;
		unsigned char rst : 1;
		unsigned char psh : 1;
		unsigned char ack : 1;
		unsigned char urg : 1;
		unsigned char ecn : 1;
		unsigned char cwr : 1;
		unsigned short window;
		unsigned short checksum;
		unsigned short urgent_pointer;
	};

	void print_ether_header(const unsigned char *data);	//이더넷 헤더 출력함수
	int print_ip_header(const unsigned char *data);	//IP 헤더 출력함수
	int print_tcp_header(const unsigned char *data);	//TCP 헤더 출력함수
	//void print_data(const unsigned char *data);	//DATA 출력함수

	void Check_Normal(const unsigned char *data, const unsigned char *data2);

	pcap_if_t *alldevs = NULL;
	char errbuf[PCAP_ERRBUF_SIZE];

	int offset;
	int inum;

	pcap_if_t *d; int i;
	pcap_t  *fp;

	afx_msg void OnBnClickedButton1();	//IDS 시작버튼
	afx_msg void OnBnClickedButton2();	//IDS 중지버튼

	CListBox IDS_LIST;
	int line;
	

	CListCtrl DEVICE_LIST;
	afx_msg void OnNMDblclkDevice(NMHDR *pNMHDR, LRESULT *pResult);
	CString click;

	static UINT ThreadFirst(LPVOID _mothod);

	void ThreadFunc();	//스레드에서 실행할 함수

	CWinThread *p1;	//스레드 포인터

	bool DestroyTherad();	//스레드 종료함수

	CEdit m_editEth;
	CEdit m_editTcp;
	CEdit m_editIp;
	CEdit m_editData;
	

	struct bpf_program fcode;
	CEdit m_editnormal;
	CEdit m_editabnormal;

	CString time;

	int h, m, s;

	int d_m;
	int count;
};
