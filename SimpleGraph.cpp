// SimpleGraph.cpp :
//

#include "stdafx.h"
#include <cmath>
#include "SimpleGraph.h"
using namespace std;
#pragma warning(disable:4996)
#pragma warning(disable:4786)

COLORREF SimpleGraph::LinesColor[16] = { RGB ( 255, 0, 0 ), RGB ( 0, 255, 0 ), RGB ( 185, 99, 188 ), RGB ( 255, 0, 255 ), RGB ( 0, 255, 255 ),
                                       RGB ( 255, 255, 0 ), RGB ( 155, 55, 255 ), RGB ( 255, 127, 0 ), RGB ( 106, 147, 219 ), RGB ( 209, 146, 117 ),
                                       RGB ( 143, 188, 143 ), RGB ( 245, 182, 204 ), RGB ( 40, 193, 164 ), RGB ( 165, 228, 64 ), RGB ( 204, 150, 53 ),
                                       RGB ( 236, 228, 137 )
                                       };

// SimpleGraph
IMPLEMENT_DYNAMIC ( SimpleGraph, CStatic )

SimpleGraph::SimpleGraph()
{

    m_plotCount = 0;
    m_copyDataCountPerChan = 0;
    m_pointCountPerScreen = 0;
    m_dataCountCachePerPlot = 0;
    m_dataCountPerPlot = 0;
    m_mapDataIndexPerPlot = 0;
    m_xIncByTime = 0;
    m_shiftCount = 0;

    m_XCordTimeOffset = 0;
    m_XCordDividedRate = 0;
    m_YCordRangeMax = 0;
    m_YCordRangeMin = 0;

    m_dataPointBuffer = NULL;
    m_drawDataBuffer = NULL;

    m_overOneScreenMode = BeginScreen;
    InitializeCriticalSection ( &criticalSection );
}


SimpleGraph::~SimpleGraph()
{
    if ( m_drawDataBuffer != NULL )
    {
        delete []m_drawDataBuffer;
        m_drawDataBuffer = NULL;
    }
    if ( m_dataPointBuffer != NULL )
    {
        delete []m_dataPointBuffer;
        m_dataPointBuffer = NULL;
    }
}


BEGIN_MESSAGE_MAP ( SimpleGraph, CStatic )
    ON_WM_PAINT()
END_MESSAGE_MAP()



void SimpleGraph::InitGraph()
{
    CDC*	 pDC = GetDC();
    GetClientRect ( &m_rect );
    m_memDc.DeleteDC();
    m_bitmap.DeleteObject();
    m_memDc.CreateCompatibleDC ( pDC );
    m_bitmap.CreateCompatibleBitmap ( pDC, m_rect.Width(), m_rect.Height() );
    ReleaseDC ( pDC );
}

void SimpleGraph::OnPaint()
{
    CPaintDC dc ( this ); // device context for painting
    CPen		   pen;
    pen.CreatePen ( PS_SOLID, 1, RGB ( 0, 140, 0 ) );
    CPen*		oldPen = m_memDc.SelectObject ( &pen );
    CBitmap*	oldMap = m_memDc.SelectObject ( &m_bitmap );
    m_memDc.FillSolidRect ( 0, 0, m_rect.Width(), m_rect.Height(), RGB ( 0, 0, 0 ) );
    for ( int x = ( m_rect.Width() / 10 ); x < m_rect.Width(); )
    {

        m_memDc.MoveTo ( x, m_rect.top );
        m_memDc.LineTo ( x, m_rect.bottom );
        x += m_rect.Width() / 10;
    }
    for ( int y = m_rect.Height() / 10; y < m_rect.Height(); )
    {
        m_memDc.MoveTo ( m_rect.left, y );
        m_memDc.LineTo ( m_rect.right, y );
        y += m_rect.Height() / 10;
    }

    pen.DeleteObject();
    POINT* pstep = m_dataPointBuffer;
    for ( int i = 0; i < m_plotCount; i++ )
    {
        pen.CreatePen ( PS_SOLID, 1, LinesColor[i] );
        m_memDc.SelectObject ( &pen );
        m_memDc.Polyline ( pstep, m_copyDataCountPerChan );
        pen.DeleteObject();
        pstep = pstep + m_rect.Width() * 4 + 1;
    }
    pstep = m_dataPointBuffer;
    dc.BitBlt ( 0, 0, m_rect.Width(), m_rect.Height(), &m_memDc, 0, 0, SRCCOPY );
    m_memDc.SelectObject ( oldPen );
    m_memDc.SelectObject ( oldMap );
}


void SimpleGraph::Chart ( DOUBLE* plotData, int plotCount, int dataCountPerPlot, double xIncBySec )
{
    m_xIncByTime = xIncBySec;
    m_dataCountPerPlot = dataCountPerPlot;

    EnterCriticalSection ( &criticalSection );
    if ( NULL == m_drawDataBuffer || plotCount != m_plotCount )
    {
        if ( m_drawDataBuffer != NULL )
        {
            delete []m_drawDataBuffer;
            m_drawDataBuffer = NULL;
        }
        m_drawDataBuffer = new DOUBLE[plotCount * ( m_rect.Width() * 4 + 1 )];
        m_drawDataBufferLength = plotCount * ( m_rect.Width() * 4 + 1 );
        if ( m_drawDataBuffer == NULL )
        {
            AfxMessageBox ( _T ( "Sorry! Error in allocating memory...." ) );
            this->CloseWindow();
            return;
        }

        if ( m_dataPointBuffer != NULL )
        {
            delete []m_dataPointBuffer;
            m_dataPointBuffer = NULL;
        }
        m_dataPointBuffer = new POINT[plotCount * ( m_rect.Width() * 4 + 1 )];
        if ( m_dataPointBuffer == NULL )
        {
            AfxMessageBox ( _T ( "Sorry! Error in allocating memory...." ) );
            this->CloseWindow();
            return;
        }

        m_dataCountCachePerPlot = 0;
        m_plotCount = plotCount;
    }
    CalcDrawParams ( m_xIncByTime, m_dataCountPerPlot );
    SaveData ( plotData, plotCount, dataCountPerPlot );
    MapDataPoints();
    Invalidate ( FALSE );
    LeaveCriticalSection ( &criticalSection );
}


void SimpleGraph::CalcDrawParams ( double XIncBySec, int dataCountPerPlot )
{
    m_shiftCount = ( int ) ( m_XCordTimeOffset * 1.0 / ( XIncBySec * 1000 ) );
    DOUBLE XcoordinateDivBase = m_rect.Width() * XIncBySec * 100.0;//ms
    if ( XIncBySec * 10 * 1000 <= 1 ) //us
    {
        m_shiftCount = ( int ) ( m_shiftCount / 1000 );
        XcoordinateDivBase = XcoordinateDivBase * 1000.0;
    }
    m_XCordDividedRate = XcoordinateDivBase / m_XCordTimeDiv;
    m_pointCountPerScreen = ( int ) ceil ( m_rect.Width() * m_XCordTimeDiv / XcoordinateDivBase ) + 1; //
    if ( EndScreen == m_overOneScreenMode && m_dataCountPerPlot > m_pointCountPerScreen )
    {
        m_mapDataIndexPerPlot = ( m_dataCountPerPlot - m_pointCountPerScreen - 1 );
    }
}


void SimpleGraph::SaveData ( DOUBLE * data, int plotCount, int dataCountPerPlot )
{
    if ( dataCountPerPlot * plotCount  > m_drawDataBufferLength )
    {
        if ( m_drawDataBuffer != NULL )
        {
            delete []m_drawDataBuffer;
            m_drawDataBuffer = NULL;
        }
        m_drawDataBuffer = new DOUBLE[ ( dataCountPerPlot + 1 ) * m_plotCount];
        m_drawDataBufferLength = ( dataCountPerPlot + 1 ) * m_plotCount;
        if ( m_drawDataBuffer == NULL )
        {
            AfxMessageBox ( _T ( "Sorry! Error in allocating memory...." ) );
            this->CloseWindow();
            return;
        }
    }

    if ( dataCountPerPlot >= m_pointCountPerScreen )
    {
        memcpy ( m_drawDataBuffer, data, plotCount * dataCountPerPlot * sizeof ( DOUBLE ) );
        m_dataCountCachePerPlot = dataCountPerPlot;
    }
    else
    {
        if ( m_dataCountCachePerPlot + dataCountPerPlot <= m_pointCountPerScreen )
        {
            memcpy ( m_drawDataBuffer + m_dataCountCachePerPlot * plotCount, data, plotCount * dataCountPerPlot * sizeof ( DOUBLE ) );
            m_dataCountCachePerPlot += dataCountPerPlot;
        }
        else
        {
            int overflowCount = plotCount * ( m_dataCountCachePerPlot + dataCountPerPlot - m_pointCountPerScreen );
            memmove ( m_drawDataBuffer, m_drawDataBuffer + overflowCount, ( plotCount * m_dataCountCachePerPlot - overflowCount ) *sizeof ( DOUBLE ) );
            memcpy ( m_drawDataBuffer + plotCount * m_dataCountCachePerPlot - overflowCount, data, plotCount * dataCountPerPlot * sizeof ( DOUBLE ) );
            m_dataCountCachePerPlot = m_pointCountPerScreen;
            m_mapDataIndexPerPlot = 0;
        }
    }
}

void SimpleGraph::MapDataPoints( )
{
    DOUBLE YCordDividedRate = 1.0 * ( m_rect.Height() - 1 ) / ( m_YCordRangeMax - m_YCordRangeMin );
    int count = m_dataCountCachePerPlot - ( int ) m_shiftCount;
    int index = 0;
    m_copyDataCountPerChan = count > m_pointCountPerScreen ? m_pointCountPerScreen : count;
    for ( int offset = m_mapDataIndexPerPlot; offset < m_mapDataIndexPerPlot + m_copyDataCountPerChan; offset++ )
    {
        index = offset - m_mapDataIndexPerPlot;
        for ( int i = 0; i < m_plotCount; i++ )
        {
            m_dataPointBuffer[ i * ( m_rect.Width() * 4 + 1 ) + index ].y = ( LONG ) ceil ( YCordDividedRate * ( m_YCordRangeMax - m_drawDataBuffer[ ( int ) ( m_plotCount * ( offset + m_shiftCount ) + i )] ) );
            m_dataPointBuffer[ i * ( m_rect.Width() * 4 + 1 ) + index ].x = ( LONG ) ( ( offset - m_mapDataIndexPerPlot ) * m_XCordDividedRate );
        }
    }
}


void SimpleGraph::Clear ( void )
{
    m_copyDataCountPerChan = 0;
    m_dataCountCachePerPlot = 0;
    Invalidate ( FALSE );
}

void SimpleGraph::Shift ( int shitfTime )
{
    m_XCordTimeOffset = shitfTime;
    Draw();
}

void SimpleGraph::Div ( int divTime )
{
    EnterCriticalSection ( &criticalSection );
    m_XCordTimeDiv = divTime;
    Draw();
    LeaveCriticalSection ( &criticalSection );
}

void SimpleGraph::Draw()
{
    CalcDrawParams ( m_xIncByTime, m_dataCountPerPlot );
    MapDataPoints();
    Invalidate ( FALSE );
}

void SimpleGraph::GetXCordRangeLabels ( CString * rangeStrs, DOUBLE rangeMax, DOUBLE rangeMin, TimeUnit unit )
{
    TCHAR const * spted_unit[] = {_T ( "us" ), _T ( "ms" ), _T ( "Sec" ) };
    int tm_unit_idx;
    for ( tm_unit_idx = unit; tm_unit_idx < Second && ! ( rangeMax < 1000 ); ++tm_unit_idx )
    {
        rangeMax /= 1000;
        rangeMin /= 1000;
    }

    rangeStrs[0].Format ( _T ( "%.2f%s" ), rangeMax, spted_unit[tm_unit_idx] );
    rangeStrs[1].Format ( _T ( "%.2f%s" ), rangeMin, spted_unit[tm_unit_idx] );
}

void SimpleGraph::GetYCordRangeLabels ( CString * rangeStrs, DOUBLE rangeMax, DOUBLE rangeMin, ValueUnit unit )
{
    TCHAR const * spted_unit [] = {_T ( "kV" ), _T ( "V" ), _T ( "mV" ), _T ( "uV" ), _T ( "KA" ), _T ( "A" ) , _T ( "mA" ) , _T ( "uA" ) , _T ( "C" ) , _T ( "" ) };
    int index = ( int ) unit;
    if ( -1 == index )
    {
        index = 8;
    }
    rangeStrs[0].Format ( _T ( "%.3f%s" ), rangeMax, spted_unit[index] );
    rangeStrs[1].Format ( _T ( "%.3f%s" ), rangeMin, spted_unit[index] );
    rangeStrs[2] = ( rangeMax == -rangeMin ) ? "0 " : "";
}
void SimpleGraph::GetYCordRangeLabels ( CString * rangeStrs, DOUBLE rangeMax, DOUBLE rangeMin, FrequencyUnit unit )
{
    TCHAR const * spted_unit [] = { _T ( "Hz" ), _T ( "k" ), _T ( "M" ), _T ( "" ) };
    int index = ( int ) unit;
    if ( -1 == index )
    {
        index = 3;
    }
    rangeStrs[0].Format ( _T ( "%.3f%s" ), rangeMax, spted_unit[index] );
    rangeStrs[1].Format ( _T ( "%.3f%s" ), rangeMin, spted_unit[index] );
    rangeStrs[2] = ( rangeMax == -rangeMin ) ? "0 " : "";
}

