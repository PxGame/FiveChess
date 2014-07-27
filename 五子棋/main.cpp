# define _CRT_SECURE_NO_WARNINGS
# include <Windows.h>
# include <iostream>

///////////////////////////////////////////////////////////////////////////////////
//全局变量
HINSTANCE m_hInst;								//程序实例
HWND m_hWnd;									//主窗口句柄
LPCSTR m_lpWindowName = "MainWindow";			//窗口类

HDC m_hDC;										//设备DC
HBRUSH m_hBrush;								//画刷句柄

int m_sWidth = ::GetSystemMetrics(SM_CXSCREEN);	//获取屏幕大小
int m_sHeight = ::GetSystemMetrics(SM_CYSCREEN);

int m_Width = 500;								//窗口大小
int m_Height = 500;

int m_ChessPos[25][25] = { 0 };					//棋子坐标
POINT m_pt;										//最新一步骑的数组下标
//BOOL m_ChessIsYes = FALSE;						//棋子位置是否有效

////////////////////////////////////////////////////////////////////////////////////
//函数声明
LRESULT CALLBACK WinProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);	//消息响应
HWND InitApplication(HINSTANCE hInstance);										//初始化程序
void DrawChessboard(void);														//绘制棋盘
void DrawChess(int player, int x, int y);										//绘制棋子
void DrawAllChess(void);														//绘制所有棋子
void PlayChess(int player = 2);													//下棋
void isWinner(void);															//当前一步棋是否胜利
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
	case WM_PAINT:
		DrawChessboard();
		DrawAllChess();
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
		break;
	case WM_LBUTTONDOWN:
		ChessRule();
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
		break;
	case WM_CLOSE :
		if (IDYES == MessageBox(m_hWnd, "是否关闭游戏？", "提示", MB_YESNO))
		{
			//释放资源
			ReleaseDC(m_hWnd, m_hDC);
			DeleteObject(m_hBrush);
			//发送消息
			DestroyWindow(m_hWnd);
		}
		break;
	case WM_DESTROY :
		PostQuitMessage(0);
		break;
	default :
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
		(m_sWidth - m_Width) / 2, (m_sHeight - m_Height) / 2, m_Width + 6, m_Height + 28,
		NULL, NULL, hInstance, NULL);

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
	case 1 :
		m_hBrush = (HBRUSH)GetStockObject(BLACK_BRUSH);//实心黑圆
		break;
	case 2 :
		m_hBrush = (HBRUSH)GetStockObject(WHITE_BRUSH);//空心黑圈
		break;
	default:
		return ;
	}

	SelectObject(m_hDC, m_hBrush);//选择画刷
	Ellipse(m_hDC, x - r, y - r, x + r, y + r);//画棋
}

void DrawAllChess(void)
{
	int x = 0, y = 0;
	for (x = 0; x <25; x++)
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

		//获取坐标对应的数组下标
		m_pt.x = pt.x / 20;
		m_pt.y = pt.y / 20;

		if (m_ChessPos[m_pt.x][m_pt.y] == 0)//如果该位置未被下，则写入棋子，退出循环，否者重行下棋
		{
			//EnableWindow(m_hWnd, TRUE);

			//写入数组，并重新绘制棋盘
			m_ChessPos[m_pt.x][m_pt.y] = player;
			SendMessage(m_hWnd, WM_PAINT, 0, 0);
			break;
		}

		//EnableWindow(m_hWnd, FALSE);
		MessageBox(NULL, "该位置已有棋子，请重新换一位置！", "ERROR", NULL);
	}

	////测试数据获取情况
	//char str[100];
	//std::sprintf(str, "pt.x = %d, pt.y = %d\nx = %d, y = %d", pt.x, pt.y, x, y);
	//MessageBox(m_hWnd, str, "TEXT", NULL);
}

////////////////////////////////////////////////////////////////////////////////
void isWinner(void)
{
	int delta = 0;		//左斜线

	int LeftDiagonal_sum = 0;		//左斜线上棋子个数
	int RightDiagonal_sum = 0;		//右斜线上棋子个数
	int HorizontalLine_sum = 0;		//横线上棋子个数
	int VerticalLine_sum = 0;		//竖线上棋子个数
	
	//------------------------------------------左斜线
	//左边
	for (delta = 1; m_pt.x - delta >= 0; delta++)
	{
		if (m_ChessPos[m_pt.x][m_pt.y] == m_ChessPos[m_pt.x - delta][m_pt.y + delta])
		{
			LeftDiagonal_sum++;
		}
		else
		{
			break;
		}
	}
	//右边
	for (delta = 1; m_pt.y - delta >= 0; delta++)
	{
		if (m_ChessPos[m_pt.x][m_pt.y] == m_ChessPos[m_pt.x + delta][m_pt.y - delta])
		{
			LeftDiagonal_sum++;
		}
		else
		{
			break;
		}
	}

	//------------------------------------------右斜线
	//左边
	for (delta = 1; m_pt.y - delta >= 0; delta++)
	{
		if (m_ChessPos[m_pt.x][m_pt.y] == m_ChessPos[m_pt.x - delta][m_pt.y - delta])
		{
			RightDiagonal_sum++;
		}
		else
		{
			break;
		}
	}
	//右边
	for (delta = 1; m_pt.y - delta <= 24; delta++)
	{
		if (m_ChessPos[m_pt.x][m_pt.y] == m_ChessPos[m_pt.x + delta][m_pt.y + delta])
		{
			RightDiagonal_sum++;
		}
		else
		{
			break;
		}
	}

	//------------------------------------------横线
	//左边
	for (delta = 1; m_pt.x - delta >= 0; delta++)
	{
		if (m_ChessPos[m_pt.x][m_pt.y] == m_ChessPos[m_pt.x - delta][m_pt.y])
		{
			HorizontalLine_sum++;
		}
		else
		{
			break;
		}
	}
	//右边
	for (delta = 1; m_pt.y - delta <= 24; delta++)
	{
		if (m_ChessPos[m_pt.x][m_pt.y] == m_ChessPos[m_pt.x + delta][m_pt.y])
		{
			HorizontalLine_sum++;
		}
		else
		{
			break;
		}
	}

	//------------------------------------------竖线
	//上边
	for (delta = 1; m_pt.y - delta <= 24; delta++)
	{
		if (m_ChessPos[m_pt.x][m_pt.y] == m_ChessPos[m_pt.x][m_pt.y - delta])
		{
			VerticalLine_sum++;
		}
		else
		{
			break;
		}
	}
	//下边

	for (delta = 1; m_pt.y - delta <= 24; delta++)
	{
		if (m_ChessPos[m_pt.x][m_pt.y] == m_ChessPos[m_pt.x][m_pt.y + delta])
		{
			VerticalLine_sum++;
		}
		else
		{
			break;
		}
	}

	////测试数据获取情况
	//char str[200];
	//std::sprintf(str, "x = %d, y = %d\nLeftDiagonal_sum = %d, RightDiagonal_sum = %d\nHorizontalLine_sum = %d, VerticalLine_sum = %d", 
	//	m_pt.x,m_pt.y, LeftDiagonal_sum, RightDiagonal_sum, HorizontalLine_sum, VerticalLine_sum);
	//MessageBox(NULL, str, "TEXT", NULL);

	if (LeftDiagonal_sum >= 4 || RightDiagonal_sum >= 4 || HorizontalLine_sum >= 4 || VerticalLine_sum >= 4)
	{
		MessageBox(NULL, "Winner !", "TEXT", NULL);
	}

}

void ChessRule(void)
{
	static int player = 1;//下一个玩家

	//选择玩家下棋
	switch (player)
	{
	case 1:
		PlayChess(1);
		player++;
		break;
	case 2:
		PlayChess(2);
		player--;
		break;
	default :
		MessageBox(NULL, "ChessRule() is error.\nNot the player !", "ERROR", NULL);
	}

	//判断是否胜利
	isWinner();
}