#pragma once


#include "GnuRouting.h"
#include "GnuPackets.h"
#include "GnuShare.h"

struct key_Value;
struct DynQuery;
struct Gnu_RecvdPacket;

class CGnuNetworks;
class CGnuLocal;
class CGnuPrefs;
class CGnuCore;
class CGnuSock;
class CGnuNode;
class CGnuCache;
class CGnuTransfers;
class CGnuShare;
class CGnuDatagram;
class CGnuProtocol;

#define GNU_ULTRAPEER 1
#define GNU_LEAF	  2

#define MAX_TTL 3

class CGnuControl
{
public:
	CGnuControl(CGnuNetworks*);
	~CGnuControl();


	void Timer();
	void HourlyTimer();

	void ManageNodes();
	void CleanDeadSocks();


	// Other threads always have to lock before touching list
	CCriticalSection m_NodeAccess;
	
	std::vector<CGnuNode*>	    m_NodeList;
	std::map<int, CGnuNode*>    m_NodeIDMap;
	std::map<uint32, CGnuNode*> m_GnuNodeAddrMap;
	std::vector<CGnuNode*>	    m_NodesBrowsing;

	CGnuLocal* m_LanSock;

	void AddNode(CString, UINT);
	void RemoveNode(CGnuNode*);
	CGnuNode* FindNode(CString, UINT);

	CGnuNode* GetRandNode(int Type);

	void AddConnect();
	void DropNode();
	void DropLeaf();

	int	 CountSuperConnects();
	int  CountNormalConnects();
	int  CountLeafConnects();
	
	void NodeUpdate(CGnuNode* pNode);


	// Other
	bool UltrapeerAble();
	void DowngradeClient();	
	void ShareUpdate();
	DWORD GetSpeed();


	// Dynamic Queries
	void AddDynQuery(DynQuery* pQuery);
	void DynQueryTimer();

	std::map<uint32, DynQuery*> m_DynamicQueries;

	void StopSearch(GUID SearchGuid);
	
	
	// Network
	CString  m_NetworkName;
	
	int m_UdpPort;


	// Local Client Data
	CTime   m_ClientUptime;

	DWORD   m_dwFiles;
	DWORD   m_dwFilesSize;


	// SuperNodes
	void SwitchGnuClientMode(int GnuMode);

	int     m_GnuClientMode;
	bool	m_ForcedUltrapeer;

	int m_NormalConnectsApprox;


	// Hash tables
	CGnuRouting m_TableRouting;
	CGnuRouting m_TablePush;
	CGnuRouting m_TableLocal;


	// Bandwidth
	double m_NetSecBytesDown;
	double m_NetSecBytesUp;

	int m_Minute;


	CGnuNetworks*  m_pNet;
	CGnuCore*	   m_pCore;
	CGnuPrefs*	   m_pPrefs;
	CGnuCache*     m_pCache;
	CGnuTransfers* m_pTrans;
	CGnuShare*	   m_pShare;
	CGnuDatagram*  m_pDatagram;
	CGnuProtocol*  m_pProtocol;
};


struct Gnu_RecvdPacket
{
	IPv4		   Source;
	packet_Header* Header;
	int			   Length;
	CGnuNode*	   pTCP;

	Gnu_RecvdPacket(IPv4 Address, packet_Header* header, int length, CGnuNode* pNode=NULL)
	{
		Source = Address;
		Header = header;
		Length = length;
		pTCP   = pNode;
	};
};


#define DQ_TARGET_HITS		50		// Number of hits to obtain for leaf
#define DQ_QUERY_TIMEOUT	(3*60)  // Time a dyn query lives for
#define DQ_QUERY_INTERVAL	7       // Interval to send next query out
#define DQ_UPDATE_INTERVAL	5       // Interval to ask leaf for a hit update
#define DQ_MAX_QUERIES      4		// Simultaneous qeueries by 1 node

struct DynQuery
{
	int NodeID;

	byte* Packet;
	int   PacketLength;

	std::map<int, bool> NodesQueried;

	int Secs;
	int Hits;

	DynQuery(int nodeID, byte* packet, int length)
	{
		NodeID = nodeID;

		Packet = new byte[length];
		memcpy(Packet, packet, length);

		PacketLength = length;

		Secs = 0;
		Hits = 0;
	};

	~DynQuery()
	{
		// Dynamic Query ID:15 Destroyed, Secs:30, Hits:15
		TRACE0("Dynamic Query ID:" + NumtoStr(NodeID) + " Destroyed, Secs:" + NumtoStr(Secs) + ", Hits:" + NumtoStr(Hits) + "\n");

		if(Packet)
			delete [] Packet;

		Packet = NULL;
	};
};