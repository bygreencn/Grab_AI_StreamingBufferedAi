// Configuration.cpp : implementation file
//

#include "stdafx.h"
#include "StreamingBufferedAi.h"
#include "ConfigurationDlg.h"
#include ".\XFolderDialog\XFolderDialog.h"

// CConfiguration dialog

IMPLEMENT_DYNAMIC ( CConfigurationDlg, CDialog )

int IDGroup1[] = 
{
    IDC_COMBO_DEVICE,
    IDC_COMBO_CHANSTART,
    IDC_COMBO_CHANCOUNT,
    IDC_COMBO_VALUERANGE,
    IDC_EDIT_CLOCKRATEPERCHAN,
    IDC_BUTTON_RECORDER_PATH,
    0
};
int IDGroup2[] = 
{
    IDC_BUTTON_REPLAY_PATH,
    0
};

CConfigurationDlg::CConfigurationDlg ( CWnd* pParent /*=NULL*/ )
    : CDialog ( CConfigurationDlg::IDD, pParent )
{

    m_deviceCount = 0;
    for ( int index = 0; index < 256; index++ )
    {
        m_deviceList[index] = -1;
    }
    m_clockRatePerChan = 1000;
    m_strRecorderPath = _T("");
    m_strReplayPath = _T("");
    m_nSelectMode = 0;
}

CConfigurationDlg::~CConfigurationDlg()
{
}

void CConfigurationDlg::DoDataExchange ( CDataExchange* pDX )
{
    CDialog::DoDataExchange ( pDX );
    DDX_Control ( pDX, IDC_COMBO_DEVICE, m_comboBox_Device );
    DDX_Control ( pDX, IDC_COMBO_CHANSTART, m_comboBox_ChannelStart );
    DDX_Control ( pDX, IDC_COMBO_CHANCOUNT, m_comboBox_ChannelCount );
    DDX_Control ( pDX, IDC_COMBO_VALUERANGE, m_comboBox_ValueRange );
    DDX_Text ( pDX, IDC_EDIT_CLOCKRATEPERCHAN, m_clockRatePerChan );
    DDV_MinMaxDouble ( pDX, m_clockRatePerChan, 1, 30000000 );
    DDX_Text(pDX, IDC_EDIT_RECORDER_PATH, m_strRecorderPath);
    DDX_Text(pDX, IDC_EDIT_REPLAY_PATH, m_strReplayPath);
    DDX_Radio(pDX, IDC_RADIO_GRAB_OR_REVIEW, m_nSelectMode);
}


BEGIN_MESSAGE_MAP ( CConfigurationDlg, CDialog )
    //{{AFX_MSG_MAP(CConfigurationDlg)
    ON_CBN_SELCHANGE ( IDC_COMBO_DEVICE, OnCbnSelchangeComboDevice )
    ON_BN_CLICKED ( IDOK, OnBnClickedOk )
    ON_BN_CLICKED ( IDCANCEL, OnBnClickedCancel )
    ON_WM_CLOSE()
    ON_EN_CHANGE ( IDC_EDIT_CLOCKRATEPERCHAN, OnEnChangeEditClockrateperchan )
	ON_BN_CLICKED(IDC_BUTTON_RECORDER_PATH, OnBnClickedButtonRecorderPath)
    ON_BN_CLICKED(IDC_BUTTON_REPLAY_PATH, OnBnClickedButtonReplayPath)
    ON_BN_CLICKED(IDC_RADIO_GRAB_OR_REVIEW, OnBnClickedRadioGrabOrReview)
    ON_BN_CLICKED(IDC_RADIO_GRAB_OR_REVIEW2, OnBnClickedRadioGrabOrReview2)
    //}}AFX_MSG_MAP
    
END_MESSAGE_MAP()

void CConfigurationDlg::CheckError ( ErrorCode errorCode )
{
    if ( errorCode != Success )
    {
        TCHAR	errorNumber[30]	=	{0};
        wsprintf ( errorNumber, _T ( "%X" ), errorCode );
        CString str = _T ( "Some errors happened, the error code is: 0x" ) ;
        str += errorNumber;
        AfxMessageBox ( str );
    }

}
BOOL CConfigurationDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    ErrorCode      err;
    BDaqDevice*    device = NULL;
    BDaqAi*        ai     = NULL;
    WCHAR          devicedescription[128] = {0};


    long subDeviceCount;
    WCHAR description[128];
    DOUBLE clockRate = 0;
    long index = 0;
    long deviceNumber = -2;
    HANDLE aiStopEventHandle = NULL;

    EnableControlWnd(IDGroup1);
    DisableControlWnd(IDGroup2);

    do
    {
        err = AdxDeviceGetLinkageInfo ( -1, index++, &deviceNumber, description, &subDeviceCount );
        if ( err == Success )
        {
            if ( device != NULL )
            {
                device->Close();
                ai = NULL;
                device = NULL;
            }
            if ( err == Success )
            {
                if ( Success == BDaqDevice::Open ( deviceNumber, ModeRead, device ) )
                {
                    if ( Success == device->GetModule ( 0, ai ) )
                    {
                        if ( Success == ai->Event.GetHandle ( EvtBufferedAiStopped, aiStopEventHandle ) )
                        {
                            m_deviceList[m_deviceCount++] = deviceNumber;
                            CString str = description;
                            m_comboBox_Device.AddString ( str );
                        }
                    }
                    device->Close();
                    device = NULL;
                }

            }
        }
    }
    while ( deviceNumber != -1 );

    if ( m_deviceCount == 0 )
    {
        GetDlgItem ( IDOK )->EnableWindow ( FALSE );
        GetDlgItem ( IDCANCEL )->EnableWindow ( FALSE );
        this->m_configureParameter.deviceNumber = -1;
        return FALSE;
    }
    for ( int i = 0; i < m_deviceCount; i++ )
    {
        if ( m_deviceList[i] == this->m_configureParameter.deviceNumber )
        {
            m_comboBox_Device.SetCurSel ( i );
            m_currentDeviceIndex = i;
            OnCbnSelchangeComboDevice();
            break;
        }
    }

    m_strRecorderPath = theApp.GetProfileString( _T("Path"), _T("Record Path"), _T(""));
	m_strReplayPath = theApp.GetProfileString(_T ( "Path" ), _T("Replay Path"), _T(""));
	UpdateData(FALSE);
    return TRUE;  // return TRUE unless you set the focus to a control
}

// CConfiguration message handlers

void CConfigurationDlg::OnCbnSelchangeComboDevice()
{
    // TODO: Add your control notification handler code here
    // TODO: Add your control notification handler code here
    if ( m_deviceCount >= 1 )
    {
        int				channelCount = 0;
        int            channelNumberMax = 0;
        int				index = m_comboBox_Device.GetCurSel();
        ErrorCode		errorCode;
        BDaqDevice*		device = NULL;
        BDaqAi*			ai	= NULL;
        MathInterval	ranges[128];

        m_comboBox_ChannelCount.ResetContent();
        m_comboBox_ChannelStart.ResetContent();
        m_comboBox_ValueRange.ResetContent();

        UpdateData ( TRUE );
        errorCode = BDaqDevice::Open ( m_deviceList[index], ModeRead, device );
        CheckError ( errorCode );
        errorCode = device->GetModule ( 0, ai );
        CheckError ( errorCode );

        errorCode = ai->Property.Get ( CFG_FeatureChannelNumberMax, channelNumberMax );
        CheckError ( errorCode );
        int i = 0;
        for ( i = 0; i <= channelNumberMax; i++ )
        {
            TCHAR channelIndex[10] = {0};
            CString str;
            wsprintf ( channelIndex, _T ( "%d" ), i );
            str = channelIndex;
            m_comboBox_ChannelStart.AddString ( str );
        }


        errorCode = ai->Property.Get ( CFG_ChannelCount, channelCount );
        CheckError ( errorCode );
        if ( channelCount > 16 )
        {
            channelCount = 16;
        }
        for ( i = 1; i < channelCount + 1; i++ )
        {
            TCHAR channelIndex[10] = {0};
            CString str;
            wsprintf ( channelIndex, _T ( "%d" ), i );
            str = channelIndex;
            m_comboBox_ChannelCount.AddString ( str );
        }

        int fetureVrgCount =  ai->Property.GetLength ( CFG_FeatureVrgTypes ) / sizeof ( int ); ////get the number of the type of Value Range in this device
        WCHAR	vrgDescription[128];
        errorCode = ai->Property.Get ( CFG_FeatureVrgTypes, fetureVrgCount, ( LONG* ) m_vrgTypes );
        CheckError ( errorCode );
        for ( i = 0; i < fetureVrgCount ; i++ )
        {
            errorCode = AdxGetValueRangeInformation ( m_vrgTypes[i], sizeof ( vrgDescription ), vrgDescription, &ranges[i], NULL );
            CheckError ( errorCode );
            CString str = vrgDescription;
            m_comboBox_ValueRange.AddString ( str );
        }


        if ( m_comboBox_Device.GetCurSel() == m_currentDeviceIndex )
        {
            for ( i = 0; i < fetureVrgCount; i++ )
            {
                if ( m_vrgTypes[i] == this->m_configureParameter.vrgType )
                {
                    m_comboBox_ValueRange.SetCurSel ( i );
                    break;
                }
            }

            m_comboBox_ChannelStart.SetCurSel ( m_configureParameter.channelStart );
            m_comboBox_ChannelCount.SetCurSel ( m_configureParameter.channelCount - 1 );
            m_clockRatePerChan = m_configureParameter.clockRatePerChan;
        }
        else
        {
            m_comboBox_ChannelStart.SetCurSel ( 0 );
            m_comboBox_ChannelCount.SetCurSel ( 2 );
            m_comboBox_ValueRange.SetCurSel ( 0 );
            m_clockRatePerChan = 1000;


        }
        UpdateData ( FALSE );
        device->Close();
        device = NULL;

    }

}

void CConfigurationDlg::OnBnClickedOk()
{
	UpdateData();
	theApp.WriteProfileString(_T ( "Path" ), _T("Record Path"), m_strRecorderPath);
	theApp.WriteProfileString(_T ( "Path" ),_T("Replay Path"), m_strReplayPath);

    // TODO: Add your control notification handler code here
    m_configureParameter.deviceNumber = m_deviceList[m_comboBox_Device.GetCurSel()];
    m_configureParameter.channelCount = m_comboBox_ChannelCount.GetCurSel() + 1;
    m_configureParameter.channelStart = m_comboBox_ChannelStart.GetCurSel();
    m_configureParameter.vrgType = m_vrgTypes[m_comboBox_ValueRange.GetCurSel()];
    m_configureParameter.clockRatePerChan = m_clockRatePerChan;
    this->m_configureParameter.isRefresh = true;
	if(m_strRecorderPath.IsEmpty())
	{
		AfxMessageBox(_T("需给出采集保存文件或要打开的文件路径."));
		return;
	}
    if ( m_configureParameter.isRefresh && m_deviceCount == 0 )
    {
        PostQuitMessage ( 0 );
    }
    OnOK();
}

void CConfigurationDlg::OnBnClickedCancel()
{
    this->m_configureParameter.isRefresh = false;
    this->CloseWindow();
    OnCancel();
}

void CConfigurationDlg::OnClose()
{
    this->m_configureParameter.isRefresh = false;
    if ( m_deviceCount <= 0 )
    {
        PostQuitMessage ( 0 );
    }
    this->CloseWindow();
    CDialog::OnClose();
}

void CConfigurationDlg::OnEnChangeEditClockrateperchan()
{
    // TODO:  If this is a RICHEDIT control, the control will not
    // send this notification unless you override the CDialog::OnInitDialog()
    // function and call CRichEditCtrl().SetEventMask()
    // with the ENM_CHANGE flag ORed into the mask.
    UpdateData();

    //UpdateData(FALSE);
    // TODO:  Add your control notification handler code here
}


void CConfigurationDlg::OnBnClickedButtonRecorderPath()
{
    CXFolderDialog dlg(m_strRecorderPath);
    dlg.SetOsVersion((CXFolderDialog::XFILEDIALOG_OS_VERSION) 2);
    dlg.EnableRegistry(TRUE);
	if(dlg.DoModal() == IDOK)
    {
        m_strRecorderPath = dlg.GetPath();
        UpdateData(FALSE);
    }
    /*
	CFileDialog dlg( FALSE , 
		_T("*.bin "),
		NULL,
		OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, 
		_T("Stream Grab Bin files(*.bin)|*.bin|| ")); 
	if(dlg.DoModal() == IDOK)
    {
        m_strRecorderPath = dlg.GetPathName();
        UpdateData(FALSE);
    }
	*/
}


void CConfigurationDlg::OnBnClickedButtonReplayPath()
{
    CFileDialog dlg( TRUE , 
		_T("*.bin "),
		NULL,
		OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, 
		_T("Stream Grab Bin files(*.bin)|*.bin|| ")); 
	if(dlg.DoModal() == IDOK)
    {
        m_strReplayPath = dlg.GetPathName();
        UpdateData(FALSE);
    }
}

void CConfigurationDlg::EnableControlWnd(int * groupid)
{
    for(int i=0;groupid[i]!=NULL;i++)
    {
		GetDlgItem(groupid[i])->EnableWindow(TRUE);
    }
}


void CConfigurationDlg::DisableControlWnd(int * groupid)
{
    for(int i=0;groupid[i]!=NULL;i++)
    {
        GetDlgItem(groupid[i])->EnableWindow(FALSE);
    }
}


void CConfigurationDlg::OnBnClickedRadioGrabOrReview()
{
    UpdateData();
    EnableControlWnd(IDGroup1);
    DisableControlWnd(IDGroup2);
    UpdateData(FALSE);
}


void CConfigurationDlg::OnBnClickedRadioGrabOrReview2()
{
    UpdateData();
    EnableControlWnd(IDGroup2);
    DisableControlWnd(IDGroup1);
    UpdateData(FALSE);
}
