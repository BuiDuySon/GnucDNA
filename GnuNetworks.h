#pragma once

#define ACCEPT_TIMEOUT 4

#define UDP_FULL  1
#define UDP_NAT   2
#define UDP_BLOCK 3

#define SUBNET_LIMIT 3


class CGnuCore;
class CGnuControl;
class CG2Control;
class CGnuSock;
class CGnuCache;
class CGnuSearch;
class CUdpListener;

class CGnuNetworks : public CAsyncSocketEx
{
public:
	CGnuNetworks(CGnuCore* pCore);
	virtual ~CGnuNetworks(void);

	void Connect_Gnu();
	void Disconnect_Gnu();

	void Connect_G2();
	void Disconnect_G2();
	
	void Timer();
	void HourlyTimer();

	IP   GuessLocalHost();
	//bool ConnectingSlotsOpen();
	int  GetMaxHalfConnects();
	bool TcpBacklog();

	int  NetworkConnecting(int Network);
	int  TransfersConnecting();

	bool NotLocal(Node);

	// Networks 
	CGnuControl* m_pGnu;
	CG2Control*  m_pG2;

	CGnuCache* m_pCache;

	int	m_NextNodeID;
	int GetNextNodeID();

	TcpStatus NetStat;

	// Runtime Variables
	IP		m_CurrentIP;
	uint16  m_CurrentPort;
	bool	m_TcpFirewall;		   // Assumes there is a firewall until someone connects
	int 	m_UdpFirewall;
	bool    m_BehindRouter;		   // Set in GuessLocalHost
	int		m_RealSpeedUp;		   // Bytes per sec up
	int		m_RealSpeedDown;	   // Bytes per sec down
	bool	m_HaveUploaded;		   // If client has uploaded successfully
	bool    m_HighBandwidth;

	// Listening control
	bool StartListening();
	void StopListening();
	
	uint16 m_ActivePort;

	virtual void OnAccept(int nErrorCode);

	std::vector<CGnuSock*>	m_SockList;

	CUdpListener* m_pUdpSock;

	// Searching
	void EndSearch(int SearchID);

	int m_NextSearchID;
	std::vector<CGnuSearch*>   m_SearchList;
	std::map<int, CGnuSearch*> m_SearchIDMap;

	void IncomingSource(GUID &SearchGuid, FileSource &Source);


	// NAT Detection
	void AddNatDetect(IP Host);

	std::map<uint32, bool> m_NatDetectMap;
	std::deque<uint32>    m_NatDetectVect;


	CGnuCore* m_pCore;
};
