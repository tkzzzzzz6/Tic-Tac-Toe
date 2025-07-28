/*
    EasyX Tic-Tac-Toe Demo (demo2.cpp)
    -----------------------------------
    This program implements a graphical Tic-Tac-Toe (三子棋) game using the EasyX graphics library.
    Features:
      - Two game modes: Player vs Player and Player vs AI (with unbeatable minimax AI).
      - Mouse-based input for placing pieces.
      - Mode selection, reset, and exit controls.
      - Visual feedback for turns, game mode, and game result.
      - Clean and modular code structure for easy extension.

    Controls:
      - Mouse Left Click: Place your piece (when it's your turn).
      - 1 / 2: Select game mode at the start.
      - R: Reset the current game.
      - M: Return to mode selection.
      - ESC: Exit the game.

    Requirements:
      - EasyX graphics library (https://easyx.cn/)
      - Windows platform

    Author: (Your Name)
    Date: (2024)
*/

#pragma warning(disable : 4819)
#include <graphics.h>
#include <vector>
#include <algorithm>
#include <tchar.h>
#include <random>
#include <chrono>

using namespace std;

// The width of a single cell in the tic-tac-toe board
int single_width = 240;

// Message structure for EasyX event handling
ExMessage msg;

// Main loop control flag
bool isRunning = true;

// Timing variables for frame rate control
DWORD start_time, delta_time;

// Enumeration for game modes
enum GameMode
{
    PLAYER_VS_PLAYER = 1, // Player vs Player mode
    PLAYER_VS_AI = 2      // Player vs AI mode
};

// Current game mode
GameMode currentGameMode = PLAYER_VS_PLAYER;

// Whether to show the mode selection screen
bool showModeSelection = true;

// The tic-tac-toe board, 3x3 grid initialized to empty ('-')
vector<vector<char>> board = {
    {'-', '-', '-'},
    {'-', '-', '-'},
    {'-', '-', '-'}};

// Player class representing a human or AI player
class Player
{
public:
    char symbol;   // 'O' or 'X'
    bool isAI;     // true if this player is an AI
    Player(char s, bool ai = false) : symbol(s), isAI(ai) {}
};

// AI class for computer opponent using minimax algorithm
class AI
{
private:
    char aiSymbol;      // Symbol used by AI ('X')
    char playerSymbol;  // Symbol used by human player ('O')

public:
    AI(char ai, char player) : aiSymbol(ai), playerSymbol(player) {}

    // Find the best move for the AI using minimax
    pair<int, int> getBestMove(vector<vector<char>> &currentBoard)
    {
        int bestScore = -1000;
        pair<int, int> bestMove = {-1, -1};

        // Try every possible move
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
    // Minimax algorithm to evaluate board positions
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

    // Check if a given symbol has won on the board
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

    // Check if the board is full (no empty cells)
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

// Draw the tic-tac-toe board grid
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

// Draw the pieces ('O' and 'X') on the board
void drawPiece()
{
    for (int i = 0; i < 3; ++i)
    {
        for (int j = 0; j < 3; ++j)
        {
            switch (board[i][j])
            {
            case 'O':
                // Draw circle for 'O'
                circle(j * single_width + single_width / 2, i * single_width + single_width / 2, single_width / 3);
                break;
            case 'X':
                // Draw 'X' as two diagonal lines
                line(j * single_width + 10, i * single_width + 10, (j + 1) * single_width - 10, (i + 1) * single_width - 10);
                line((j + 1) * single_width - 10, i * single_width + 10, j * single_width + 10, (i + 1) * single_width - 10);
                break;
            default:
                break;
            }
        }
    }
}

// Draw the tip text showing whose turn it is
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

// Draw the mode selection screen
void drawModeSelection()
{
    static TCHAR str[256];
    _stprintf_s(str, 256, _T("Select Game Mode:\n\n1. Player vs Player\n2. Player vs AI\n\nPress 1 or 2 to select"));
    settextcolor(RGB(255, 255, 255));
    settextstyle(20, 0, _T("Arial"));
    outtextxy(50, 200, str);
}

// Draw the current game mode info on the screen
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

// Check if the given player has won the game
bool checkWin(Player p)
{
    // Check rows
    for (int i = 0; i < 3; i++)
    {
        if (board[i][0] == p.symbol && board[i][1] == p.symbol && board[i][2] == p.symbol)
        {
            return true;
        }
    }
    // Check columns
    for (int i = 0; i < 3; i++)
    {
        if (board[0][i] == p.symbol && board[1][i] == p.symbol && board[2][i] == p.symbol)
        {
            return true;
        }
    }
    // Check main diagonal
    if (board[0][0] == p.symbol && board[1][1] == p.symbol && board[2][2] == p.symbol)
        return true;
    // Check anti-diagonal
    if (board[0][2] == p.symbol && board[1][1] == p.symbol && board[2][0] == p.symbol)
        return true;
    return false;
}

// Check if the game is a draw (no empty cells and no winner)
bool checkDraw()
{
    for (const auto &row : board)
    {
        if (find(row.begin(), row.end(), '-') != row.end())
            return false;
    }
    return true;
}

// Reset the game board to the initial state
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

// Main function: program entry point
int main()
{
    Player player1('O', false); // Player 1 uses 'O'
    Player player2('X', false); // Player 2 or AI uses 'X'
    Player currentPlayer = player1; // Track whose turn it is
    Player lastPlayer = player1;    // Track who made the last move
    AI ai('X', 'O'); // AI uses X, player uses O

    // Initialize the EasyX window (add extra height for info)
    initgraph(single_width * 3, single_width * 3 + 50);

    BeginBatchDraw(); // Enable batch drawing for smoother graphics

    while (isRunning)
    {
        start_time = GetTickCount();

        if (showModeSelection)
        {
            // Mode selection screen: handle key input for mode selection
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
            // Game screen: handle mouse and keyboard input
            while (peekmessage(&msg))
            {
                // Handle mouse left button down for placing a piece
                if (msg.message == WM_LBUTTONDOWN && !currentPlayer.isAI)
                {
                    // Calculate board position from mouse coordinates
                    int x = msg.x;
                    int y = msg.y;
                    int row = y / single_width;
                    int col = x / single_width;

                    // Check if click is within board bounds
                    if (row >= 0 && row < 3 && col >= 0 && col < 3)
                    {
                        // Place piece if cell is empty
                        if (board[row][col] == '-')
                        {
                            board[row][col] = currentPlayer.symbol;
                            lastPlayer = currentPlayer; // Record who made the move

                            // Switch to the other player
                            currentPlayer = (currentPlayer.symbol == player1.symbol) ? player2 : player1;
                        }
                    }
                }
                // Handle keyboard input for reset, mode selection, and exit
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

            // If it's the AI's turn, let the AI make a move
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

            // Draw the board, pieces, tip text, and game mode info
            drawBoard();
            drawPiece();
            drawTipText(currentPlayer);
            drawGameModeInfo();

            // Draw controls info at the bottom
            settextcolor(RGB(100, 100, 100));
            settextstyle(12, 0, _T("Arial"));
            outtextxy(0, single_width * 3 + 10, _T("ESC: Exit | R: Reset | M: Mode Selection"));

            FlushBatchDraw();

            // Check if the last player has won
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

            // Check if the game is a draw
            if (checkDraw())
            {
                MessageBox(GetHWnd(), _T("It's a draw!"), _T("Game Over"), MB_OK);
                resetGame();
                currentPlayer = player1;
                lastPlayer = player1;
            }
        }

        // Frame rate control: sleep to maintain ~60 FPS
        delta_time = GetTickCount() - start_time;
        if (delta_time < 1000 / 60)
        {
            Sleep(1000 / 60 - delta_time);
        }
    }
    EndBatchDraw();
    return 0;
}