#pragma warning(disable : 4819)
#include <graphics.h>
#include <vector>
#include <algorithm>
#include <tchar.h>
#include <random>
#include <chrono>

using namespace std;

int single_width = 240;
ExMessage msg;
bool isRunning = true;
DWORD start_time, delta_time;

// Game mode enum
enum GameMode
{
    PLAYER_VS_PLAYER = 1,
    PLAYER_VS_AI = 2
};

GameMode currentGameMode = PLAYER_VS_PLAYER;
bool showModeSelection = true;

vector<vector<char>> board = {
    {'-', '-', '-'},
    {'-', '-', '-'},
    {'-', '-', '-'}};

class Player
{
public:
    char symbol;
    bool isAI;
    Player(char s, bool ai = false) : symbol(s), isAI(ai) {}
};

// AI class for computer opponent
class AI
{
private:
    char aiSymbol;
    char playerSymbol;

public:
    AI(char ai, char player) : aiSymbol(ai), playerSymbol(player) {}

    // Simple AI: find best move using minimax algorithm
    pair<int, int> getBestMove(vector<vector<char>> &currentBoard)
    {
        int bestScore = -1000;
        pair<int, int> bestMove = {-1, -1};

        for (int i = 0; i < 3; i++)
        {
            for (int j = 0; j < 3; j++)
            {
                if (currentBoard[i][j] == '-')
                {
                    currentBoard[i][j] = aiSymbol;
                    int score = minimax(currentBoard, 0, false);
                    currentBoard[i][j] = '-';

                    if (score > bestScore)
                    {
                        bestScore = score;
                        bestMove = {i, j};
                    }
                }
            }
        }

        return bestMove;
    }

private:
    int minimax(vector<vector<char>> &currentBoard, int depth, bool isMaximizing)
    {
        // Check if AI wins
        if (checkWinForSymbol(currentBoard, aiSymbol))
        {
            return 10 - depth;
        }
        // Check if player wins
        if (checkWinForSymbol(currentBoard, playerSymbol))
        {
            return depth - 10;
        }
        // Check if it's a draw
        if (isBoardFull(currentBoard))
        {
            return 0;
        }

        if (isMaximizing)
        {
            int bestScore = -1000;
            for (int i = 0; i < 3; i++)
            {
                for (int j = 0; j < 3; j++)
                {
                    if (currentBoard[i][j] == '-')
                    {
                        currentBoard[i][j] = aiSymbol;
                        int score = minimax(currentBoard, depth + 1, false);
                        currentBoard[i][j] = '-';
                        bestScore = max(bestScore, score);
                    }
                }
            }
            return bestScore;
        }
        else
        {
            int bestScore = 1000;
            for (int i = 0; i < 3; i++)
            {
                for (int j = 0; j < 3; j++)
                {
                    if (currentBoard[i][j] == '-')
                    {
                        currentBoard[i][j] = playerSymbol;
                        int score = minimax(currentBoard, depth + 1, true);
                        currentBoard[i][j] = '-';
                        bestScore = min(bestScore, score);
                    }
                }
            }
            return bestScore;
        }
    }

    bool checkWinForSymbol(const vector<vector<char>> &currentBoard, char symbol)
    {
        // Check rows
        for (int i = 0; i < 3; i++)
        {
            if (currentBoard[i][0] == symbol && currentBoard[i][1] == symbol && currentBoard[i][2] == symbol)
            {
                return true;
            }
        }
        // Check columns
        for (int i = 0; i < 3; i++)
        {
            if (currentBoard[0][i] == symbol && currentBoard[1][i] == symbol && currentBoard[2][i] == symbol)
            {
                return true;
            }
        }
        // Check diagonals
        if (currentBoard[0][0] == symbol && currentBoard[1][1] == symbol && currentBoard[2][2] == symbol)
        {
            return true;
        }
        if (currentBoard[0][2] == symbol && currentBoard[1][1] == symbol && currentBoard[2][0] == symbol)
        {
            return true;
        }
        return false;
    }

    bool isBoardFull(const vector<vector<char>> &currentBoard)
    {
        for (const auto &row : currentBoard)
        {
            if (find(row.begin(), row.end(), '-') != row.end())
            {
                return false;
            }
        }
        return true;
    }
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
    static TCHAR str[128];
    if (currentPlayer.isAI)
    {
        _stprintf_s(str, 128, _T("AI's turn (thinking...)"));
    }
    else
    {
        _stprintf_s(str, 128, _T("Player %c's turn"), currentPlayer.symbol);
    }
    settextcolor(RGB(225, 175, 45));
    outtextxy(0, 0, str);
}

void drawModeSelection()
{
    static TCHAR str[256];
    _stprintf_s(str, 256, _T("Select Game Mode:\n\n1. Player vs Player\n2. Player vs AI\n\nPress 1 or 2 to select"));
    settextcolor(RGB(255, 255, 255));
    settextstyle(20, 0, _T("Arial"));
    outtextxy(50, 200, str);
}

void drawGameModeInfo()
{
    static TCHAR str[128];
    if (currentGameMode == PLAYER_VS_PLAYER)
    {
        _stprintf_s(str, 128, _T("Mode: Player vs Player"));
    }
    else
    {
        _stprintf_s(str, 128, _T("Mode: Player vs AI"));
    }
    settextcolor(RGB(150, 150, 150));
    settextstyle(16, 0, _T("Arial"));
    outtextxy(0, 30, str);
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

void resetGame()
{
    // Reset board
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            board[i][j] = '-';
        }
    }
}

int main()
{
    Player player1('O', false);
    Player player2('X', false);
    Player currentPlayer = player1;
    Player lastPlayer = player1;
    AI ai('X', 'O'); // AI uses X, player uses O

    // 初始化窗口
    initgraph(single_width * 3, single_width * 3 + 50); // Add extra height for mode info

    BeginBatchDraw();

    while (isRunning)
    {
        start_time = GetTickCount();

        if (showModeSelection)
        {
            // Mode selection screen
            while (peekmessage(&msg))
            {
                if (msg.message == WM_KEYDOWN)
                {
                    if (msg.vkcode == '1')
                    {
                        currentGameMode = PLAYER_VS_PLAYER;
                        player2.isAI = false;
                        showModeSelection = false;
                        resetGame();
                    }
                    else if (msg.vkcode == '2')
                    {
                        currentGameMode = PLAYER_VS_AI;
                        player2.isAI = true;
                        showModeSelection = false;
                        resetGame();
                    }
                    else if (msg.vkcode == VK_ESCAPE)
                    {
                        isRunning = false;
                    }
                }
            }

            cleardevice();
            drawModeSelection();
            FlushBatchDraw();
        }
        else
        {
            // Game screen
            while (peekmessage(&msg))
            {
                // 检查鼠标左键按下消息
                if (msg.message == WM_LBUTTONDOWN && !currentPlayer.isAI)
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
                    else if (msg.vkcode == 'R' || msg.vkcode == 'r')
                    {
                        // Reset game
                        resetGame();
                        currentPlayer = player1;
                        lastPlayer = player1;
                    }
                    else if (msg.vkcode == 'M' || msg.vkcode == 'm')
                    {
                        // Return to mode selection
                        showModeSelection = true;
                    }
                }
            }

            // AI move
            if (currentPlayer.isAI && !showModeSelection)
            {
                Sleep(500); // Add delay to make AI move visible
                auto aiMove = ai.getBestMove(board);
                if (aiMove.first != -1 && aiMove.second != -1)
                {
                    board[aiMove.first][aiMove.second] = currentPlayer.symbol;
                    lastPlayer = currentPlayer;
                    currentPlayer = (currentPlayer.symbol == player1.symbol) ? player2 : player1;
                }
            }

            cleardevice();

            drawBoard();
            drawPiece();
            drawTipText(currentPlayer);
            drawGameModeInfo();

            // Draw controls info
            settextcolor(RGB(100, 100, 100));
            settextstyle(12, 0, _T("Arial"));
            outtextxy(0, single_width * 3 + 10, _T("ESC: Exit | R: Reset | M: Mode Selection"));

            FlushBatchDraw();

            // 检查上一个落子的玩家是否获胜
            if (checkWin(lastPlayer))
            {
                TCHAR str[128];
                if (lastPlayer.isAI)
                {
                    _stprintf_s(str, 128, _T("AI wins!"));
                }
                else
                {
                    _stprintf_s(str, 128, _T("Player %c wins!"), lastPlayer.symbol);
                }
                MessageBox(GetHWnd(), str, _T("Game Over"), MB_OK);
                resetGame();
                currentPlayer = player1;
                lastPlayer = player1;
            }

            // 检查是否平局
            if (checkDraw())
            {
                MessageBox(GetHWnd(), _T("It's a draw!"), _T("Game Over"), MB_OK);
                resetGame();
                currentPlayer = player1;
                lastPlayer = player1;
            }
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