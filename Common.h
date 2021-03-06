#pragma once


#include "stdafx.h"

namespace gdna 
{

// Functions
DWORD	AssignThreadToCPU(CWinThread *pThread, DWORD cpuNumber);
void    GnuStartThread(CWinThread* &pTarget, AFX_THREADPROC pWorkFunc, void* pCaller);
void    GnuEndThread(CWinThread* &pTarget);

void	SetRandColor(COLORREF &);

CString	GetFileError(CFileException* error);


CString DecodeURL(CString URL);

CString ParseString( CString &Str, char delim = ',');

void    GnuCreateGuid(GUID *pGuid);

CTimeSpan LocalTimeZone();
CString   GetTimeZoneStr();

#define tolowerex(letter) ((65 <= letter && letter <= 90) ? letter + 32 : letter)

//char tolowerex(char letter);

int memfind(byte* mem, int length, byte value);

uint32 HashGuid(GUID &Guid);

CString HexDump(byte* buffer, int length);

bool IsPrivateIP(IP Address);

CString IPv4toStr(IPv4 Address);
IPv4    StrtoIPv4(CString HostPort);

std::vector<CString> RandomizeVector(std::vector<CString> vTarget);

} // end gdna namespace

// Definitions
class AltLocation /* : public HashedFile */
{
public:
    CString Name;			// Name of File
	uint32	Index;			// Index of the remote file
	CString Sha1Hash;
    Node	HostPort;		// location of alternate

	AltLocation()  { Clear(); };
	AltLocation(CString& str) { *this = str;};
	~AltLocation() { Clear(); };

	void Clear()
	{
		Name   = Sha1Hash = "";
		Index  = 0; 
		HostPort.Clear();
	};

	bool isValid()
	{
		if (!HostPort.Host.IsEmpty() && (!Sha1Hash.IsEmpty() || (!Name.IsEmpty() && Index != 0)))
			return true;

		return false;
	};

	void UpdateTime()
	{
		HostPort.LastSeen = CTime::GetCurrentTime() - LocalTimeZone();	
	};

	bool operator ==(AltLocation& first)
	{
		// check if host matches
		if (HostPort == first.HostPort)
		{
			if(!Sha1Hash.IsEmpty())
				if (Sha1Hash == first.Sha1Hash)
					return true;

			if(Name == first.Name && Index == first.Index)
				return true;
		}

		return false;
	};

	// Simple Assignment
	AltLocation& operator=(CString& str);

	// Build an AltLocation string
	CString GetString();
};


struct RangeType
{
	uint64 StartByte;
	uint64 EndByte;
};


struct FileSource 
{
	uint32 SourceID;
	
	// Node info
	IPv4	  Address;
	int		  Network;
	CString   HostStr;
	CString   Path;
	int       Speed;  // kilobits per sec
	CString	  Vendor;
	int		  Distance;

	int  GnuRouteID;
	GUID PushID;
	std::vector<IPv4> DirectHubs;

	bool Firewall;
	bool OpenSlots;
	bool Busy;
	bool Stable;
	bool ActualSpeed;
	bool SupportF2F;

	int	RealBytesPerSec;


	// File info
	CString   Name;
	CString   NameLower;

	uint32  FileIndex;
	uint64  Size;

	CString	Sha1Hash;
	CString TigerHash;
	
	int MetaID;
	std::map<int, CString> AttributeMap;
	
	std::vector<CString>   GnuExtraInfo;
	
	std::vector<RangeType> AvailableRanges;

	// Download info
	CString	  Handshake;
	CString   Error;
	int		  RetryWait;
	uint64	  LastTried;
	uint64	  OnHeapTime;
	uint64	  TimeToTry;
	int		  Tries;
	bool	  TigerSupport;
	bool	  TigerUseThex;
	CString   TigerPath;
	bool	  PushSent;
	bool	  HasDownload; // true while a socket exists for this host

	enum States 
	{
		eUntested = 1,
		eTrying   = 2,
		eAlive    = 4,
		eCorrupt  = 8,
		eFailed   = 16,
		eUdpAlive = 32,
		eTryAgain = 64
	} Status;


	FileSource() 
	{	
		Init();
	};

	FileSource(AltLocation& nAltLoc)
	{
		Init();

		Network	   = NETWORK_GNUTELLA; // Replace with more specific

		Name	= NameLower = nAltLoc.Name;
		NameLower.MakeLower();

		Sha1Hash	= nAltLoc.Sha1Hash;
		FileIndex	= nAltLoc.Index;

		Address.Host = StrtoIP(nAltLoc.HostPort.Host);
		Address.Port = nAltLoc.HostPort.Port;
	};

	void Init()
	{
		SourceID	= 0;
		
		// Node
		Network	   = 0;
		Speed	   = 0;
		Distance   = 0;
		GnuRouteID = 0;
		memset(&PushID, 0, 16);
		
		Firewall	= false;
		OpenSlots	= false;
		Busy		= false;
		Stable		= false;
		ActualSpeed = false;
		SupportF2F  = false;

		RealBytesPerSec = 0;
		
		// File
		FileIndex	= 0;
		Size		= 0;
		MetaID = 0;
	
		// Download
		RetryWait	 = 0;
		Tries		 = 0;
		LastTried    = 0;
		OnHeapTime	 = 0;
		TimeToTry	 = 0;
		HasDownload  = false;
		TigerSupport = false;
		TigerUseThex = false;
		PushSent     = false;

		Status = States::eUntested;
	};
};

struct ResultGroup
{
	uint32 ResultID;

	CString  Name;
	CString  NameLower;
	uint64   Size;
	uint32   AvgSpeed;

	CString Sha1Hash;

	uint32					  NextHostID;
	std::map<uint32, uint32>  HostMap;
	std::vector<FileSource>   ResultList;

	// Meta avg
	int AvgMetaID;
	std::map<int, CString> AvgAttributeMap;

	int State;

	ResultGroup()
	{
		ResultID = 0;
		NextHostID = 1;

		Size = 0;
		AvgSpeed = 0;

		AvgMetaID = 0;
	
		State = 0;
	};
};

class CMovingAvg
{
public:
	int* m_pAvg;
	int  m_Range;
	int  m_Total;
	int  m_Entries;
	int  m_Pos;
	int  m_SecondSum;

	CMovingAvg()
	{
		m_pAvg    = NULL;
		m_Range   = 0;
		m_Total   = 0;
		m_Entries = 0;
		m_Pos	  = 0;
		m_SecondSum = 0;
	};

	~CMovingAvg()
	{
		if(m_pAvg)
			delete [] m_pAvg;
	};

	void SetSize(int size)
	{
		ASSERT(size);
		if(size <= 0)
			return;

		if(m_pAvg)
			delete [] m_pAvg;

		m_pAvg    = new int[size];

		memset(m_pAvg, 0, sizeof(int) * size);

		m_Range   = size;
		m_Total   = 0;
		m_Entries = 0;
		m_Pos     = 0;
	};

	void Input(int value)
	{
		m_SecondSum += value;
	}

	void Next()
	{
		if(m_Entries < m_Range)
			m_Entries++;

		if(m_Pos == m_Range)
			m_Pos = 0;

		m_Total       -= m_pAvg[m_Pos];
		m_pAvg[m_Pos]  = m_SecondSum;
		m_Total       += m_SecondSum;

		m_SecondSum = 0;

		m_Pos++;
	};

	int GetAverage()
	{
		if(m_Entries)
			return m_Total / m_Entries;
		
		return 0;
	}
};



