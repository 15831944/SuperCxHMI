//CxDatDef.h

#ifndef __CXDATDEF_H_
#define __CXDATDEF_H_

#include <Time.h>

#define _SUPERCX_DATA_VER 0x0300 

enum 
{ 
	TAG_ANALOG = 0x0,              
	TAG_DIGITAL = 0x1, 
	TAG_TEXT = 0x2,
}; 

class CGenData
{
public:
	DWORD dwID;
	TCHAR szName[50];
	TCHAR szValue[100];
};

class CTagGroupData
{
public:
	DWORD dwID;
	DWORD dwParentGroup;
	TCHAR szName[50];
};

// 1324 ���ֽ�
class CTagData
{
public:
	DWORD dwID;
	DWORD dwParentGroup;
	TCHAR szName[50];	// Name
	int nType;
	TCHAR szDesc[100];	// Desc
	TCHAR szDevice[50];	
	TCHAR szAddress[256];
	TCHAR szDevice2[50];
	TCHAR szAddress2[256];
	BOOL bReadable;
	BOOL bWritable;
	int nConvertMode;
	BOOL bSaveStatus;
	int nScanTime;
	double dSensitivity;
	union
	{
		TCHAR szInitValue[256];
		struct  
		{
			union
			{
				double dInitValue;
				BOOL bInitValue;
			};
			double dMeasureLow;
			double dMeasureHigh;
			double dEngineerLow;
			double dEngineerHigh;
			int nDecimalDigits;
			TCHAR szEngUnitDesc[20];
			TCHAR szArea[256];
			int nPriority;
			TCHAR szExtend1[100];
			TCHAR szExtend2[100];
			double dDeadBand;
			int nDelayTime;
			union
			{
				BOOL bEableLowLow;
				BOOL bEableOpen;
			};
			double dLowLow;
			union
			{
				TCHAR szLowLowText[50];
				TCHAR szOpenText[50];
			};
			union
			{
				BOOL bEableLow;
				BOOL bEableClose;
			};
			double dLow;
			union
			{
				TCHAR szLowText[50];
				TCHAR szCloseText[50];
			};
			union
			{
				BOOL bEableHigh;
				BOOL bEableChange;
			};
			double dHigh;
			union
			{
				TCHAR szHighText[50];
				TCHAR szChangeText[50];
			};
			BOOL bEableHighHigh;
			double dHighHigh;
			TCHAR szHighHighText[50];
			double dDeviationTarget;
			BOOL bEableMajorDeviation;
			BOOL bEableMinorDeviation;
			double dMinorDeviation;
			union
			{
				TCHAR szMinorDeviationText[50];
				TCHAR szOpenDesc[50];
			};
			double dMajorDeviation;
			union
			{
				TCHAR szMajorDeviationText[50];
				TCHAR szCloseDesc[50];
			};
			BOOL bEableRateOfChange; 
			double dRateOfChange;
			TCHAR szRateOfChangeText[50];
		};
	};
	int nPrivilege;
	BOOL bCreateOperation;
	TCHAR szReserved[32];
};

class CTagLogData
{
public:
	DWORD dwID;
	TCHAR szName[50];
	TCHAR szDesc[100];
	TCHAR szRate[20];
	TCHAR szPhase[20];
	TCHAR szQualifier[256];
};

class CTagLogTagData
{
public:
	DWORD dwID;
	DWORD dwTagLog; // ��ӦCTagLogData��dwID
	TCHAR szTag[256];
};

class CDeviceData
{
public:
	DWORD dwID;
	TCHAR szName[50];
	CLSID clsidDriver;
};

class CNodeData
{
public:
	DWORD dwID;
	TCHAR szName[50];
	TCHAR szComputer[50];
	TCHAR szBackupComputer[50];
};

class CAlarmAreaData
{
public:
	DWORD dwID;
	TCHAR szName[50];
};

class CSecurityAreaData
{
public:
	DWORD dwID;
	TCHAR szName[50];
};

class CRecpData
{
public:
	DWORD dwID;
	TCHAR szProduct[50];
	TCHAR szUOM[20];
	double dStandBatchSize;
	double dBatchSize;
};

class CRecpItemData
{
public:
	DWORD dwID;
	TCHAR szIdentify[100];
	TCHAR szFormula[256];
	double dValue;
	TCHAR szUOM[20];
	TCHAR szDesc[100];
};

class CTimeSchedData
{
public:
	DWORD dwID;
	TCHAR szName[50];
	int nTriggerType; //��һ���ԡ��ա��µȷ�ʽ
	FILETIME tmStartTime; //��ʼʱ��
	BOOL bEnableEndTime; //�Ƿ���Խ���
	FILETIME tmEndTime; //����ʱ��
	int nInterval; //���������Ϊ��λ��
	int iAction; //������ʽ
	DWORD dwDayOfWeek; 
	DWORD dwDayOfMonth;
	TCHAR szActionPara[256]; //��������
	TCHAR szDesc[100];
};

class CEventSchedData
{
public:
	DWORD dwID;
	TCHAR szName[50];
	TCHAR szExpression[256];
	int nTriggerType; //�Ա仯���桢�١������桢�����ٵȷ�ʽ
	int nInterval; //���������Ϊ��λ��
	int iAction; //������ʽ
	TCHAR szActionPara[256]; //��������
	TCHAR szDesc[100];
};

class CUserData
{
public:
	DWORD dwID;
	TCHAR szName[50];
	TCHAR szPassword[50];
	TCHAR szDesc[100];
	TCHAR szGroup[50];
	TCHAR szAccessSecrityAreas[256]; // �ɷ��ʵİ�ȫ��
	DWORD dwPrivilege;
};



#endif //__CXDATDEF_H_


