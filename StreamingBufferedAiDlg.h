// StreamingAiDlg.h : header file
//

#pragma once
#include "ConfigurationDlg.h"
#include "SimpleGraph.h"

// CStreamingBufferedAiDlg dialog
class CStreamingBufferedAiDlg : public CDialog
{
// Construction
    static   UINT   CheckEventThread ( LPVOID pParam );
public:
    CStreamingBufferedAiDlg ( CWnd* pParent = NULL );	// standard constructor

// Dialog Data
    enum { IDD = IDD_STREAMINGAI_DIALOG };

protected:
    virtual void DoDataExchange ( CDataExchange* pDX );	// DDX/DDV support


// Implementation
protected:
	CString m_strFilePathName;
	int m_nGrabMode;
    HICON m_hIcon;
    //CBrush	m_bkBk;

    // Generated message map functions
    virtual BOOL OnInitDialog();
    afx_msg void OnPaint();
    afx_msg HCURSOR OnQueryDragIcon();
    DECLARE_MESSAGE_MAP()
    void ConfigurateDevice();
    void ConfigurateGraph();
    void InitListView();
    void CheckError ( ErrorCode error );
    SimpleGraph         m_simpleGraph;
    ConfigureParameter  m_configureParameter;
    BDaqDevice*         m_device;
    BDaqAi*             m_ai;
    DOUBLE*			    m_data;
    void *              m_dataRaw;
    TimeUnit m_timeUnit;
public:
    int m_rawDataSize;
    int m_sectionCount;
    EventId m_aiEventIds[4];
    HANDLE m_aiEventHandles[4];
    CWinThread * m_pThread;
    long m_channelNumberMax;
    long m_vrgTypes[128];
public:
    afx_msg void OnBnClickedButtonConfig();
public:
    afx_msg void OnBnClickedButtonStart();
public:
    afx_msg void OnBnClickedButtonPause();
public:
    afx_msg void OnBnClickedButtonStop();
public:
    afx_msg void OnClose();
public:
    afx_msg HBRUSH OnCtlColor ( CDC* pDC, CWnd* pWnd, UINT nCtlColor );
public:
    afx_msg void OnNMCustomdrawList ( NMHDR *pNMHDR, LRESULT *pResult );
public:
    afx_msg void OnNMReleasedcaptureSliderTimediv ( NMHDR *pNMHDR, LRESULT *pResult );
    afx_msg void OnBnClickedButtonExit();
};
