#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"
#include "Stos.h"
#include "ListaSasiedztwa.h"
#include <random>

/*						Generator Labirytow
 * Ponizszy program to zastosowanie algorytmu z nawrotami (ang. Backtracking)
 * jest to modyfikacja algorytmu typu DFS (Deep first search). Reprezentacja
 * labirytnyu to graf, spojny i nieskierowany. Przejscie takiego grafu metoda
 * DFS daje gwarancje tego, ze odwiedzimy kazdy wiercholek grafu - kazda komorke
 * naszego labirytnu.
 * 
 * Wiemy, ze zlozonosc algorytmu DFS wynosi O(V+E);
 * [gdzie V - ilosc wierzcholkow; E - ilosc krawedzi]
 * Muismy odwiedzic kazdy wiercholek (V) oraz probowac
 * dostac sie do niego poprzez kazda krawedz (E).
 * Jesli wierzcholek, do ktorego porwadzi nas inna krawedz
 * byl juz odwiedzony, a nie mamy innego sasiada do przejscia,
 * musimy sie cofac, az do momentu, kiedy bedzie mozliwosc
 * wyboru innego nieodwiedzonego wierzcholka.
 * 
 * Tak wiec gdybysmy chcieli tylko i wylacznie wygenerowac
 * labirynt, to zlozonosc czasowa bedzie wynosila wlasnie O(V+E).
 * [Ten program prowadzi obliczenia w Runtime, a nie np. compiled time,
 * [dzieki temu uzyskujemy efekt 'tworzenia sie' labiryntu]
 * 
 * Skomplikowaniu labiryntu pomaga fakt, ze sasiada, do ktorego
 * chcemy dojsc, wybieramy w sposob losowy. Oczywiscie mozna
 * wybierac np. pierwszego mozliwego sasiada, natomiast moze to
 * poprowadzic do bradzo 'nudnego' wzoru labiryntu (latwego do rozwiazania).
 * Wzor bedzie takze silnie zalezny od struktury danych uzytej do 
 * przetrzymywania grafu. Niedeterministycznosc tzw 'randomized DFS'
 * jest pozadanym efektem naszego algorytmu.
 * 
 * Jako silnik graficzny zostal uzyty silnik olcPixelGameEnigne
 * autorstwa javidx9 na licencji License (OLC-3).
 * 
 * W celu uruchomienia programu przeczytaj RAEDME!
*/

using namespace std::chrono_literals;

enum {
	PATH_TO_EAST = 0x800,
	PATH_TO_SOUTH = 0x1000,
	CELL_VISITED = 0x4000,
	CELL_BACKTRACK = 0x8000
};

// Macro do wyliczania indeksu
#define CALCULATE_INDEX(searchPoint, pathWidth, mazeWidth) (((int)(searchPoint.x) / (pathWidth)) + ((((int)(searchPoint.y))/(pathWidth)))*(mazeWidth))

class GeneratorLabiryntow : public olc::PixelGameEngine
{
private:

	// Uzytkowe zmienne
	// Punkt szukania rozwiazania
	olc::vf2d searchPoint;
	// Szerekosc i wysokosc labiryntu (defininiowana jako ilosc komorek)
	int width, height;
	// Szerokosc klocka w odniesieniu do ustawionych pikseli *
	const int pathWidth = 4;

	// Struktury danych uzyte do algorytmu
	Stos stack;
	ListaSasiedztwa *lst;
	// Ta tablica przetrzymuje stan komorek labiryntu
	// (pelni role tablicy visited dla algorytmu dfs oraz
	// zawiera informacje dodatkowe np. w ktora strone narysowac
	// przejscie)
	int *cellArray;
	// Mala tablica, uzywana w celu losowego wyboru sasiada
	// Kazda komorka ma maksymalnie 4 sasiadow
	int randomizeArray[4];
	// Tablica do przechowywania sciezki
	int *prevArray;
	// Zmienne pomocnicza
	int isDone;
	int backTrackCell;

public:
	GeneratorLabiryntow(int w, int h, ListaSasiedztwa* lst)
	{
		sAppName = "GeneratorLabiryntow";
		this->lst = lst;
		this->width = w;
		this->height = h;
		this->isDone = 0;
		this->backTrackCell = -1;
	}
	~GeneratorLabiryntow() {
		delete[]cellArray;
		delete[]prevArray;
	}

public:
	bool OnUserCreate() override
	{
		// W tym miejscu silnika zalecana jest inicjalizacja wszystkich zmiennych
		cellArray = new int[width * height];
		prevArray = new int[width * height];
		// Tablice komorek wypelnia sie 0 (a tablice losowego wyboru -1) zlozonosc memset to O(n)
		memset(cellArray, 0x00, width * height * sizeof(int));
		memset(prevArray, -0x01, width * height * sizeof(int));
		memset(randomizeArray, -0x01, 4 * sizeof(int));
		// Dodajemy losowa liczbe na gore stosu O(1)
		std::random_device rd;
		std::mt19937 mt(rd());
		std::uniform_int_distribution<int> dist(0, 599);
		int random = dist(mt);
		// Dodaj oraz ustaw na odwiedzony
		stack.Push(random);
		cellArray[random] |= CELL_VISITED;
		isDone++;

		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		// chrono / radnom
		std::this_thread::sleep_for(1ms);
		std::random_device rd;
		std::mt19937 mt(rd());
		std::uniform_int_distribution<int> dist(0, 3);

		if (isDone != (width*height)) {
			// Wyciagamy element ze stosu (ale go nie usuwamy)
			int prevVertex = stack.TopElem();

			// Ustawiamy wskaznik do pierwszego sasiada wiercholka
			Node* edgePointer = lst->matrixList[prevVertex];
			bool hadNeighbour = false;

			// Dopoki mamy jakiegokolwiek sasiada nieodwiedzonego
			// Dodajemy go do randomizeArray w losowe miejsce tej tablicy
			// Maksymalnie ta petle wywolamy 4 razy
			while (edgePointer != nullptr) {
				if (!(cellArray[edgePointer->nodeNumber] & CELL_VISITED)) {
					int random = dist(mt);
					while (randomizeArray[random] != -1)
						random = dist(mt);
					randomizeArray[random] = edgePointer->nodeNumber;
					// Jesli mielismy sasiada do ktorego moglismy pojsc
					// ustawiamy flage hadNeighbour na prawde
					// w ten sposob wiemy, czy mozemy isc 'dalej'
					// czy musimy sie cofac
					hadNeighbour = true;
				}
				edgePointer = edgePointer->edgeToNode;
			}

			// Jesli mielismy sasiada (sasiadow)
			if (hadNeighbour) {

				// Wybierz losowego sasiada
				int takeRandomRoute = dist(mt);
				while (randomizeArray[takeRandomRoute] == -1)
					takeRandomRoute = dist(mt);

				int takenRouteThroughCell = randomizeArray[takeRandomRoute];

				// Wyczysc podreczna tablice sasiadow
				memset(randomizeArray, -1, 4 * sizeof(int));

				// Dodajemy elemnt na stos O(1)
				stack.Push(takenRouteThroughCell);
				prevArray[takenRouteThroughCell] = prevVertex;
				isDone++;
				// Ustawiamy element jako odwiedzony O(1)
				cellArray[takenRouteThroughCell] |= CELL_VISITED;

				// Sprawdzamy, gdzie wokol nas znajduje sie wybrany sasiad
				// moze byc albo North South East West od nas
				// Analogicznie ustawiamy nasze przejscia w tablicy komorek
				// Operacja O(1)
				if (takenRouteThroughCell > prevVertex) {
					if ((takenRouteThroughCell - 1) == prevVertex)
						cellArray[prevVertex] |= PATH_TO_EAST;
					else
						cellArray[prevVertex] |= PATH_TO_SOUTH;
				}
				else if ((takenRouteThroughCell + 1) == prevVertex) {
					cellArray[takenRouteThroughCell] |= PATH_TO_EAST;
				}
				else if (takenRouteThroughCell < prevVertex) {
					cellArray[takenRouteThroughCell] |= PATH_TO_SOUTH;
				}
			}
			// W przypadku braku sasiadow, musimy cofnac sie do wierzcholka, ktory takowego posiada
			// tak wiec schodzimy w dol stosu
			else {
				// O(1)
				stack.Pop();
			}
		}

		if (backTrackCell != -1) {
			backTrackCell = prevArray[backTrackCell];
			cellArray[backTrackCell] |= CELL_BACKTRACK;
		}

		if (GetMouse(olc::Mouse::LEFT).bPressed) {
			olc::vf2d mouse = { float(GetMouseX()), float(GetMouseY()) };
			searchPoint = mouse;
			std::cout << "Komorka: " << CALCULATE_INDEX(searchPoint, (pathWidth+1), width) << std::endl;
			backTrackCell = CALCULATE_INDEX(searchPoint, (pathWidth + 1), width);
			cellArray[backTrackCell] |= CELL_BACKTRACK;
		}

		if (GetKey(olc::ENTER).bPressed) {
			// Wyczysc dla innej sciezki operacja O(n)
			for (int i = 0; i < (width * height); i++)
				cellArray[i] -= cellArray[i] & CELL_BACKTRACK ? CELL_BACKTRACK : 0;
		}

		// Ta czesc algorytmu odpowiada za wysietlanie odpowiednich 
		// obiektow na ekranie, jest wiec czescia budowy silnika graficznego
		// a nie naszego algorytmu, dlatego zrezygnowano ze szczegolowego opisu tej czesci.

		Clear(olc::BLACK);

		// Iteruj po kazdym bloku labiryntu
		for (int x = 0; x < width; x++) {
			for (int y = 0; y < height; y++) {

				// Rysuj bloczek
				for (int wallX = 0; wallX < pathWidth; wallX++) {
					for (int wallY = 0; wallY < pathWidth; wallY++) {
						// Jesli bloczek byl odwiedzony rysuj innym kolorem
						if (cellArray[y * width + x] & CELL_VISITED) {
							Draw(x * (pathWidth + 1) + wallX, y * (pathWidth + 1) + wallY);
							if (cellArray[y * width + x] & CELL_BACKTRACK)
								Draw(x * (pathWidth + 1) + wallX, y * (pathWidth + 1) + wallY, olc::BLUE);
						}
						else
							Draw(x * (pathWidth + 1) + wallX, y * (pathWidth + 1) + wallY, olc::DARK_MAGENTA);
					}
				}

				// Jesli klocek zniszczyl sciane to ja zamaluj
				for (int i = 0; i < pathWidth; i++) {
					if (cellArray[y * width + x] & PATH_TO_SOUTH) {
						Draw(x * (pathWidth + 1) + i, y * (pathWidth + 1)+ pathWidth);
						if (cellArray[y * width + x] & CELL_BACKTRACK)
							Draw(x * (pathWidth + 1) + i, y * (pathWidth + 1) + pathWidth, olc::BLUE);
					}
					if (cellArray[y * width + x] & PATH_TO_EAST) {
						Draw(x * (pathWidth + 1) + pathWidth, y * (pathWidth + 1)+i);
						if (cellArray[y * width + x] & CELL_BACKTRACK)
							Draw(x * (pathWidth + 1) + pathWidth, y * (pathWidth + 1) + i, olc::BLUE);
					}
				}

			}
		}
		
		return true;
	}
};


int main()
{
	// Utworz liste sadziedtwa dla labiryntu
	// Struktura dostêpna w ListaSasiedztwa.h
	ListaSasiedztwa lst(30, 20);

	// Start silnika - ustawiono ekran 150x100 pixeli
	// kazdy pixel okna to 8 pixeli fizycznych
	GeneratorLabiryntow maze(30, 20, &lst);
	if (maze.Construct(150, 100, 8, 8))
		maze.Start();
	return 0;
}