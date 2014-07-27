# define _CRT_SECURE_NO_WARNINGS
# define IDS_INFO 1000

# include <Windows.h>
# include <iostream>

///////////////////////////////////////////////////////////////////////////////////
//全局变量
HINSTANCE m_hInst;								//程序实例
HWND m_hWnd;									//主窗口句柄
LPCSTR m_lpWindowName = "MainWindow";			//窗口类

HWND m_hInfo;									//提示信息

HDC m_hDC;										//设备DC
HBRUSH m_hBrush;								//画刷句柄

int m_sWidth = ::GetSystemMetrics(SM_CXSCREEN);	//获取屏幕大小
int m_sHeight = ::GetSystemMetrics(SM_CYSCREEN);

int m_Width = 500;								//用户区大小
int m_Height = 550;

int m_ChessPos[25][25] = { 0 };					//棋子坐标
POINT m_pt;										//最新一步骑的数组下标

int m_player = 1;								//当前玩家

////////////////////////////////////////////////////////////////////////////////////
//函数声明
void SetClientSize();															//纠正用户区大小为500*500
LRESULT CALLBACK WinProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);	//消息响应
HWND InitApplication(HINSTANCE hInstance);										//初始化程序
void DrawChessboard(void);														//绘制棋盘
void DrawChess(int player, int x, int y);										//绘制棋子
void DrawAllChess(void);														//绘制所有棋子
void PlayChess(int player = 2);													//下棋
void WhetherVictory(void);														//当前一步棋是否胜利
void ChessRule(void);															//下棋规则

///////////////////////////////////////////////////////////////////////////////////
int WINAPI WinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int ShowCmd)
{
	////棋子占满棋盘测试
	//for (int i = 0; i < 25; i++)
	//{
	//	for (int j = 0; j < 25; j++)
	//	{
	//		m_ChessPos[i][j] = 2;
	//	}
	//}


	m_hInst = hInstance;//保存程序实例

	if (NULL == InitApplication(hInstance))
	{
		return ERROR;
	}

	m_hDC = GetDC(m_hWnd);//保存设备DC

	ShowWindow(m_hWnd, SW_SHOW);//显示窗口

	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return msg.wParam;
}

LRESULT CALLBACK WinProc(
	HWND hWnd,
	UINT uMsg,
	WPARAM wParam,
	LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_SHOWWINDOW:
		if (wParam == TRUE)
		{
			SetClientSize();
		}
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
		break;
	case WM_PAINT:
		DrawChessboard();
		DrawAllChess();
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
		break;
	case WM_LBUTTONDOWN:
		ChessRule();
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
		break;
	case WM_CLOSE:
		if (IDYES == MessageBox(m_hWnd, "是否关闭游戏？", "提示", MB_YESNO))
		{
			//释放资源
			ReleaseDC(m_hWnd, m_hDC);
			DeleteObject(m_hBrush);
			//发送消息
			DestroyWindow(m_hWnd);
		}
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
	return 0;
}

HWND InitApplication(HINSTANCE hInstance)
{
	WNDCLASS wnd;
	wnd.cbClsExtra = 0;
	wnd.cbWndExtra = 0;
	wnd.hbrBackground = (HBRUSH)GetStockObject(COLOR_WINDOW + 1);
	wnd.hCursor = LoadCursor(NULL, IDC_ARROW);
	wnd.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_APPLICATION));
	wnd.hInstance = hInstance;
	wnd.lpfnWndProc = WinProc;
	wnd.lpszClassName = m_lpWindowName;
	wnd.lpszMenuName = NULL;
	wnd.style = NULL;

	RegisterClass(&wnd);

	m_hWnd = CreateWindow(m_lpWindowName, "五子棋", WS_CAPTION | WS_SYSMENU,
		(m_sWidth - m_Width) / 2, (m_sHeight - m_Height) / 2, m_Width + 16, m_Height + 38,
		NULL, NULL, hInstance, NULL);


	m_hInfo = CreateWindow("STATIC", "\n游戏提示信息...", WS_CHILD | WS_VISIBLE | SS_CENTER,
		0, 500, 500, 50, m_hWnd, (HMENU)IDS_INFO, m_hInst, NULL);
	if (m_hInfo == NULL)
	{
		MessageBox(NULL, "void ConsoleInfo(void) is error !", "ERROR", NULL);
		return NULL;
	}

	return m_hWnd;
}

///////////////////////////////////////////////////////////////////////////////////
void DrawChessboard(void)
{
	int delta = 0;//增量

	//画线
	for (delta = 0; delta <= 480; delta += 20)
	{
		//横线
		MoveToEx(m_hDC, 10, 10 + delta, NULL);
		LineTo(m_hDC, 490, 10 + delta);

		//竖线
		MoveToEx(m_hDC, 10 + delta, 10, NULL);
		LineTo(m_hDC, 10 + delta, 490);
	}

	//画中心点
	m_hBrush = (HBRUSH)GetStockObject(BLACK_BRUSH);
	SelectObject(m_hDC, m_hBrush);
	Ellipse(m_hDC, 247, 247, 253, 253);
}

void DrawChess(int player, int x, int y)
{
	int r = 8;//圆的半径

	switch (player)
	{
	case 1:
		m_hBrush = (HBRUSH)GetStockObject(BLACK_BRUSH);//实心黑圆
		break;
	case 2:
		m_hBrush = (HBRUSH)GetStockObject(WHITE_BRUSH);//空心黑圈
		break;
	default:
		return;
	}

	SelectObject(m_hDC, m_hBrush);//选择画刷
	Ellipse(m_hDC, x - r, y - r, x + r, y + r);//画棋
}

void DrawAllChess(void)
{
	int x = 0, y = 0;
	for (x = 0; x < 25; x++)
	{
		for (y = 0; y < 25; y++)
		{
			DrawChess(m_ChessPos[x][y], x * 20 + 10, y * 20 + 10);
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
void PlayChess(int player)
{
	//取坐标
	POINT pt;
	while (1)
	{
		//获取鼠标在用户区的坐标
		if (!GetCursorPos(&pt))
		{
			MessageBox(m_hWnd, "void PlayChess(void) is error !", "ERROR", NULL);
		}
		ScreenToClient(m_hWnd, &pt);//转换为客户区坐标

		if (pt.x <= 500 && pt.y <= 500)
		{
			//获取坐标对应的数组下标
			m_pt.x = pt.x / 20;
			m_pt.y = pt.y / 20;

			if (m_ChessPos[m_pt.x][m_pt.y] == 0)//如果该位置未被下，则写入棋子，退出循环，否者重行下棋
			{

				//写入数组，并重新绘制棋盘
				m_ChessPos[m_pt.x][m_pt.y] = player;
				SendMessage(m_hWnd, WM_PAINT, 0, 0);
				break;
			}

			//EnableWindow(m_hWnd, FALSE);
			//ShowWindow(m_hWnd, SW_HIDE);//隐藏窗口

			MessageBox(NULL, "该位置已有棋子，请重新换一位置！", "ERROR", NULL);

			//EnableWindow(m_hWnd, TRUE);
			//ShowWindow(m_hWnd, SW_SHOW);
		}
	}

	////测试数据获取情况
	//char str[100];
	//std::sprintf(str, "pt.x = %d, pt.y = %d\nx = %d, y = %d", pt.x, pt.y, x, y);
	//MessageBox(m_hWnd, str, "TEXT", NULL);
}

////////////////////////////////////////////////////////////////////////////////
void ChessRule(void)
{
	//取坐标
	POINT pt;
	//获取鼠标在用户区的坐标
	if (!GetCursorPos(&pt))
	{
		MessageBox(m_hWnd, "void ChessRule(void) is error !", "ERROR", NULL);
	}
	ScreenToClient(m_hWnd, &pt);//转换为客户区坐标

	if (pt.x <= 500 && pt.y <= 500)//判断鼠标是否在棋盘范围内单击，如果不是则不执行下棋
	{
		//选择玩家下棋
		switch (m_player)
		{
		case 1:
			PlayChess(1);
			m_player++;
			break;
		case 2:
			PlayChess(2);
			m_player--;
			break;
		default:
			MessageBox(NULL, "ChessRule() is error.\nNot the player !", "ERROR", NULL);
		}
		//判断是否胜利
		WhetherVictory();

		//设置状态栏信息显示
		char info[100] = { 0 };
		sprintf(info, "\n当前玩家 ：player %d", m_player);
		SetWindowText(m_hInfo, info);
	}
}

void WhetherVictory(void)
{
	int delta = 1;					//从除这一步棋以外的棋开始计算连棋个数

	int LeftDiagonal_sum = 0;		//左斜线上棋子个数
	int RightDiagonal_sum = 0;		//右斜线上棋子个数
	int HorizontalLine_sum = 0;		//横线上棋子个数
	int VerticalLine_sum = 0;		//竖线上棋子个数

	while (delta < 5)
	{
		//------------------------------------------左斜线
		//左边
		if (m_pt.x - delta >= 0 && m_pt.y + delta <= 24 &&
			m_ChessPos[m_pt.x][m_pt.y] == m_ChessPos[m_pt.x - delta][m_pt.y + delta])
		{
			LeftDiagonal_sum++;
		}
		//右边
		if (m_pt.x + delta <= 24 && m_pt.y - delta >= 0 &&
			m_ChessPos[m_pt.x][m_pt.y] == m_ChessPos[m_pt.x + delta][m_pt.y - delta])
		{
			LeftDiagonal_sum++;
		}
		//判断是否胜利
		if (LeftDiagonal_sum >= 4)
		{
			MessageBox(NULL, "Winner !", "TEXT", NULL);
			return;
		}
		//------------------------------------------右斜线
		//左边
		if (m_pt.x - delta >= 0 && m_pt.y - delta >= 0 &&
			m_ChessPos[m_pt.x][m_pt.y] == m_ChessPos[m_pt.x - delta][m_pt.y - delta])
		{
			RightDiagonal_sum++;
		}
		//右边
		if (m_pt.x + delta <= 24 && m_pt.y + delta <= 24 &&
			m_ChessPos[m_pt.x][m_pt.y] == m_ChessPos[m_pt.x + delta][m_pt.y + delta])
		{
			RightDiagonal_sum++;
		}
		//判断是否胜利
		if (RightDiagonal_sum >= 4)
		{
			MessageBox(NULL, "Winner !", "TEXT", NULL);
			return;
		}
		//------------------------------------------横线
		//左边
		if (m_pt.x - delta >= 0 &&
			m_ChessPos[m_pt.x][m_pt.y] == m_ChessPos[m_pt.x - delta][m_pt.y])
		{
			HorizontalLine_sum++;
		}
		//右边
		if (m_pt.x + delta <= 24 &&
			m_ChessPos[m_pt.x][m_pt.y] == m_ChessPos[m_pt.x + delta][m_pt.y])
		{
			HorizontalLine_sum++;
		}
		//判断是否胜利
		if (HorizontalLine_sum >= 4)
		{
			MessageBox(NULL, "Winner !", "TEXT", NULL);
			return;
		}
		//------------------------------------------竖线
		//上边
		if (m_pt.y - delta >= 0 &&
			m_ChessPos[m_pt.x][m_pt.y] == m_ChessPos[m_pt.x][m_pt.y - delta])
		{
			VerticalLine_sum++;
		}
		//下边
		if (m_pt.y + delta <= 24 &&
			m_ChessPos[m_pt.x][m_pt.y] == m_ChessPos[m_pt.x][m_pt.y + delta])
		{
			VerticalLine_sum++;
		}
		//判断是否胜利
		if (VerticalLine_sum >= 4)
		{
			MessageBox(NULL, "Winner !", "TEXT", NULL);
			return;
		}

		delta++;
	}

	////测试数据获取情况
	//char str[200];
	//std::sprintf(str, "x = %d, y = %d\nLeftDiagonal_sum = %d, RightDiagonal_sum = %d\nHorizontalLine_sum = %d, VerticalLine_sum = %d", 
	//	m_pt.x,m_pt.y, LeftDiagonal_sum, RightDiagonal_sum, HorizontalLine_sum, VerticalLine_sum);
	//MessageBox(NULL, str, "TEXT", NULL);
}

/////////////////////////////////////////////////////////////////////////////////////////////
void SetClientSize(void)
{
	RECT  rectProgram, rectClient;

	GetClientRect(m_hWnd, &rectClient);      //获得客户区坐标
	if ((rectClient.right - rectClient.left) == 500 && (rectClient.bottom - rectClient.top) == 500)
	{
		//	MessageBox(NULL, "大小一样", NULL, NULL);
		return;
	}
	//else
	//{
	//	//测试数据获取情况
	//	char str[100];
	//	std::sprintf(str, "Client_x = %d, Client_y = %d", rectClient.right - rectClient.left,rectClient.bottom - rectClient.top);
	//	MessageBox(m_hWnd, str, "TEXT", NULL);
	//	MessageBox(NULL, "大小不一样", NULL, NULL);
	//}

	GetWindowRect(m_hWnd, &rectProgram);   //获得程序窗口位于屏幕坐标

	//非客户区宽,高
	int nWidth = rectProgram.right - rectProgram.left - (rectClient.right - rectClient.left);
	int nHeight = rectProgram.bottom - rectProgram.top - (rectClient.bottom - rectClient.top);
	nWidth += m_Width;
	nHeight += m_Height;

	rectProgram.right = nWidth;
	rectProgram.bottom = nHeight;
	int showToScreenx = (m_sWidth - nWidth) / 2;    //居中处理
	int showToScreeny = (m_sHeight - nHeight) / 2;

	MoveWindow(m_hWnd, showToScreenx, showToScreeny, rectProgram.right, rectProgram.bottom, false);
}

///////////////////////////////////////////////////////////////////////////////////////////////