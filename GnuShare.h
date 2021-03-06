#pragma once


#define MAX_EVENTS	   128
#define MAX_INDEX_EST  1000


class CGnuCore;
class CGnuNetworks;
class CGnuNode;

class CGnuFileHash;
class CGnuWordHash;
class CGnuMeta;

struct SharedFile;
struct SharedDirectory;
struct GnuQuery;

class CGnuShare  
{
public:
	CGnuShare(CGnuCore*);
	virtual ~CGnuShare();


	void	InitShare();
	void	StopShare(UINT);
	void    endThreads();

	bool AddSharedDir(CString strPath, bool bRecursive);

	CString GetFilePath(int index);
	CString GetFileName(int index);
	CString GetFileHash(int index, int HashType);
	
	void	ShareUpdate(UINT FileID);
	void	ShareReload();

	void HitTableRefresh();

	void Timer();

	CEvent m_HashReady;
	
	// Functions called from share thread
	void LoadFiles();
	void RecurseLoad(CString, CString, bool, DWORD &, uint64 &);
	void ResetDirectories(DWORD &, LPHANDLE);
	void ReleaseEventList(DWORD &EventCount, LPHANDLE EventList);

	DWORD  m_TotalLocalFiles;
	uint64 m_TotalLocalSize;
	DWORD  m_UltrapeerSizeMarker;

	bool  m_LoadingActive;
	bool  m_ShareReload;
	bool  m_StopThread;
	bool  m_BlockUpdate;

	// Alt Locations
	void    AddShareAltLocation(CString Hash, IPv4 Location);
	CString GetShareAltLocHeader(CString Hash, IP ToIP, int HostCount=6);

	void    RemoveShareAltLocation(CString Hash, IPv4 Location);

	// Stats
	double m_Freq;
	
	int m_Minute;

	UINT   m_NextFileID;
	std::map<UINT, UINT> m_FileIDMap;

	UINT   m_NextDirID;
	std::map<UINT, UINT> m_DirIDMap;


	// Must *properly* gain access to the following members
	// The thread uses them as well as gnucleus
	static UINT ShareWorker(LPVOID pVoidShare);

	CEvent	m_TriggerThread;
	bool	m_UpdateShared;

	CCriticalSection m_QueueAccess;
	std::list<GnuQuery*> m_PendingQueries;

	CCriticalSection             m_FilesAccess;
	std::vector<SharedFile>		 m_SharedFiles;		  // Local file index
	std::vector<SharedDirectory> m_SharedDirectories;
	std::map<CString, int>       m_SharedHashMap; // Key hash, Value index


	CGnuCore*     m_pCore;
	CGnuNetworks* m_pNet;
	CGnuFileHash* m_pHash;
	CGnuWordHash* m_pWordTable;
	CGnuMeta*	  m_pMeta;

	CWinThread*   m_pShareThread;
};

struct SharedFile
{
	std::basic_string<char> Name;
	std::basic_string<char> NameLower;
	std::basic_string<char> Dir;

	std::basic_string<char> TimeStamp;
	bool HashError;

	std::deque<IPv4> AltHosts;

	uint32  FileID;
	uint32  Index;

	uint64 Size;  
	uint32 Matches; 
	uint32 Uploads;

	// Hash
	std::basic_string<char> HashValues[HASH_TYPES];
	byte* TigerTree;
	int   TreeSize;
	int   TreeDepth;

	// Meta
	int MetaID;
	std::map<int, CString> AttributeMap;

	// QRP
	std::vector< std::basic_string<char> > Keywords;
	std::vector<UINT> HashIndexes;

	// Constructor
	SharedFile()
	{
		HashError = false;

		FileID  = 0;
		Index   = 0;
		Size    = 0;  
		Matches = 0; 
		Uploads = 0;

		TigerTree = 0;
		TreeSize  = 0;
		TreeDepth = 0;

		MetaID = 0;
	};
};

struct SharedDirectory
{
	UINT    DirID;

	CString Name;
	bool	Recursive;
	uint64	Size;
	DWORD	FileCount;
};

#define MAX_QUERY_PACKET_SIZE 512

struct GnuQuery
{
	int  Network;
	bool Forward;

	bool UltraForward;
	bool SupportF2F;

	int   OriginID;
	IPv4  DirectAddress;
	GUID  SearchGuid;

	std::vector<CString> Terms;

	uint64 MinSize;
	uint64 MaxSize;

	byte Packet[MAX_QUERY_PACKET_SIZE];
	int  PacketSize;

	GnuQuery()
	{
		Network  = 0;
		Forward  = false;

		UltraForward = false;
		SupportF2F   = false;

		OriginID = 0;

		MinSize = 0;
		MaxSize = 0;

		PacketSize = 0;
	};
};

