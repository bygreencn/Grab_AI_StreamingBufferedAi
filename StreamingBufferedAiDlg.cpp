// StreamingAiDlg.cpp : implementation file
//

#include "stdafx.h"
#include "StreamingBufferedAi.h"
#include "StreamingBufferedAiDlg.h"
#include "fstream.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CStreamingBufferedAiDlg dialog




CStreamingBufferedAiDlg::CStreamingBufferedAiDlg ( CWnd* pParent /*=NULL*/ )
	: CDialog ( CStreamingBufferedAiDlg::IDD, pParent )
	, m_nGrabMode(-1)
	, m_strFilePathName(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon ( IDR_MAINFRAME );
	ErrorCode err = Success;
	m_device = NULL;
	m_ai = NULL;
	m_data = NULL;
	m_pThread = NULL;


	this->m_configureParameter.channelStart = 0;
	this->m_configureParameter.channelStart = 0;
	this->m_configureParameter.channelCount = 1;// 3 channels are used by default.
	this->m_configureParameter.isRefresh = false;//the first use,need to refresh parameters.
	this->m_configureParameter.clockRatePerChan = 1000;
	long vrgTypes[128]  = {0};
	this->m_configureParameter.deviceNumber = -1;
	long subDeviceCount;
	WCHAR description[100];
	DOUBLE clockRate = 0;
	long index = 0;
	long deviceNumber = -2;
	HANDLE aiStopEventHandle = NULL;
	do
	{
		err = AdxDeviceGetLinkageInfo ( -1, index++, &deviceNumber, description, &subDeviceCount );
		if ( err == Success )
		{
			if ( Success == BDaqDevice::Open ( deviceNumber, ModeRead, m_device ) )
			{
				if ( Success == m_device->GetModule ( 0, m_ai ) )
				{
					if ( Success == m_ai->Event.GetHandle ( EvtBufferedAiStopped, aiStopEventHandle ) )
					{
						this->m_configureParameter.deviceNumber = deviceNumber;
						m_ai->Property.Get ( CFG_FeatureVrgTypes, 128, vrgTypes );
						this->m_configureParameter.vrgType = vrgTypes[0];
						m_device->Close();
						m_device = NULL;
						break;
					}

				}
				m_device->Close();
				m_device = NULL;
			}
		}

	}
	while ( deviceNumber != -1 );

	if ( -1 == this->m_configureParameter.deviceNumber )
	{
		AfxMessageBox ( _T ( "No device to support the currently demonstrated function!" ) );
	}
}

void CStreamingBufferedAiDlg::DoDataExchange ( CDataExchange* pDX )
{
	CDialog::DoDataExchange ( pDX );
	DDX_Control ( pDX, IDC_STATIC_GRAPH, m_simpleGraph );
}

BEGIN_MESSAGE_MAP ( CStreamingBufferedAiDlg, CDialog )
	//{{AFX_MSG_MAP(CStreamingBufferedAiDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED ( IDC_BUTTON_CONFIG, OnBnClickedButtonConfig )
	ON_BN_CLICKED ( IDC_BUTTON_START, OnBnClickedButtonStart )
	ON_BN_CLICKED ( IDC_BUTTON_PAUSE, OnBnClickedButtonPause )
	ON_BN_CLICKED ( IDC_BUTTON_STOP, OnBnClickedButtonStop )
	ON_WM_CLOSE()
	ON_WM_CTLCOLOR()
	ON_NOTIFY ( NM_CUSTOMDRAW, IDC_LIST, OnNMCustomdrawList )
	ON_NOTIFY ( NM_RELEASEDCAPTURE, IDC_SLIDER_TIMEDIV, OnNMReleasedcaptureSliderTimediv )
	ON_BN_CLICKED(IDC_BUTTON_EXIT, OnBnClickedButtonExit)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


// CStreamingBufferedAiDlg message handlers

BOOL CStreamingBufferedAiDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	GetDlgItem ( IDC_BUTTON_PAUSE )->EnableWindow ( FALSE );
	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon ( m_hIcon, TRUE );			// Set big icon
	SetIcon ( m_hIcon, FALSE );		// Set small icon

	//CBitmap			bmp;
	//bmp.LoadBitmap(IDB_BACKGROUND);                 //IDB_BCAKGROUND is the ID of the background picture
	//m_bkBk.CreatePatternBrush(&bmp);
	//bmp.DeleteObject();

	CConfigurationDlg configDlg;
	configDlg.m_configureParameter = this->m_configureParameter;
	if(IDOK == configDlg.DoModal())
	{
		m_nGrabMode = configDlg.m_nSelectMode;
		if(0 == m_nGrabMode)
		{
			m_strFilePathName = configDlg.m_strRecorderPath;
			if ( configDlg.m_configureParameter.deviceNumber == -1 )
			{
				this->CloseWindow();
				return FALSE;
			}
			if ( configDlg.m_configureParameter.isRefresh )
			{
				this->m_configureParameter = configDlg.m_configureParameter;
			}
			else
			{
				this->CloseWindow();
				PostQuitMessage ( 0 );
				return FALSE;
			}
			ConfigurateDevice();
		}
		else if(1 == m_nGrabMode)
		{
			m_strFilePathName = configDlg.m_strReplayPath;
		}
		else
		{
			this->CloseWindow();
			PostQuitMessage ( 0 );
			return FALSE;
		}
		ConfigurateGraph();
		InitListView();
		CRect rc;
		CWnd *pWnd = GetDlgItem ( IDC_STATIC_GRAPH );
		pWnd->GetClientRect ( &rc );
		m_simpleGraph.InitGraph();
	}
	else
	{
		this->CloseWindow();
		PostQuitMessage ( 0 );
	}
	return FALSE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CStreamingBufferedAiDlg::OnPaint()
{
	if ( IsIconic() )
	{
		CPaintDC dc ( this ); // device context for painting

		SendMessage ( WM_ICONERASEBKGND, reinterpret_cast<WPARAM> ( dc.GetSafeHdc() ), 0 );

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics ( SM_CXICON );
		int cyIcon = GetSystemMetrics ( SM_CYICON );
		CRect rect;
		GetClientRect ( &rect );
		int x = ( rect.Width() - cxIcon + 1 ) / 2;
		int y = ( rect.Height() - cyIcon + 1 ) / 2;

		// Draw the icon
		dc.DrawIcon ( x, y, m_hIcon );
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CStreamingBufferedAiDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR> ( m_hIcon );
}

void CStreamingBufferedAiDlg::CheckError ( ErrorCode error )
{
	if ( error != Success )
	{
		TCHAR	errorNumber[30]	=	{0};
		wsprintf ( errorNumber, _T ( "%X" ), error );
		CString str = _T ( "Some errors happened, the error code is: 0x" ) ;
		str += errorNumber;
		AfxMessageBox ( str );

		this->m_configureParameter.isRefresh = false;//charge if refresh the parameter with this bool value.
		CConfigurationDlg dlg;
		dlg.m_configureParameter = this->m_configureParameter;
		dlg.DoModal();
		if ( dlg.m_configureParameter.isRefresh )
		{
			this->m_configureParameter = dlg.m_configureParameter;
			ConfigurateDevice();
			ConfigurateGraph();
		}
		else
		{
			exit ( EXIT_FAILURE );
		}
	}
}
void CStreamingBufferedAiDlg::ConfigurateDevice()
{
	WCHAR		desription[128] = {0};
	ErrorCode	errorCode;

	if ( m_device != NULL )
	{
		m_device->Close();
		m_device = NULL;
		m_ai = NULL;
	}

	errorCode = BDaqDevice::Open ( this->m_configureParameter.deviceNumber, ModeWriteWithReset, m_device );
	CheckError ( errorCode );
	errorCode = m_device->GetModule ( 0, m_ai );
	CheckError ( errorCode );

	errorCode = m_device->Property.Get ( CFG_Description, 128, desription );
	CheckError ( errorCode );
	CString str = _T ( "Streaming Buffered AI - Run" );
	str += _T ( "(" );
	str += desription;
	str += _T ( ")" );
	SetWindowText ( str );


	// get rawDataSize.
	errorCode = m_ai->Property.Get ( CFG_FeatureDataSize, m_rawDataSize );
	CheckError ( errorCode );

	// get section count and to calculate the data count of per-channel.
	m_sectionCount = ( int ) ( this->m_configureParameter.clockRatePerChan * this->m_configureParameter.channelCount / 4 );
	errorCode = m_ai->Property.Set ( CFG_SectionDataCount, m_sectionCount );
	CheckError ( errorCode );
	if ( m_data != NULL )
	{
		delete [] m_data;
		m_data = NULL;
	}
	m_data = new DOUBLE[m_sectionCount * 4];// re-allocate array if channel count has changed.
	if ( m_data == NULL )
	{
		AfxMessageBox ( _T ( "Sorry! Error in allocating memory...." ) );
		this->CloseWindow();
		return;
	}

	//get channel max number.
	errorCode = m_ai->Property.Get ( CFG_FeatureChannelNumberMax, m_channelNumberMax );
	CheckError ( errorCode );

	//set value range for every channel.

	int i = 0;
	for ( i = 0; i < m_channelNumberMax + 1; i++ )
	{
		m_vrgTypes[i] = this->m_configureParameter.vrgType;
	}
	errorCode = m_ai->Property.Set ( CFG_VrgTypeOfChannels, m_channelNumberMax + 1, m_vrgTypes );
	CheckError ( errorCode );

	errorCode = m_ai->Property.Set ( CFG_ScanChannelStart, this->m_configureParameter.channelStart );
	CheckError ( errorCode );

	errorCode = m_ai->Property.Set ( CFG_ScanChannelCount, this->m_configureParameter.channelCount );
	CheckError ( errorCode );

	errorCode = m_ai->Property.Set ( CFG_ConvertClockRatePerChannel, ( DOUBLE ) this->m_configureParameter.clockRatePerChan );
	CheckError ( errorCode );

	errorCode = m_ai->Property.Set ( CFG_VrgTypeOfChannels, m_channelNumberMax + 1, m_vrgTypes );
	CheckError ( errorCode );

	errorCode = m_ai->BfdAiPrepare ( m_sectionCount * 4, &m_dataRaw );
	CheckError ( errorCode );

	m_aiEventIds[0] = EvtBufferedAiOverrun;
	m_aiEventIds[1] = EvtBufferedAiDataReady;
	m_aiEventIds[2] = EvtBufferedAiStopped;
	m_aiEventIds[3] = EvtBufferedAiCacheOverflow;

	for ( i = 0; i < 4; ++i )
	{
		errorCode = m_ai->Event.GetHandle ( m_aiEventIds[i], m_aiEventHandles[i] );
		CheckError ( errorCode );
	}
}
void CStreamingBufferedAiDlg::ConfigurateGraph()
{
	m_timeUnit = Millisecond;
	CRect rc;
	CWnd *pWnd = GetDlgItem ( IDC_STATIC_GRAPH );
	pWnd->GetClientRect ( &rc );

	CSliderCtrl* pdivSlider = ( CSliderCtrl* ) GetDlgItem ( IDC_SLIDER_TIMEDIV );
	pdivSlider->SetRangeMax ( 10000000 );
	pdivSlider->SetRangeMin ( 0 );
	int divValue = ( int ) ( 1.0 * 100 * rc.Width() / this->m_configureParameter.clockRatePerChan );

	if ( this->m_configureParameter.clockRatePerChan >= 10 * 1000 )
	{
		divValue = ( int ) ( 1.0 * 100 * rc.Width() / ( this->m_configureParameter.clockRatePerChan / 1000 ) );
		m_timeUnit = Microsecond;
	}
	pdivSlider->SetRangeMax ( 4 * divValue );
	int divMin = divValue / 10;
	if ( divMin == 0 )
	{
		divMin = 1;
	}
	pdivSlider->SetRangeMin ( divMin );
	pdivSlider->SetPos ( divValue );
	UpdateData ( FALSE );
	//set parameters for plot.
	m_simpleGraph.m_XCordTimeDiv = divValue;

	CString X_rangeLabels[2];
	DOUBLE shiftTime = this->m_simpleGraph.m_XCordTimeOffset;
	m_simpleGraph.GetXCordRangeLabels ( X_rangeLabels, m_simpleGraph.m_XCordTimeDiv * 10 + shiftTime, shiftTime, m_timeUnit );
	( ( CStatic* ) GetDlgItem ( IDC_STATIC_ENDTIME ) )->SetWindowText ( X_rangeLabels[0] );
	( ( CStatic* ) GetDlgItem ( IDC_STATIC_STARTTIME ) )->SetWindowText ( X_rangeLabels[1] );

	ValueUnit	  unit;
	MathInterval rangeY ;
	if ( ( ValueRange ) this->m_configureParameter.vrgType < Jtype_0To760C )
	{
		CheckError ( AdxGetValueRangeInformation ( ( ValueRange ) this->m_configureParameter.vrgType, 0, NULL, &rangeY, &unit ) );
		if ( Milliampere == unit  || Millivolt == unit )
		{
			rangeY.Max /= 1000;
			rangeY.Min /= 1000;
		}
	}
	else
	{
		unit = CelsiusUnit;
		switch ( ( ValueRange ) this->m_configureParameter.vrgType )
		{
		case Jtype_0To760C:
			rangeY.Max = 760;
			rangeY.Min = 0;
			break;
		case Ktype_0To1370C:
			rangeY.Max = 1370;
			rangeY.Min = 0;
			break;
		case Ttype_Neg100To400C:
			rangeY.Max = 400;
			rangeY.Min = -100;
			break;
		case Etype_0To1000C:
			rangeY.Max = 1000;
			rangeY.Min = 0;
			break;
		case Rtype_500To1750C:
		case Stype_500To1750C:
			rangeY.Max = 1750;
			rangeY.Min = 500;
			break;
		case Btype_500To1800C:
			rangeY.Max = 1800;
			rangeY.Min = 500;
			break;
		default:
			rangeY.Max = 100;
			rangeY.Min = 0;
			break;
		}
	}

	CString Y_rangeLabels[3];
	m_simpleGraph.GetYCordRangeLabels ( Y_rangeLabels, rangeY.Max, rangeY.Min, unit );
	( ( CStatic* ) GetDlgItem ( IDC_STATIC_YRANGEMAX ) )->SetWindowText ( Y_rangeLabels[0] );
	( ( CStatic* ) GetDlgItem ( IDC_STATIC_YRANGEMID ) )->SetWindowText ( Y_rangeLabels[2] );
	( ( CStatic* ) GetDlgItem ( IDC_STATIC_YRANGEMIN ) )->SetWindowText ( Y_rangeLabels[1] );
	m_simpleGraph.m_YCordRangeMax = rangeY.Max;
	m_simpleGraph.m_YCordRangeMin = rangeY.Min;

	m_simpleGraph.m_overOneScreenMode = EndScreen;
}

void CStreamingBufferedAiDlg::InitListView()
{
	CListCtrl*	pcolorList = ( CListCtrl* ) this->GetDlgItem ( IDC_LIST );
	while ( pcolorList->DeleteColumn ( 0 ) );
	pcolorList->DeleteAllItems();
	pcolorList->SetExtendedStyle ( LVS_EX_GRIDLINES );
	pcolorList->SetExtendedStyle ( pcolorList->GetExtendedStyle() );

	CImageList imagelist;
	imagelist.Create ( IDB_BITMAP_LIST, 1, 12, RGB ( 255, 255, 0 ) );
	pcolorList->SetImageList ( &imagelist, LVSIL_SMALL );

	int row = 2;
	int column = 8;        ////Notice, we use '8' for the number of items in a color list row
	int i = 0;
	CString strText = _T ( " " );
	for ( i = 0; i < column; i++ )
	{
		pcolorList->InsertColumn ( i, _T ( " " ), LVCFMT_LEFT, 50 );
	}
	for ( i = 0; i < row; i++ )
	{
		pcolorList->InsertItem ( i, _T ( " " ) );

		for ( int j = 0; j < column; j++ )
		{
			if(i*8+j<m_configureParameter.channelCount)
				strText.Format(" %d ",m_configureParameter.channelStart+i*8+j);
			else
				strText=_T ( " " );
			pcolorList->SetItemText ( i, j, strText);
		}
		pcolorList->SetItemData ( i, i );
	}
}
void CStreamingBufferedAiDlg::OnBnClickedButtonConfig()
{
	// TODO: Add your control notification handler code here
	m_ai->BfdAiStop();
	m_simpleGraph.Clear();
	m_configureParameter.isRefresh = FALSE;
	CConfigurationDlg dlg;
	dlg.m_configureParameter = this->m_configureParameter;
	dlg.DoModal();
	if ( dlg.m_configureParameter.isRefresh )
	{
		if ( m_data != NULL )
		{
			delete [] m_data;
			m_data = NULL;
		}
		this->m_configureParameter = dlg.m_configureParameter;
		ConfigurateDevice();
		ConfigurateGraph();
		InitListView();
	}
}

void CStreamingBufferedAiDlg::OnBnClickedButtonStart()
{
	if(0 == m_nGrabMode)
	{
		GetDlgItem ( IDC_BUTTON_START )->EnableWindow ( FALSE );
		GetDlgItem ( IDC_BUTTON_CONFIG )->EnableWindow ( FALSE );
		GetDlgItem ( IDC_BUTTON_PAUSE )->EnableWindow ( TRUE );

		ErrorCode errorCode = Success;
		errorCode = m_ai->BfdAiRun();
		CheckError ( errorCode );
		m_pThread = AfxBeginThread ( CheckEventThread, this );
	}
	else if(0 == m_nGrabMode)
	{
		GetDlgItem ( IDC_BUTTON_START )->EnableWindow ( FALSE );
		GetDlgItem ( IDC_BUTTON_CONFIG )->EnableWindow ( FALSE );
		GetDlgItem ( IDC_BUTTON_PAUSE )->EnableWindow ( TRUE );
	}
}
UINT CStreamingBufferedAiDlg::CheckEventThread ( LPVOID pParam )
{

	CStreamingBufferedAiDlg * streamingAi = ( CStreamingBufferedAiDlg * ) pParam;

	ErrorCode err = Success;
	int readyDataCount = 0;
	LONG channelOffset  = 0;
	LONG readyDataOffset = 0;   //unit in sample data
	int  waitRet = 0;

	fstream fout;
	/*
	CString filename = streamingAi->m_strFilePathName;
	filename = filename.Left(filename.ReverseFind('.'));
	CString fileext = streamingAi->m_strFilePathName;
	fileext = fileext.Right(fileext.GetLength()-fileext.ReverseFind('.'));

	CString strFilePathName = _T("");

	SYSTEMTIME time;
	GetSystemTime(&time);


	strFilePathName.Format(_T("%s_%d%02d%02d-%02d%02d%02d%s"),
		filename,
		time.wYear,
		time.wMonth,
		time.wDay,
		time.wHour,
		time.wMinute,
		time.wSecond,
		fileext);
	*/
	CString strFilePathName;
	SYSTEMTIME time;
	GetLocalTime(&time);
	strFilePathName.Format(_T("%s\\%d%02d%02d-%02d%02d%02d.bin"),
		streamingAi->m_strFilePathName,
		time.wYear,
		time.wMonth,
		time.wDay,
		time.wHour,
		time.wMinute,
		time.wSecond);


	fout.open(strFilePathName,ios::out|ios::binary);
	if(!fout.is_open())
	{
		AfxMessageBox(_T("Open grab file failed!"));
		return 1;
	}
	fout.write((const char *)&streamingAi->m_configureParameter,sizeof(ConfigureParameter));
	fout.flush();
	do
	{
		waitRet = WaitForMultipleObjects ( 3, streamingAi->m_aiEventHandles, FALSE, 1000 );
		if ( waitRet < 3 ) //some AI event occurred
		{
			switch ( streamingAi->m_aiEventIds[waitRet] )
			{
			case EvtBufferedAiOverrun:
			{
				AfxMessageBox ( _T ( "BufferedAiOverrun" ) );
			}
			break;
			case EvtBufferedAiCacheOverflow:
			{
				AfxMessageBox ( _T ( "BufferedAiCacheOverflow" ) );
			}
			break;
			case EvtBufferedAiDataReady:
			{
				//the count of newly data is not always equal to the section data count
				readyDataCount = streamingAi->m_ai->Event.GetStatus ( EvtBufferedAiDataReady, readyDataOffset );
				int lastDataCount = 0;
				if ( readyDataCount != 0 )
				{
					//ensure that the data is begin with the start channel.
					readyDataCount -= ( int ) readyDataCount % ( int ) streamingAi->m_configureParameter.channelCount;


					double* dataScale1 = streamingAi->m_data;

					void* raw = ( ( ( byte* ) streamingAi->m_dataRaw ) + readyDataOffset * streamingAi->m_rawDataSize );
					if ( readyDataOffset + readyDataCount > streamingAi->m_sectionCount * 4 )
					{
						lastDataCount = ( int ) ( streamingAi->m_sectionCount * 4 - readyDataOffset );

						err = streamingAi->m_ai->BfdAiScaleData ( raw, dataScale1, lastDataCount, &channelOffset );

						double* dataScale2 = & streamingAi->m_data[lastDataCount];
						err = streamingAi->m_ai->BfdAiScaleData ( ( void* ) ( ( byte* ) streamingAi->m_dataRaw ),
								dataScale2,
								readyDataCount - lastDataCount,
								&channelOffset );
					}
					else
					{
						err = streamingAi->m_ai->BfdAiScaleData ( raw, dataScale1, readyDataCount, &channelOffset );
					}
					
					fout.write((const char *)streamingAi->m_data,readyDataCount*4);
					fout.flush();
					
					streamingAi->m_simpleGraph.Chart ( streamingAi->m_data,
													   streamingAi->m_configureParameter.channelCount,
													   readyDataCount / streamingAi->m_configureParameter.channelCount,
													   1.0 / streamingAi->m_configureParameter.clockRatePerChan );
					//tell the driver how many data has been handled
					streamingAi->m_ai->Event.ClearFlag ( EvtBufferedAiDataReady, readyDataCount, 0 );
				}
			}
			break;
			case EvtBufferedAiStopped:
			{
				//MessageBox.Show("EvtBufferedAiStopped");
			}
			break;
			}
		}
		else if ( waitRet == WAIT_OBJECT_0 )
		{
			AfxMessageBox ( _T ( "WaitTimeout!" ) );
		}
	}
	while ( true );

	fout.close();
}

void CStreamingBufferedAiDlg::OnBnClickedButtonPause()
{
	// TODO: Add your control notification handler code here
	GetDlgItem ( IDC_BUTTON_START )->EnableWindow ( TRUE );
	GetDlgItem ( IDC_BUTTON_CONFIG )->EnableWindow ( FALSE );
	GetDlgItem ( IDC_BUTTON_PAUSE )->EnableWindow ( FALSE );
	//GetDlgItem(IDC_SLIDER_TIMEDIV)->EnableWindow(FALSE);

	TerminateThread ( m_pThread->m_hThread, 0 );
	m_ai->BfdAiStop();
}

void CStreamingBufferedAiDlg::OnBnClickedButtonStop()
{
	if(0 == m_nGrabMode)
	{
		GetDlgItem ( IDC_BUTTON_START )->EnableWindow ( TRUE );
		GetDlgItem ( IDC_BUTTON_CONFIG )->EnableWindow ( TRUE );
		GetDlgItem ( IDC_BUTTON_PAUSE )->EnableWindow ( FALSE );
		GetDlgItem ( IDC_SLIDER_TIMEDIV )->EnableWindow ( TRUE );

		if ( m_pThread != NULL )
		{
			TerminateThread ( m_pThread->m_hThread, 0 );
		}

		m_ai->BfdAiStop();
		this->m_simpleGraph.Clear();
	}
	else if(1 == m_nGrabMode)
	{
		GetDlgItem ( IDC_BUTTON_START )->EnableWindow ( TRUE );
		GetDlgItem ( IDC_BUTTON_CONFIG )->EnableWindow ( TRUE );
		GetDlgItem ( IDC_BUTTON_PAUSE )->EnableWindow ( FALSE );
		GetDlgItem ( IDC_SLIDER_TIMEDIV )->EnableWindow ( TRUE );
	}
}

void CStreamingBufferedAiDlg::OnClose()
{
	// TODO: Add your message handler code here and/or call default
	if ( m_pThread != NULL )
	{
		TerminateThread ( m_pThread->m_hThread, 0 );
	}

	if ( m_ai != NULL )
	{
		m_ai->BfdAiStop();
		m_ai = NULL;
	}
	if ( m_data != NULL )
	{
		delete [] m_data;
		m_data = NULL;
	}
	CDialog::OnClose();
}

HBRUSH CStreamingBufferedAiDlg::OnCtlColor ( CDC* pDC, CWnd* pWnd, UINT nCtlColor )
{
	HBRUSH hbr = CDialog::OnCtlColor ( pDC, pWnd, nCtlColor );

	// TODO:  Change any attributes of the DC here
	//if (pWnd == this)
	//{
	//	return (HBRUSH)m_bkBk;
	//}
	// TODO:  Return a different brush if the default is not desired
	return hbr;
}

void CStreamingBufferedAiDlg::OnNMCustomdrawList ( NMHDR *pNMHDR, LRESULT *pResult )
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW> ( pNMHDR );
	// TODO: Add your control notification handler code here
	LPNMLVCUSTOMDRAW	lplvcd	= ( LPNMLVCUSTOMDRAW ) pNMHDR;
	int					row		   = 0 ;
	switch ( lplvcd->nmcd.dwDrawStage )
	{
	case CDDS_PREPAINT :
		*pResult = CDRF_NOTIFYITEMDRAW;
		break;
	case CDDS_ITEMPREPAINT:
		*pResult = CDRF_NOTIFYSUBITEMDRAW;
		break;
	case CDDS_SUBITEM | CDDS_ITEMPREPAINT:
		row = ( int ) ( lplvcd->nmcd.lItemlParam );
		if ( 8 * row + lplvcd->iSubItem < m_configureParameter.channelCount )
		{
			lplvcd->clrTextBk  = m_simpleGraph.LinesColor[row * 8 + lplvcd->iSubItem];
			*pResult = CDRF_NEWFONT;
		}
		else
		{
			lplvcd->clrTextBk = RGB ( 255, 255, 255 );
			*pResult = CDRF_NEWFONT;
		}
		break;
	default:
		*pResult = CDRF_DODEFAULT;
		break;
	}
	//*pResult = 0;
}

void CStreamingBufferedAiDlg::OnNMReleasedcaptureSliderTimediv ( NMHDR *pNMHDR, LRESULT *pResult )
{
	// TODO: Add your control notification handler code here
	CSliderCtrl* pdivSlider = ( CSliderCtrl* ) GetDlgItem ( IDC_SLIDER_TIMEDIV );
	m_simpleGraph.Div ( pdivSlider->GetPos() );
	//m_simpleGraph.Clear();

	CString ranges[2];
	double shiftTime = this->m_simpleGraph.m_XCordTimeOffset;
	m_simpleGraph.GetXCordRangeLabels ( ranges, m_simpleGraph.m_XCordTimeDiv * 10 + shiftTime, shiftTime, m_timeUnit );
	( ( CStatic* ) GetDlgItem ( IDC_STATIC_ENDTIME ) )->SetWindowText ( ranges[0] );
	( ( CStatic* ) GetDlgItem ( IDC_STATIC_STARTTIME ) )->SetWindowText ( ranges[1] );
}


void CStreamingBufferedAiDlg::OnBnClickedButtonExit()
{
	EndDialog(IDOK);
}
