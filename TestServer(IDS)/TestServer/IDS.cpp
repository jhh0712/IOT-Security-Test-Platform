// IDS.cpp : implementation file
//

#include "stdafx.h"
#include "TestServer.h"
#include "IDS.h"
#include "afxdialogex.h"

//#define FILTER_RULE "host 192.168.0.57 and port 7777"	//�����ּҿ� ��Ʈ��ȣ �Է°���
#define FILTER_RULE "port 7777"
#pragma warning(disable:4996)

// CIDS dialog

IMPLEMENT_DYNAMIC(CIDS, CDialogEx)

CIDS::CIDS(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_IDS, pParent)
{

}

CIDS::~CIDS()
{
}

void CIDS::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, IDS_LIST);
	DDX_Control(pDX, IDC_DEVICE, DEVICE_LIST);
	DDX_Control(pDX, IDC_ETHERNET, m_editEth);
	DDX_Control(pDX, IDC_TCP, m_editTcp);
	DDX_Control(pDX, IDC_IP, m_editIp);
	DDX_Control(pDX, IDC_DATA, m_editData);
	DDX_Control(pDX, IDC_NORMAL2, m_editnormal);
	DDX_Control(pDX, IDC_ABNORMAL, m_editabnormal);
}


BEGIN_MESSAGE_MAP(CIDS, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON1, &CIDS::OnBnClickedButton1)
//	ON_LBN_DBLCLK(IDC_LIST1, &CIDS::OnLbnDblclkList1)
ON_NOTIFY(NM_DBLCLK, IDC_DEVICE, &CIDS::OnNMDblclkDevice)
ON_BN_CLICKED(IDC_BUTTON2, &CIDS::OnBnClickedButton2)
END_MESSAGE_MAP()


// CIDS message handlers


BOOL CIDS::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  Add extra initialization here

	offset = 0;
	line = 0;

	p1 = NULL;

	DEVICE_LIST.InsertColumn(1, _T("NUM"), LVCFMT_LEFT, 50);		//List Control �ʱ�ȭ
	DEVICE_LIST.InsertColumn(2, _T("DEVICE"), LVCFMT_LEFT, 200);	//List Control �ʱ�ȭ

	GetDlgItem(IDC_BUTTON2)->EnableWindow(FALSE);

	m_editEth.SetLimitText(0);
	m_editTcp.SetLimitText(0);
	m_editIp.SetLimitText(0);
	m_editData.SetLimitText(0);	
	m_editnormal.SetLimitText(0);
	m_editabnormal.SetLimitText(0);
	//Edit Control ���� �ִ�����

	h = 0;
	m = 0;
	s = 0;
	d_m = 0;
	count = 0;
	//�ʱ�ȭ

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

UINT CIDS::ThreadFirst(LPVOID _mothod)
{
	CIDS *fir = (CIDS*)_mothod;

	fir->ThreadFunc();	//�����忡�� ������ �Լ�

	return 0;

}

void CIDS::ThreadFunc()	//��������� �Լ�
{
	struct pcap_pkthdr *header;

	const unsigned char *pkt_data;
	
	int res;
	int offset2;

	while ((res = pcap_next_ex(fp, &header, &pkt_data)) >= 0) {
		if (res == 0) continue;
		offset2 = 0;

		print_ether_header(pkt_data);
		pkt_data = pkt_data + 14;       // raw_pkt_data�� 14�������� �̴���

		offset = print_ip_header(pkt_data);
		offset2 += offset;
		pkt_data = pkt_data + offset;           // ip_header�� ���̸�ŭ ������

		offset = print_tcp_header(pkt_data);
		offset2 += offset;
		pkt_data = pkt_data + offset;           //print_tcp_header *4 ������ ��ġ�� ������
		
		pkt_data = pkt_data - offset2;
		Check_Normal(pkt_data, pkt_data + offset2);	//����, ������Ž�� , ������ ���
		
	}

}

void CIDS::Check_Normal(const unsigned char *data, const unsigned char *data2)
{
	count++;

	struct  ip_header *ih;
	ih = (struct ip_header *)data;  // ip_header�� ����ü ���·� ��ȯ

	char text[2048] = { 0, };

	for (int i = 0; i < (ntohs(ih->ip_total_length) - 52); i++)
	{
		text[i] = data2[i];	//���ڿ� �迭�� ������ ���̸�ŭ ������ ����
	}
	
	CString str = "";
	CString normal = "";
	CString abnormal = "";

	char len2[100] = { 0, };

	int value;
	int no;
	char val[100] = { 0, };

	//IDS RULE ����

	CTime ctime = CTime::GetCurrentTime();	//����ð� ������

	int past;	//���� �� ������ ����

	past = s;	//���ſ� �� ����

	h = ctime.GetHour();
	m = ctime.GetMinute();
	s = ctime.GetSecond();	//�ð��� ���� ����

	time = "";
	CString temp2 = "";

	temp2.Format("%02d:", h);
	time += temp2;
	temp2.Format("%02d:", m);
	time += temp2;
	temp2.Format("%02d", s);
	time += temp2;
	time += "\r\n";	//�ð��� ����

	char proto[100] = { 0, };
	char timing[100] = { 0, };

	past = s - past;	//Ÿ�̹� ���

	if (past < 0)
		past += 60;	// 60�� �Ѿ�� �������� 60���ؼ� Ÿ�̹� ���

	d_m += past;	//�𵵽� Ž���� ���� �ð� ���ذ�

	if (d_m > 60 && d_m < 70)//�ð� 60~70�� 
	{
		if (count > 1000)	//6���� �ʰ� ī��Ʈ�� �𵵽��� �Ǹ� (�𵵽� ���� ��������)
		{
			abnormal += "DDOS DETCTION\r\n";
		}
	}

	for (int i = 0; i < strlen(text); i++)
	{
		value = text[i];

		if (value > 0 && value < 51)	//������ ��Ŷ �˻� (���� ������ ���� 1~50) ����� ȯ�濡 ���� ��������
			no = -1;
		else{	//������ ���� ��� ��� �ش� ���� ������
			no = value;
			break;
		}

	}

	
	//����Ž�� ���� (����� ȯ�濡 ���� ��������, ���� �������� 6��, ��Ŷ���� 50, Ÿ�̹� 0,3 �ʷ� ����)
	if (ih->ip_protocol == 0x06 && ((ntohs(ih->ip_total_length) - 52) == 50 || ntohs(ih->ip_total_length) == 52) && (past == 3 || past == 0) && no == -1)
	{
		if ((ntohs(ih->ip_total_length)-52) == 50)
		{
				normal += "==========NORMAL==========\r\n";
				normal += time;
				normal += "Protocol : TCP\r\n";
				sprintf(len2, "Data_Packet Length : %d\r\n", ntohs(ih->ip_total_length) - 52);
				sprintf(timing, "Timing : %d s\r\n", past);

				normal += len2;
				normal += timing;
				normal += "Value : OK.\r\n";
		}

	}
	else {	//������Ž�� ���� (����� ȯ�濡 ���� ��������)
		if (no != -1 || ((ntohs(ih->ip_total_length) - 52) != 50 && ntohs(ih->ip_total_length) != 52) || past != 3)
		{
			if (((ntohs(ih->ip_total_length) - 52) != 64 && ntohs(ih->ip_total_length) != 52) && past != 0)	//��ȣȭ ��Ŷ �ɷ����� �κ�
			{
			
				abnormal += "==========ABNORMAL==========\r\n";
				abnormal += time;
				sprintf(proto, "Protocol Num : %d\r\n", ih->ip_protocol);
				sprintf(len2, "Data_Packet Length : %d\r\n", ntohs(ih->ip_total_length) - 52);
				sprintf(timing, "Timing : %d s\r\n", past);
				sprintf(val, "Value Error : %d\r\n", value);

				abnormal += proto;
				abnormal += len2;
				abnormal += timing;
				abnormal += val;
			}
		}
		
	}



	//�������
	int nLen2 = m_editnormal.GetWindowTextLength();
	if (nLen2 > 2100000000)
	{
		m_editnormal.SetSel(0, -1);
		m_editnormal.Clear();

		str = "NORMAL_Log Clear";

		IDS_LIST.InsertString(line++, str);
		IDS_LIST.SetCurSel(line - 1);

	}
	else
	{
		m_editnormal.SetSel(nLen2, nLen2);
		m_editnormal.ReplaceSel(normal);
	}

	//���������
	int nLen3 = m_editabnormal.GetWindowTextLength();
	if (nLen3 > 2100000000)
	{
		m_editabnormal.SetSel(0, -1);
		m_editabnormal.Clear();

		str = "ABNORMAL_Log Clear";

		IDS_LIST.InsertString(line++, str);
		IDS_LIST.SetCurSel(line - 1);

	}
	else
	{
		m_editabnormal.SetSel(nLen3, nLen3);
		m_editabnormal.ReplaceSel(abnormal);
	}


	//������ ��º�
	CString str2 = "";
	str2 = "\r\n============DATA============\r\n";
	
	CString temp = "";
	int e_count = 1;

	for (int i = 0; i < (ntohs(ih->ip_total_length) - 52); i++)	
	{
		value = text[i];

		if (value < 0)
			temp.Format("%02X\t", value & (0xff));
		else
			temp.Format("%02X\t", value);	//�� ����Ʈ�� HEX �ڵ�� �ٲ���

		str2 += temp;

		if (e_count % 5 == 0)
			str2 += "\r\n";	//8����Ʈ���� �ٹٲ�

		e_count++;

	}

	int nLen = m_editData.GetWindowTextLength();
	if (nLen > 2100000000)
	{
		m_editEth.SetSel(0, -1);
		m_editEth.Clear();

		m_editIp.SetSel(0, -1);
		m_editIp.Clear();

		m_editTcp.SetSel(0, -1);
		m_editTcp.Clear();

		m_editData.SetSel(0, -1);
		m_editData.Clear();

		str = "Log Clear";

		IDS_LIST.InsertString(line++, str);
		IDS_LIST.SetCurSel(line - 1);

	}
	else
	{
		m_editData.SetSel(nLen, nLen);
		m_editData.ReplaceSel(str2);
	}


}

void CIDS::OnBnClickedButton1()	//IDS ���� ��ư
{
	GetDlgItem(IDC_BUTTON1)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON2)->EnableWindow(TRUE);

	CString str = "";

	if (pcap_findalldevs(&alldevs, errbuf) == -1) {	

		str="dev find failed\n";

		IDS_LIST.InsertString(line++, str);
		IDS_LIST.SetCurSel(line - 1);

	}
	if (alldevs == NULL) {

		str = "no devs found\n";
		
		IDS_LIST.InsertString(line++, str);
		IDS_LIST.SetCurSel(line - 1);
	}

	int list_count = 0;
	char text[2048] = { 0, };
	
	DEVICE_LIST.DeleteAllItems();

	str = "=========����̽� ��ġ ���=========\n";

	IDS_LIST.InsertString(line++, str);
	IDS_LIST.SetCurSel(line - 1);

	for (d = alldevs, i = 0; d != NULL; d = d->next) {
		
		str = "";
		str.Format("%d", ++i);

		DEVICE_LIST.InsertItem(list_count, str);
		DEVICE_LIST.SetItem(list_count, 1, LVIF_TEXT, d->name, 0, 0, 0, NULL);	//��ġ ��� List Control�� ���

		list_count++;

		if (d->description)
		{
			sprintf(text, "%d. (%s)\n",i, d->description);
			IDS_LIST.InsertString(line++, text);
			IDS_LIST.SetCurSel(line - 1);
		}
		else
		{
			str = "(No description available)\n";
			IDS_LIST.InsertString(line++, str);
			IDS_LIST.SetCurSel(line - 1);
		}
	}

	str = "DEVICE_LIST ���� ��ȣ ����Ŭ��\n";

	IDS_LIST.InsertString(line++, str);
	IDS_LIST.SetCurSel(line - 1);
	
}

void CIDS::print_ether_header(const unsigned char *data)	//�̴��� ��� ��º�
{
	struct  ether_header *eh;               // �̴��� ��� ����ü
	unsigned short ether_type;
	eh = (struct ether_header *)data;       // �޾ƿ� �ο� �����͸� �̴��� �������ü ���·� ���
	ether_type = ntohs(eh->ether_type);       // ���ڴ� ��Ʈ��ũ ����Ʈ �������� ȣ��Ʈ ����Ʈ ������ �ٲ����

	CString str = "";

	if (ether_type != 0x0800)
	{
		str = "ether type wrong\n";

		IDS_LIST.InsertString(line++, str);
		IDS_LIST.SetCurSel(line - 1);

		return;
	}

	// �̴��� ��� ���
	char text[2048] = { 0, };
	char text2[2048] = { 0, };
	sprintf(text, "Dst MAC Addr [%02x:%02x:%02x:%02x:%02x:%02x]\r\n", 
		eh->ether_dhost.ether_addr_octet[0],
		eh->ether_dhost.ether_addr_octet[1],
		eh->ether_dhost.ether_addr_octet[2],
		eh->ether_dhost.ether_addr_octet[3],
		eh->ether_dhost.ether_addr_octet[4],
		eh->ether_dhost.ether_addr_octet[5]);


	sprintf(text2, "Src MAC Addr [%02x:%02x:%02x:%02x:%02x:%02x]\r\n", // 6 byte for src
		eh->ether_shost.ether_addr_octet[0],
		eh->ether_shost.ether_addr_octet[1],
		eh->ether_shost.ether_addr_octet[2],
		eh->ether_shost.ether_addr_octet[3],
		eh->ether_shost.ether_addr_octet[4],
		eh->ether_shost.ether_addr_octet[5]);

	str = "============ETHERNET HEADER============\r\n";

	str += text;
	str += text2;

	int nLen = m_editEth.GetWindowTextLength();
	if (nLen > 2100000000)
	{
		m_editEth.SetSel(0, -1);
		m_editEth.Clear();

		m_editIp.SetSel(0, -1);
		m_editIp.Clear();

		m_editTcp.SetSel(0, -1);
		m_editTcp.Clear();

		m_editData.SetSel(0, -1);
		m_editData.Clear();

		str = "Log Clear";

		IDS_LIST.InsertString(line++, str);
		IDS_LIST.SetCurSel(line - 1);

	}
	else
	{
		m_editEth.SetSel(nLen, nLen);
		m_editEth.ReplaceSel(str);
	}
}

int CIDS::print_ip_header(const unsigned char *data)	//ip ��� ��º�
{
	struct  ip_header *ih;
	ih = (struct ip_header *)data;  // ip_header�� ����ü ���·� ��ȯ
	
	CString str = "";
	

	char ip[100] = { 0, };

	str = "============IP HEADER============\r\n";
	sprintf(ip, "IPv%d ver \r\n", ih->ip_version);


	// Total packet length (Headers + data)
	char len[100] = { 0, };
	sprintf(len, "Packet Length : %d\r\n", ntohs(ih->ip_total_length) + 14);
	

	char ttl[100] = { 0, };

	sprintf(ttl, "TTL : %d\r\n", ih->ip_ttl);

	str += ip;
	str += len;
	str += ttl;
	
	if (ih->ip_protocol == 0x06)	//�������� 6��
	{
		str += "Protocol : TCP\r\n";
		
	}
	
	char src_ip[100] = { 0, };
	char dst_ip[100] = { 0, };

	sprintf(src_ip, "Src IP Addr : %s\r\n", inet_ntoa(ih->ip_srcaddr));		//���� ip �ּ�
	sprintf(dst_ip, "Dst IP Addr : %s\r\n", inet_ntoa(ih->ip_destaddr));	//������ ip �ּ�

	str += src_ip;
	str += dst_ip;

	//��� �� �ʱ�ȭ
	int nLen = m_editIp.GetWindowTextLength();
	if (nLen > 2100000000)
	{
		m_editEth.SetSel(0, -1);
		m_editEth.Clear();

		m_editIp.SetSel(0, -1);
		m_editIp.Clear();

		m_editTcp.SetSel(0, -1);
		m_editTcp.Clear();

		m_editData.SetSel(0, -1);
		m_editData.Clear();

		str = "Log Clear";

		IDS_LIST.InsertString(line++, str);
		IDS_LIST.SetCurSel(line - 1);

	}
	else
	{
		m_editIp.SetSel(nLen, nLen);
		m_editIp.ReplaceSel(str);
	}

	// return to ip header size
	return ih->ip_header_len * 4;
}


int CIDS::print_tcp_header(const unsigned char *data)	//tcp ��� ��º�
{
	struct tcp_header *th;
	th = (struct tcp_header *)data;	//tcp ��� ����ü ���·� ��ȣ��

	char src_port[100] = { 0, };
	char des_port[100] = { 0, };

	sprintf(src_port, "Src Port Num : %d\r\n", ntohs(th->source_port));
	sprintf(des_port, "Dest Port Num : %d\r\n", ntohs(th->dest_port));	//��Ʈ���

	CString str = "";

	str = "============TCP HEADER============\r\n";

	str += src_port;
	str += des_port;
	
	str += "Flag :";

	if (ntohs(th->cwr))
	{
		str += " CWR ";
	}
	if (ntohs(th->ecn))
	{
		str += " ENC ";
	}
	if (ntohs(th->urg))
	{
		str += " URG ";
	}
	if (ntohs(th->ack))
	{
		str += " ACK ";
	}
	if (ntohs(th->psh))
	{
		str += " PUSH ";
	}
	if (ntohs(th->rst))
	{
		str += " RST ";
	}
	if (ntohs(th->syn))
	{
		str += " SYN ";
	}
	if (ntohs(th->fin))
	{
		str += " FIN ";
	}

	str += "\r\n";	//Flag ���

	int nLen = m_editTcp.GetWindowTextLength();
	if (nLen > 2100000000)
	{
		m_editEth.SetSel(0, -1);
		m_editEth.Clear();

		m_editIp.SetSel(0, -1);
		m_editIp.Clear();

		m_editTcp.SetSel(0, -1);
		m_editTcp.Clear();

		m_editData.SetSel(0, -1);
		m_editData.Clear();

		str = "Log Clear";

		IDS_LIST.InsertString(line++, str);
		IDS_LIST.SetCurSel(line - 1);

	}
	else
	{
		m_editTcp.SetSel(nLen, nLen);
		m_editTcp.ReplaceSel(str);
	}
	// return to tcp header size
	return th->data_offset * 4;
}


void CIDS::OnNMDblclkDevice(NMHDR *pNMHDR, LRESULT *pResult)	//List Control Ŭ�� �̺�Ʈ
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: Add your control notification handler code here

	if (pNMItemActivate->iItem != -1)
	{
		CString NUM = DEVICE_LIST.GetItemText(pNMItemActivate->iItem, 0);	//���� Ŭ�� �� �� ������
		inum = _ttoi(NUM);

		CString str = "";
		for (d = alldevs, i = 0; i<inum - 1; d = d->next, i++); // jump to the i-th dev

		if ((fp = pcap_open_live(d->name,      // name of the device
			65536,                   // capture size
			1,  // promiscuous mode
			20,                    // read timeout
			errbuf
			)) == NULL) {

			str = "pcap open failed\n";
			IDS_LIST.InsertString(line++, str);
			IDS_LIST.SetCurSel(line - 1);

			pcap_freealldevs(alldevs);
		}	//�ش� ��ī��� ��Ŷĸó ����

		str = "pcap open successful\n";
		IDS_LIST.InsertString(line++, str);
		IDS_LIST.SetCurSel(line - 1);


		if (pcap_compile(fp,  // pcap handle
			&fcode,  // compiled rule
			FILTER_RULE,  // filter rule
			1,            // optimize
			NULL) < 0) {

			str = "pcap compile failed\n";
			IDS_LIST.InsertString(line++, str);
			IDS_LIST.SetCurSel(line - 1);

			pcap_freealldevs(alldevs);

		}	//pcap ������

		if (pcap_setfilter(fp, &fcode) <0) {

			str = "pcap compile failed\n";
			IDS_LIST.InsertString(line++, str);
			IDS_LIST.SetCurSel(line - 1);

			pcap_freealldevs(alldevs);

		}

		pcap_freealldevs(alldevs); // we don't need this anymore


		if (p1 != NULL)
		{
			DestroyTherad();	//p1 ������ �����Լ� ����
		}

		p1 = AfxBeginThread(ThreadFirst, this, THREAD_PRIORITY_NORMAL, 0, CREATE_SUSPENDED);	//AfxBeginThread �� ������ ����

		if (p1 == NULL)	//p1 ������ �����Ͱ� NULL �ΰ�� (������ ������ �ȵ��� ��)
		{
			AfxMessageBox("Error");
		}

		p1->m_bAutoDelete = FALSE;	//������ �ڵ����� ����
		p1->ResumeThread();

		GetDlgItem(IDC_DEVICE)->EnableWindow(FALSE);

	}

	
	*pResult = 0;
}

bool CIDS::DestroyTherad()	//������ ���� �Լ�
{
	if (p1 != NULL)
	{
		DWORD dwResult = ::WaitForSingleObject(p1->m_hThread, INFINITE);	//Ư�� ������Ʈ ���°� ������ �� ���� ���� �������� ������ ����

		if (dwResult == WAIT_TIMEOUT)	//������Ʈ ���°� �������� �������
			AfxMessageBox("Time OUT!");

		delete p1;

		p1 = NULL;
	}
	return true;

}




void CIDS::OnBnClickedButton2()	//IDS ���� ��ư
{
	// TODO: Add your control notification handler code here


	pcap_close(fp);

	GetDlgItem(IDC_DEVICE)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON1)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON2)->EnableWindow(FALSE);
	
	CString str = "";

	str = "pcap close successful\n";
	IDS_LIST.InsertString(line++, str);
	IDS_LIST.SetCurSel(line - 1);

	DEVICE_LIST.DeleteAllItems();
	
}
