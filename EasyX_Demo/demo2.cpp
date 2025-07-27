#pragma warning(disable : 4819)
#include <graphics.h>
#include <vector>
#include <algorithm>
#include <tchar.h>

using namespace std;

int single_width = 200;
ExMessage msg;
bool isRunning = true;
DWORD start_time, delta_time;

vector<vector<char>> board = {
    {'-', '-', '-'},
    {'-', '-', '-'},
    {'-', '-', '-'}};

class Player
{
public:
    char symbol;
    Player(char s) : symbol(s) {} // 添加构造函数
};

void drawBoard()
{
    for (int i = 0; i < 3; ++i)
    {
        for (int j = 0; j < 3; ++j)
        {
            line(j * single_width, i * single_width, (j + 1) * single_width, i * single_width);
            line(j * single_width, i * single_width, j * single_width, (i + 1) * single_width);
        }
    }
}

void drawPiece()
{
    for (int i = 0; i < 3; ++i)
    {
        for (int j = 0; j < 3; ++j)
        {
            switch (board[i][j])
            {
            case 'O':
                circle(j * single_width + single_width / 2, i * single_width + single_width / 2, single_width / 3);
                break;
            case 'X':
                // 修正X的绘制 - 需要两条对角线
                line(j * single_width + 10, i * single_width + 10, (j + 1) * single_width - 10, (i + 1) * single_width - 10);
                line((j + 1) * single_width - 10, i * single_width + 10, j * single_width + 10, (i + 1) * single_width - 10);
                break;
            default:
                break;
            }
        }
    }
}

void drawTipText(Player currentPlayer)
{
    static TCHAR str[64];
    _stprintf_s(str, 64, _T("Player %c's turn"), currentPlayer.symbol);
    settextcolor(RGB(225, 175, 45));
    outtextxy(0, 0, str);
}

// 检测指定棋子的玩家是否胜利
bool checkWin(Player p)
{
    // 检查行
    for (int i = 0; i < 3; i++)
    {
        if (board[i][0] == p.symbol && board[i][1] == p.symbol && board[i][2] == p.symbol)
        {
            return true;
        }
    }
    // 检查列
    for (int i = 0; i < 3; i++)
    {
        if (board[0][i] == p.symbol && board[1][i] == p.symbol && board[2][i] == p.symbol)
        {
            return true;
        }
    }
    // 检查主对角线
    if (board[0][0] == p.symbol && board[1][1] == p.symbol && board[2][2] == p.symbol)
        return true;
    // 检查副对角线
    if (board[0][2] == p.symbol && board[1][1] == p.symbol && board[2][0] == p.symbol)
        return true;
    return false;
}

// check draw
bool checkDraw()
{
    for (const auto &row : board)
    {
        if (find(row.begin(), row.end(), '-') != row.end())
            return false;
    }
    return true;
}

int main()
{
    Player player1('O');
    Player player2('X');
    Player currentPlayer = player1;
    Player lastPlayer = player1; // 跟踪上一个落子的玩家
    // 初始化窗口
    initgraph(single_width * 3, single_width * 3);

    BeginBatchDraw();

    while (isRunning)
    {
        start_time = GetTickCount();
        while (peekmessage(&msg))
        {
            // 检查鼠标左键按下消息
            if (msg.message == WM_LBUTTONDOWN)
            {
                // 计算点击位置
                int x = msg.x;
                int y = msg.y;
                int row = y / single_width;
                int col = x / single_width;

                // 检查边界
                if (row >= 0 && row < 3 && col >= 0 && col < 3)
                {

                    // 尝试落子
                    if (board[row][col] == '-')
                    {
                        board[row][col] = currentPlayer.symbol;
                        lastPlayer = currentPlayer; // 记录落子的玩家

                        // 切换玩家
                        currentPlayer = (currentPlayer.symbol == player1.symbol) ? player2 : player1;
                    }
                }
            }
            else if (msg.message == WM_KEYDOWN)
            {
                if (msg.vkcode == VK_ESCAPE)
                {
                    isRunning = false;
                }
            }
        }
        cleardevice();

        drawBoard();
        drawPiece();
        drawTipText(currentPlayer);

        FlushBatchDraw();

        // 检查上一个落子的玩家是否获胜
        if (checkWin(lastPlayer))
        {
            TCHAR str[64];
            _stprintf_s(str, 64, _T("Player %c wins!"), lastPlayer.symbol);
            MessageBox(GetHWnd(), str, _T("Game Over"), MB_OK);
            isRunning = false;
            break;
        }

        // 检查是否平局
        if (checkDraw())
        {
            MessageBox(GetHWnd(), _T("It's a draw!"), _T("Game Over"), MB_OK);
            isRunning = false;
            break;
        }
        delta_time = GetTickCount() - start_time;
        if (delta_time < 1000 / 60)
        {
            Sleep(1000 / 60 - delta_time);
        }
    }
    EndBatchDraw();
    return 0;
}