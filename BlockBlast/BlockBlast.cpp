// BlockBlast.cpp : Tento soubor obsahuje funkci main. Provádění programu se tam zahajuje a ukončuje.


#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include <SFML/Audio.hpp>
#include <time.h>
#include <string>

using namespace sf;

const int M = 20;
const int N = 10;

int field[M][N] = { 0 };

struct Point
{
    int x, y;
} a[4], b[4];

int figures[7][4] =
{
    1,3,5,7,    // I
    2,4,5,7,    // Z
    3,5,4,6,    // S
    3,5,4,7,    // T
    2,3,5,7,    // L
    3,5,7,6,    // J
    2,3,4,5     // O
};

bool check()
{
    for (int i = 0; i < 4; i++)
        if (a[i].x < 0 || a[i].x >= N || a[i].y >= M)
            return 0;
        else if (field[a[i].y][a[i].x])
            return 0;

    return 1;
};

int main()
{
    srand(time(0));

    RenderWindow window(VideoMode(320, 480), "Tetris!");
    window.setFramerateLimit(60);

    Texture t1, t2, t3;
    t1.loadFromFile("images/tiles.png");
    t2.loadFromFile("images/background.png");
    t3.loadFromFile("images/frame.png");


    Music backgroundmusic;
    backgroundmusic.openFromFile("music/backgroundmusic.wav");
    
    backgroundmusic.setLoop(true);
    backgroundmusic.play();

    Sprite s(t1), background(t2), frame(t3);

    int dx = 0;
    bool rotate = 0;
    int colorNum = 1;
    float timer = 0, delay = 0.3;

    int score = 0;
    int highscore = 0;
    bool gameOver = false;

    // Načtení fontu pro zobrazení textu
    Font font;
    if (!font.loadFromFile("font/arial.ttf"))
        return -1;

    Text textScore, textHighscore;
    textScore.setFont(font);
    textHighscore.setFont(font);
    textScore.setCharacterSize(20);
    textHighscore.setCharacterSize(20);
    textScore.setFillColor(Color::Black);
    textHighscore.setFillColor(Color::Black);
    textScore.setPosition(10, 400);
    textHighscore.setPosition(10, 420);

    Clock clock;

    while (window.isOpen())
    {
        float time = clock.getElapsedTime().asSeconds();
        clock.restart();
        timer += time;

        Event e;
        while (window.pollEvent(e))
        {
            if (e.type == Event::Closed)
                window.close();

            if (gameOver)
            {
                
                if (e.type == Event::KeyPressed && e.key.code == Keyboard::R) {
                    // Restart hry
                    for (int i = 0; i < M; i++)
                        for (int j = 0; j < N; j++)
                            field[i][j] = 0;
                    score = 0;
                    gameOver = false;
                }
                continue;
            }

            if (e.type == Event::KeyPressed)
                if (e.key.code == Keyboard::Up)
                    rotate = true;
                else if (e.key.code == Keyboard::Left)
                    dx = -1;
                else if (e.key.code == Keyboard::Right)
                    dx = 1;
        }

        if (e.key.code == Keyboard::Down)
            delay = 0.05;

        for (int i = 0; i < 4; i++) // move
        {
            b[i] = a[i];
            a[i].x += dx;
        }
        if (!check())
            for (int i = 0; i < 4; i++)
                a[i] = b[i];

        if (rotate) // rotate
        {
            Point p = a[1]; // centre of rotation
            for (int i = 0; i < 4; i++)
            {
                int x = a[i].y - p.y;
                int y = a[i].x - p.x;
                a[i].x = p.x - x;
                a[i].y = p.y + y;
            }
            if (!check())
                for (int i = 0; i < 4; i++)
                    a[i] = b[i];
        }

        // tick
        if (timer > delay)
        {
            for (int i = 0; i < 4; i++) { b[i] = a[i]; a[i].y += 1; }
            if (!check())
            {
                for (int i = 0; i < 4; i++) field[b[i].y][b[i].x] = colorNum;

                colorNum = 1 + rand() % 7;
                int n = rand() % 7;
                for (int i = 0; i < 4; i++)
                {
                    a[i].x = figures[n][i] % 2;
                    a[i].y = figures[n][i] / 2;
                }

                // Kontrola na Game Over (bloky dosáhly vrcholu)
                for (int i = 0; i < 4; i++) {
                    if (a[i].y == 0 && field[a[i].y][a[i].x] != 0) {
                        gameOver = true;
                        if (score > highscore) highscore = score; // aktualizace highscore
                    }
                }
            }
            timer = 0;
        }

        // check lines
        int K = M - 1;
        for (int i = M - 1; i > 0; i--)
        {
            int count = 0;
            for (int j = 0; j < N; j++)
            {
                if (field[i][j]) count++;
                field[K][j] = field[i][j];
            }
            if (count == N) // řádek je plný
            {
                score += 100;  // přidáme skóre za odstraněný řádek
            }
            if (count < N) K--;
        }

        dx = 0; rotate = 0; delay = 0.3;

        window.clear(Color::White); // draw
        window.draw(background);

        // Zobrazení polí
        for (int i = 0; i < M; i++)
            for (int j = 0; j < N; j++)
            {
                if (field[i][j] == 0) continue;
                s.setTextureRect(IntRect(field[i][j] * 18, 0, 18, 18));
                s.setPosition(j * 18, i * 18);
                s.move(28, 31); // offset
                window.draw(s);
            }

        // Zobrazení aktuální figury
        for (int i = 0; i < 4; i++)
        {
            s.setTextureRect(IntRect(colorNum * 18, 0, 18, 18));
            s.setPosition(a[i].x * 18, a[i].y * 18);
            s.move(28, 31); // offset
            window.draw(s);
        }

        window.draw(frame);

        // Zobrazení skóre a highscore
        textScore.setString("Score: " + std::to_string(score));
        textHighscore.setString("Highscore: " + std::to_string(highscore));
        window.draw(textScore);
        window.draw(textHighscore);

        // Zobrazení Game Over obrazovky
        if (gameOver)
        {
            Text gameOverText;
            gameOverText.setFont(font);
            gameOverText.setString("GAME OVER\nPress R to Restart");
            gameOverText.setCharacterSize(30);
            gameOverText.setFillColor(Color::Red);
            gameOverText.setPosition(40, 200);
            window.clear(Color::Black);
            window.draw(gameOverText);
           
        }

        window.display();
    }

    return 0;
}







// Spuštění programu: Ctrl+F5 nebo nabídka Ladit > Spustit bez ladění
// Ladění programu: F5 nebo nabídka Ladit > Spustit ladění

// Tipy pro zahájení práce:
//   1. K přidání nebo správě souborů použijte okno Průzkumník řešení.
//   2. Pro připojení ke správě zdrojového kódu použijte okno Team Explorer.
//   3. K zobrazení výstupu sestavení a dalších zpráv použijte okno Výstup.
//   4. K zobrazení chyb použijte okno Seznam chyb.
//   5. Pokud chcete vytvořit nové soubory kódu, přejděte na Projekt > Přidat novou položku. Pokud chcete přidat do projektu existující soubory kódu, přejděte na Projekt > Přidat existující položku.
//   6. Pokud budete chtít v budoucnu znovu otevřít tento projekt, přejděte na Soubor > Otevřít > Projekt a vyberte příslušný soubor .sln.
