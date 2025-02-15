#include <iostream>
#include <conio.h>
#include <windows.h>
#include <ctime>
#include <string>
#include <fstream>
#include <iomanip>

using namespace std;

const int width = 20;
const int height = 20;
bool gameOver;
int score;
int fruitX, fruitY;
int x, y;
int tailX[100], tailY[100];
int nTail;
enum eDirection { STOP = 0, LEFT, RIGHT, UP, DOWN };
eDirection dir;
bool obstacle[width][height];
int speed;
string playerName;
int lives;
bool savedPoints = false;

void Setup(int level) {
    gameOver = false;
    dir = STOP;
    x = width / 2;
    y = height / 2;
    fruitX = rand() % width;
    fruitY = rand() % height;
    score = 0;
    nTail = 0;
    lives = 3;
    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {
            obstacle[i][j] = false;
        }
    }
    int numObstacles = 0;
    switch (level) {
    case 1:
        speed = 150;
        numObstacles = 5;
        break;
    case 2:
        speed = 100;
        numObstacles = 10;
        break;
    case 3:
        speed = 75;
        numObstacles = 20;
        obstacle[rand() % width][rand() % height] = true;
        break;
    default:
        cout << "Invalid input. Defaulting to Medium level." << endl;
        speed = 100;
        numObstacles = 10;
    }
    for (int i = 0; i < numObstacles; i++) {
        int obsX = rand() % width;
        int obsY = rand() % height;
        if (obsX != fruitX && obsY != fruitY)
            obstacle[obsX][obsY] = true;
    }
}

void Draw() {
    system("cls");

    // Calculate horizontal margin to center the game board
    int horizontalMargin = (80 - (width + 2)) / 2;
    for (int i = 0; i < horizontalMargin; i++)
        cout << " ";
    for (int i = 0; i < width + 2; i++)
        cout << "#";
    cout << endl;

    for (int i = 0; i < height; i++) {
        for (int j = 0; j < horizontalMargin; j++)
            cout << " ";
        for (int j = 0; j < width; j++) {
            if (j == 0)
                cout << "#";
            if (i == y && j == x)
                cout << "\033[32mO";
            else if (i == fruitY && j == fruitX)
                cout << "\033[31mF";
            else if (obstacle[j][i])
                cout << "\033[33m#";
            else if (savedPoints && i == fruitY && j == fruitX)
                cout << "\033[35m@";
            else {
                bool print = false;
                for (int k = 0; k < nTail; k++) {
                    if (tailX[k] == j && tailY[k] == i) {
                        cout << "\033[32mo";
                        print = true;
                    }
                }
                if (!print)
                    cout << " ";
            }

            if (j == width - 1)
                cout << "#";
        }
        cout << endl;
    }

    for (int i = 0; i < horizontalMargin; i++)
        cout << " ";
    for (int i = 0; i < width + 2; i++)
        cout << "#";
    cout << endl;

    cout << "Score:" << score << endl;
    cout << "Lives: " << lives << endl;
}

void Input() {
    if (_kbhit()) {
        switch (_getch()) {
        case 'a':
            if (dir != RIGHT) dir = LEFT;
            break;
        case 'd':
            if (dir != LEFT) dir = RIGHT;
            break;
        case 'w':
            if (dir != DOWN) dir = UP;
            break;
        case 's':
            if (dir != UP) dir = DOWN;
            break;
        case 'x':
            gameOver = true;
            break;
        }
    }
}

void Logic() {
    int prevX = tailX[0];
    int prevY = tailY[0];
    int prev2X, prev2Y;
    tailX[0] = x;
    tailY[0] = y;
    for (int i = 1; i < nTail; i++) {
        prev2X = tailX[i];
        prev2Y = tailY[i];
        tailX[i] = prevX;
        tailY[i] = prevY;
        prevX = prev2X;
        prevY = prev2Y;
    }
    switch (dir) {
    case LEFT:
        x--;
        break;
    case RIGHT:
        x++;
        break;
    case UP:
        y--;
        break;
    case DOWN:
        y++;
        break;
    default:
        break;
    }
    if (x >= width) x = 0; else if (x < 0) x = width - 1;
    if (y >= height) y = 0; else if (y < 0) y = height - 1;

    for (int i = 0; i < nTail; i++)
        if (tailX[i] == x && tailY[i] == y)
            gameOver = true;

    if (x == fruitX && y == fruitY) {
        score += 10;
        fruitX = rand() % width;
        fruitY = rand() % height;
        nTail++;
        if (score % 50 == 0) {
            savedPoints = true;
        }
    }

    if (obstacle[x][y]) {
        lives--;
        if (lives == 0) {
            gameOver = true;
        } else {
            score = savedPoints ? score : 0;
            nTail = 0;
            x = width / 2;
            y = height / 2;
            for (int i = 0; i < width; i++) {
                for (int j = 0; j < height; j++) {
                    if (obstacle[i][j] && !(i == x && j == y)) {
                        obstacle[i][j] = false;
                        int newX, newY;
                        do {
                            newX = rand() % width;
                            newY = rand() % height;
                        } while (obstacle[newX][newY] || (newX == fruitX && newY == fruitY));
                        obstacle[newX][newY] = true;
                    }
                }
            }
        }
    }
}

void DisplayGameOver() {
    system("cls");
    cout << "######################################################################" << endl;
    cout << "#                                                                    #" << endl;
    cout << "#                        G A M E   O V E R                           #" << endl;
    cout << "#                                                                    #" << endl;
    cout << "######################################################################" << endl;
    cout << "Player Name: " << playerName << endl;
    cout << "Score: " << score << endl;
    cout << "Do you want to save your score on the leaderboard? (Y/N): ";
    char choice;
    cin >> choice;
    if (choice == 'Y' || choice == 'y') {
        ofstream leaderboardFile("leaderboard.txt", ios::app);
        if (leaderboardFile.is_open()) {
            time_t now = time(0);
            char* date = ctime(&now);
            leaderboardFile << playerName << " " << score << " " << date;
            leaderboardFile.close();
        }
    }
    cout << "C'mon " << playerName << ", don't give up! Try again? (Y/N): ";
    cin >> choice;
    if (choice == 'Y' || choice == 'y') {
        Setup(2);
        score = 0;
        lives = 3;
        x = width / 2;
        y = height / 2;
        nTail = 0;
        gameOver = false;
        savedPoints = false;
    } else {
        cout << "Thanks for playing! Goodbye!" << endl;
        exit(0);
    }
    cout << "Press any key to continue..." << endl;
    _getch();
}

void DisplayLeaderboard() {
    system("cls");
    cout << "######################################################################" << endl;
    cout << "#  No.  |   Player Name   |   Score   |   Date                      #" << endl;
    cout << "######################################################################" << endl;
    string line;
    ifstream leaderboardFile("leaderboard.txt");
    if (leaderboardFile.is_open()) {
        int rank = 1;
        while (getline(leaderboardFile, line)) {
            cout << setw(6) << rank++ << " | " << line << endl;
        }
        leaderboardFile.close();
    }
    cout << "Press any key to continue..." << endl;
    _getch();
}

int main() {
    cout << "######################################################################" << endl;
    cout << "#                                                                    #" << endl;
    cout << "#                     S N A K E   G A M E                            #" << endl;
    cout << "#                                                                    #" << endl;
    cout << "######################################################################" << endl;
    cout << "Welcome! Please enter your name: ";
    cin >> playerName;
    cout << "Hello, " << playerName << "!" << endl;

    char choice;
    ofstream leaderboardFile;
    do {
        cout << "Menu:" << endl;
        cout << "\033[32m1. Start Game\033[0m" << endl;
        cout << "\033[32m2. Display Leaderboard\033[0m" << endl;
        cout << "\033[32m3. Clear Leaderboard\033[0m" << endl;
        cout << "\033[32m4. Exit\033[0m" << endl;
        cout << "\033[32mEnter your choice: \033[0m";
        cin >> choice;

        switch (choice) {
        case '1':
            cout << "\033[32mSelect Difficulty Level: 1(Easy) 2(Medium) 3(Hard): \033[0m";
            int level;
            cin >> level;
            Setup(level);
            while (!gameOver) {
                Draw();
                Input();
                Logic();
                Sleep(speed);
            }
            DisplayGameOver();
            break;
        case '2':
            DisplayLeaderboard();
            break;
        case '3':
            leaderboardFile.open("leaderboard.txt");
            leaderboardFile.close();
            cout << "Leaderboard cleared." << endl;
            break;
        case '4':
            cout << "Exiting the game. Goodbye!" << endl;
            break;
        default:
            cout << "Invalid choice. Please enter a valid option." << endl;
        }
    } while (choice != '4');

    return 0;
}
