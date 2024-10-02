//
//
// Created by Julian  Gomez on 6/5/24.
//
// game.cpp
//
//

#include <iostream>
#include <string>
#include <random>
#include <utility>
#include <cstdlib>
using namespace std;

///////////////////////////////////////////////////////////////////////////
// Manifest constants
///////////////////////////////////////////////////////////////////////////

const int MAXROWS  = 20;   // max number of rows in a valley
const int MAXCOLS  = 40;   // max number of columns in a valley
const int MAXBIRDS = 160;  // max number of birds allowed

const int UP      = 0;
const int DOWN    = 1;
const int LEFT    = 2;
const int RIGHT   = 3;
const int NUMDIRS = 4;

///////////////////////////////////////////////////////////////////////////
// Type definitions
///////////////////////////////////////////////////////////////////////////

class Valley;  // This is needed to let the compiler know that Valley is a
               // type name, since it's mentioned in the Bird declaration.

class Bird
{
  public:
        // Constructor
    Bird(Valley* vp, int r, int c);

        // Accessors
    int  row() const;
    int  col() const;

        // Mutators
    void move();

  private:
    Valley* m_valley;
    int     m_row;
    int     m_col;
};

class Player
{
  public:
        // Constructor
    Player(Valley* vp, int r, int c);

        // Accessors
    int  row() const;
    int  col() const;
    int  age() const;
    bool isDead() const;

        // Mutators
    void stand();
    void move(int dir);
    bool flingRock(int dir);
    void setDead();

  private:
    Valley* m_valley;
    int     m_row;
    int     m_col;
    int     m_age;
    bool    m_dead;
};

class Valley
{
  public:
        // Constructor/destructor
    Valley(int nRows, int nCols);
    ~Valley();

        // Accessors
    int     rows() const;
    int     cols() const;
    Player* player() const;
    int     birdCount() const;
    int     numBirdsAt(int r, int c) const;
    void    display(string msg) const;

        // Mutators
    bool    addBird(int r, int c);
    bool    addPlayer(int r, int c);
    bool    knockOutBird(int r, int c);
    bool    moveBirds();

  private:
    int     m_rows;
    int     m_cols;
    Player* m_player;
    Bird*   m_birds[MAXBIRDS];
    int     m_nBirds;
};

class Game
{
  public:
        // Constructor/destructor
    Game(int rows, int cols, int nBirds);
    ~Game();

        // Mutators
    void play();

  private:
    Valley* m_valley;
};

///////////////////////////////////////////////////////////////////////////
//  Auxiliary function declarations
///////////////////////////////////////////////////////////////////////////

int decodeDirection(char dir);
int randInt(int min, int max);
void clearScreen();

///////////////////////////////////////////////////////////////////////////
//  Bird implementation
///////////////////////////////////////////////////////////////////////////

Bird::Bird(Valley* vp, int r, int c)
{
    if (vp == nullptr)
    {
        cout << "***** A bird must be created in some Valley!" << endl;
        exit(1);
    }
    if (r < 1  ||  r > vp->rows()  ||  c < 1  ||  c > vp->cols())
    {
        cout << "***** Bird created with invalid coordinates (" << r << ","
             << c << ")!" << endl;
        exit(1);
    }
    m_valley = vp;
    m_row = r;
    m_col = c;
}

int Bird::row() const
{
    return m_row;
}

int Bird::col() const
{
    
    return m_col;
}

void Bird::move()
{
    // Attempt to move in a random direction; if can't move, don't move.
    int dir = randInt(0, NUMDIRS-1);
    switch (dir) {
        case UP:
            if (m_row > 1) m_row--;
            break;
        case DOWN:
            if (m_row < m_valley->rows()) m_row++;
            break;
        case LEFT:
            if (m_col > 1) m_col--;
            break;
        case RIGHT:
            if (m_col < m_valley->cols()) m_col++;
            break;
    }
}

///////////////////////////////////////////////////////////////////////////
//  Player implementations
///////////////////////////////////////////////////////////////////////////

Player::Player(Valley* vp, int r, int c)
{
    if (vp == nullptr)
    {
        cout << "***** The player must be created in some Valley!" << endl;
        exit(1);
    }
    if (r < 1  ||  r > vp->rows()  ||  c < 1  ||  c > vp->cols())
    {
        cout << "**** Player created with invalid coordinates (" << r
             << "," << c << ")!" << endl;
        exit(1);
    }
    m_valley = vp;
    m_row = r;
    m_col = c;
    m_age = 0;
    m_dead = false;
}

int Player::row() const
{
    return m_row;
}

int Player::col() const
{
    return m_col;
}

int Player::age() const
{
    return m_age;
}

void Player::stand()
{
    m_age++;
}

void Player::move(int dir)
{
    m_age++;
    // Attempt to move in the indicated direction; if we can't move, don't move
    switch (dir) {
        case UP:
            if (m_row > 1) m_row--;
            break;
        case DOWN:
            if (m_row < m_valley->rows()) m_row++;
            break;
        case LEFT:
            if (m_col > 1) m_col--;
            break;
        case RIGHT:
            if (m_col < m_valley->cols()) m_col++;
            break;
    }
}


bool Player::flingRock(int dir)
{
    m_age++;
    if (randInt(1, 3) > 1) // miss with 2/3 probability
        return false;
    int r = m_row, c = m_col;
    switch (dir) {
        case UP:
            for (r--; r >= 1; r--)
                if (m_valley->numBirdsAt(r, c) > 0)
                    return m_valley->knockOutBird(r, c);
            break;
        case DOWN:
            for (r++; r <= m_valley->rows(); r++)
                if (m_valley->numBirdsAt(r, c) > 0)
                    return m_valley->knockOutBird(r, c);
            break;
        case LEFT:
            for (c--; c >= 1; c--)
                if (m_valley->numBirdsAt(m_row, c) > 0)
                    return m_valley->knockOutBird(m_row, c);
            break;
        case RIGHT:
            for (c++; c <= m_valley->cols(); c++)
                if (m_valley->numBirdsAt(m_row, c) > 0)
                    return m_valley->knockOutBird(m_row, c);
            break;
    }
    return false;
}


bool Player::isDead() const
{
    return m_dead;
}

void Player::setDead()
{
    m_dead = true;
}

///////////////////////////////////////////////////////////////////////////
//  Valley implementations
///////////////////////////////////////////////////////////////////////////

Valley::Valley(int nRows, int nCols)
{
    if (nRows <= 0  ||  nCols <= 0  ||  nRows > MAXROWS  ||  nCols > MAXCOLS)
    {
        cout << "***** Valley created with invalid size " << nRows << " by "
             << nCols << "!" << endl;
        exit(1);
    }
    m_rows = nRows;
    m_cols = nCols;
    m_player = nullptr;
    m_nBirds = 0;
}

Valley::~Valley()
{
    delete m_player;
    for (int i = 0; i < m_nBirds; i++) {
        delete m_birds[i];
    }
}

int Valley::rows() const
{
    return m_rows;
}

int Valley::cols() const
{
    return m_cols;
}

Player* Valley::player() const
{
    return m_player;
}

int Valley::birdCount() const
{
    return m_nBirds;
}

int Valley::numBirdsAt(int r, int c) const
{
    int count = 0;
    for (int i = 0; i < m_nBirds; i++) {
        if (m_birds[i]->row() == r && m_birds[i]->col() == c)
            count++;
    }
    return count;
}

void Valley::display(string msg) const
{
      // Position (row,col) in the valley coordinate system is represented in
      // the array element grid[row-1][col-1]
    char grid[MAXROWS][MAXCOLS];
    int r, c;
    
        // Fill the grid with dots
    for (r = 0; r < rows(); r++)
        for (c = 0; c < cols(); c++)
            grid[r][c] = '.';

        // Indicate each bird's position
    //        If it's 2 through 8, set it to '2' through '8'.
    //        For 9 or more, set it to '9'.
    for (int i = 0; i < m_nBirds; i++) {
            int br = m_birds[i]->row();
            int bc = m_birds[i]->col();
            if (grid[br-1][bc-1] == '.')
                grid[br-1][bc-1] = 'B';
            else if (grid[br-1][bc-1] >= '2' && grid[br-1][bc-1] < '9')
                grid[br-1][bc-1]++;
            else if (grid[br-1][bc-1] == 'B')
                grid[br-1][bc-1] = '2';
            else
                grid[br-1][bc-1] = '9';
        }

        // Indicate player's position
    if (m_player != nullptr)
    {
          // Set the char to '@', unless there's also a bird there,
          // in which case set it to '*'.
        char& gridChar = grid[m_player->row()-1][m_player->col()-1];
        if (gridChar == '.')
            gridChar = '@';
        else
            gridChar = '*';
    }

        // Draw the grid
    clearScreen();
    for (r = 0; r < rows(); r++)
    {
        for (c = 0; c < cols(); c++)
            cout << grid[r][c];
        cout << endl;
    }
    cout << endl;

        // Write message, bird, and player info
    cout << endl;
    if (msg != "")
        cout << msg << endl;
    cout << "There are " << birdCount() << " birds remaining." << endl;
    if (m_player == nullptr)
        cout << "There is no player." << endl;
    else
    {
        if (m_player->age() > 0)
            cout << "The player has lasted " << m_player->age() << " steps." << endl;
        if (m_player->isDead())
            cout << "The player is dead." << endl;
    }
}

bool Valley::addBird(int r, int c)
{
      // If MAXBIRDS have already been added, return false.  Otherwise,
      // dynamically allocate a new bird at coordinates (r,c).  Save the
      // pointer to the newly allocated bird and return true.

    if (m_nBirds >= MAXBIRDS)
        return false;
    m_birds[m_nBirds++] = new Bird(this, r, c);
        return true;
}

bool Valley::addPlayer(int r, int c)
{
      // Don't add a player if one already exists
    if (m_player != nullptr)
        return false;

      // Dynamically allocate a new Player and add it to the valley
    m_player = new Player(this, r, c);
    return true;
}

bool Valley::knockOutBird(int r, int c)
{
    //        Return false if there is no bird there to destroy.
    for (int i = 0; i < m_nBirds; i++) {
        if (m_birds[i]->row() == r && m_birds[i]->col() == c) {
            delete m_birds[i];
            m_birds[i] = m_birds[--m_nBirds];
            return true;
        }
    }
    return false;
}


bool Valley::moveBirds()
{
    for (int k = 0; k < m_nBirds; k++)
    {
        //        move results in the bird being at the same  position
        //        as the player, the player dies.
        m_birds[k]->move();
        if (m_birds[k]->row() == m_player->row() && m_birds[k]->col() == m_player->col())
            m_player->setDead();
    }
    // return true if the player is still alive, false otherwise
    return !m_player->isDead();
}

///////////////////////////////////////////////////////////////////////////
//  Game implementations
///////////////////////////////////////////////////////////////////////////

Game::Game(int rows, int cols, int nBirds)
{
    if (nBirds < 0)
    {
        cout << "***** Cannot create Game with negative number of birds!" << endl;
        exit(1);
    }
    if (nBirds > MAXBIRDS)
    {
        cout << "***** Trying to create Game with " << nBirds
             << " birds; only " << MAXBIRDS << " are allowed!" << endl;
        exit(1);
    }
    if (rows == 1  &&  cols == 1  &&  nBirds > 0)
    {
        cout << "***** Cannot create Game with nowhere to place the birds!" << endl;
        exit(1);
    }

        // Create valley
    m_valley = new Valley(rows, cols);

        // Add player
    int rPlayer = randInt(1, rows);
    int cPlayer = randInt(1, cols);
    m_valley->addPlayer(rPlayer, cPlayer);

      // Populate with birds
    while (nBirds > 0)
    {
        int r = randInt(1, rows);
        int c = randInt(1, cols);
          // Don't put a bird where the player is
        if (r == rPlayer  &&  c == cPlayer)
            continue;
        m_valley->addBird(r, c);
        nBirds--;
    }
}

Game::~Game()
{
    delete m_valley;
}

void Game::play()
{
    string msg = "";
    m_valley->display(msg);
    Player* player = m_valley->player();
    if (player == nullptr)
        return;

    while ( ! player->isDead()  &&  m_valley->birdCount() > 0)
    {
        cout << "Move (u/d/l/r/fu/fd/fl/fr/q or nothing): ";
        string action;
        getline(cin,action);
        if (action.size() == 0)
            player->stand();
        else
        {
            switch (action[0])
            {
              default:   // if bad move, nobody moves
                cout << '\a' << endl;  // beep
                continue;
              case 'q':
                return;
              case 'u':
              case 'd':
              case 'l':
              case 'r':
                player->move(decodeDirection(action[0]));
                break;
              case 'f':
                if (action.size() < 2)  // if no direction, nobody moves
                {
                    cout << '\a' << endl;  // beep
                    continue;
                }
                switch (action[1])
                {
                  default:   // if bad direction, nobody moves
                    cout << '\a' << endl;  // beep
                    continue;
                  case 'u':
                  case 'd':
                  case 'l':
                  case 'r':
                    if (player->flingRock(decodeDirection(action[1])))
                        msg = "Hit!";
                    else
                        msg = "Missed!";
                    break;
                }
                break;
            }
        }
        if ( ! player->isDead())
            m_valley->moveBirds();
        m_valley->display(msg);
        msg = "";
    }
    if (player->isDead())
        cout << "You lose." << endl;
    else
        cout << "You win." << endl;
}

///////////////////////////////////////////////////////////////////////////
//  Auxiliary function implementations
///////////////////////////////////////////////////////////////////////////

int decodeDirection(char dir)
{
    switch (dir)
    {
      case 'u':  return UP;
      case 'd':  return DOWN;
      case 'l':  return LEFT;
      case 'r':  return RIGHT;
    }
    return -1;  // bad argument passed in!
}

  // Return a uniformly distributed random int from min to max, inclusive
int randInt(int min, int max)
{
    if (max < min)
        swap(max, min);
    static random_device rdev;
    static default_random_engine generator(rdev());
    uniform_int_distribution<> distro(min, max);
    return distro(generator);
}

///////////////////////////////////////////////////////////////////////////
//  main()
///////////////////////////////////////////////////////////////////////////


int main()
{
 
      // Create a game
      // Use this instead to create a mini-game:   Game g(3, 3, 2);
    Game g(15, 18, 100);
    // Game g(3,3,2);
      // Play the game
    g.play();
}


///////////////////////////////////////////////////////////////////////////
//  clearScreen implementation
///////////////////////////////////////////////////////////////////////////
///
#ifdef _WIN32

#pragma warning(disable : 4005)
#include <windows.h>

void clearScreen()
{
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(hConsole, &csbi);
    DWORD dwConSize = csbi.dwSize.X * csbi.dwSize.Y;
    COORD upperLeft = { 0, 0 };
    DWORD dwCharsWritten;
    FillConsoleOutputCharacter(hConsole, TCHAR(' '), dwConSize, upperLeft,
                                                        &dwCharsWritten);
    SetConsoleCursorPosition(hConsole, upperLeft);
}

#else  // not _WIN32

#include <iostream>
#include <cstring>
#include <cstdlib>

void clearScreen()  // will just write a newline in an Xcode output window
{
    static const char* term = getenv("TERM");
    if (term == nullptr  ||  strcmp(term, "dumb") == 0)
        cout << endl;
    else
    {
        static const char* ESC_SEQ = "\x1B[";  // ANSI Terminal esc seq:  ESC [
        cout << ESC_SEQ << "2J" << ESC_SEQ << "H" << flush;
    }
}

#endif
