#pragma once
#include "./SimpleGraph.h"

#include "../../../Inc/BDaqCL.h"
using namespace Automation::BDaq;

typedef struct DiveceConfigueInfomation
{
	int			deviceNumber;
	int			channelCount;
	int			channelStart;
	int			vrgType;
	double      clockRatePerChan;
	bool        isRefresh;

} ConfigureParameter;
// CConfiguration dialog

class CConfigurationDlg : public CDialog
{
	DECLARE_DYNAMIC ( CConfigurationDlg )

public:
	CConfigurationDlg ( CWnd* pParent = NULL ); // standard constructor
	virtual ~CConfigurationDlg();

// Dialog Data
	enum { IDD = IDD_CONFIGURATION };
private:
	CComboBox m_comboBox_Device;
	CComboBox m_comboBox_ChannelStart;
	CComboBox m_comboBox_ChannelCount;
	CComboBox m_comboBox_ValueRange;


	int m_deviceList[256];
	int m_deviceCount;
	int m_currentDeviceIndex;
	ValueRange		m_vrgTypes[128];

	double m_clockRatePerChan;
public:
	ConfigureParameter m_configureParameter;
public:
	// show the error code if Some errors happened
	void CheckError ( ErrorCode errorCode );
public:
	virtual BOOL OnInitDialog();
protected:
	virtual void DoDataExchange ( CDataExchange* pDX ); // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnCbnSelchangeComboDevice();
public:
	afx_msg void OnBnClickedOk();
public:
	afx_msg void OnBnClickedCancel();
public:
	afx_msg void OnClose();
public:
	afx_msg void OnEnChangeEditClockrateperchan();
	afx_msg void OnBnClickedButtonRecorderPath();
	afx_msg void OnBnClickedButtonReplayPath();
public:
	CString m_strReplayPath;
	CString m_strRecorderPath;
protected:
	void EnableControlWnd(int * groupid);
	void DisableControlWnd(int * groupid);
public:
	int m_nSelectMode;
	afx_msg void OnBnClickedRadioGrabOrReview();
	afx_msg void OnBnClickedRadioGrabOrReview2();
};
