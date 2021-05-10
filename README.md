# Automat komórkowy

## Problem badawczy
Celem jest badanie zachowań automatu komórkowego — układu
składającego się z czworokątnych komórek ułożonych w siatkę.
Zakładamy, że każda z komórek przyjmuje stan aktywny bądź nieaktywny i zmienia 
go w zależności od wartości [komórek sąsiednich](https://pl.wikipedia.org/wiki/Sąsiedztwo_Moore’a).
Ponadto przyjmujemy [periodyczny warunek brzegowy](https://pl.wikipedia.org/wiki/Automat_komórkowy#Periodyczne_(przenikające)) — komórki na krawędzi układu
jako swoich sąsiadów będą traktować pola z drugiej strony.

## Rozwiązanie problemu
Program jest w stanie symulować automat komórkowy dla różnych warunków
przejścia stanów komórek. Warunki zapisywane są w pliku konfiguracyjnym *config*
jako parametr *bornConditions* określający ilości sąsiednich komórek aktywnych,
przy których pole nieaktywne zostanie aktywowane oraz *surviveConditions*
definiujący, kiedy komórka ma przetrwać iterację. 
Ilość przebiegów symulacji kontroluje opcja *iter*.
Rozmiar układu zależny jest od *boardSize* natomiast wielkość pojednyczej
komórki w wyjściowym pliku graficznym od *fieldSize*.
Forma danych wyjściowych ustalana jest przez parametr *outputMode* zapisany jako
suma wartości odpowiadającym trybom:
1. Rysowanie stanów układu we wszystkich iteracjach jako grafiki i zapisywanie ich jako frame*.bmp
2. Rysowanie finalnego stanu układu jako grafiki zapisanej jako output.bmp
4. Wyświetlanie finalnego stanu w konsoli gdzie " _ " reprezentuje komórkę nieaktywną a " @ " aktywną
8. Wypisanie finalnego stanu układu do pliku tekstowego (output.txt)
Stan początkowy układu jest definiowany przy pomocy parametru *init*
poprzez podanie pozycji komórek aktywnych w formacie **_kolumna_-_wiersz_,**
gdzie indeksy numerowane są od zera do wielkości układu pomniejszonego o 1.

## Przykład użycia
Załóżmy, że chcemy zasymulować układ *[Gry w życie](https://pl.wikipedia.org/wiki/Gra_w_życie#Opis_reguł_gry)* na zasadach [Johna Conwaya](https://pl.wikipedia.org/wiki/John_Horton_Conway).
Potrzebujemy zatem ustawić w pliku konfiguracyjnym odpowiednie wartości
```
bornConditions=3
surviveConditions=2,3
```
Chcemy zasymulować 512 iteracji układu, zatem ustawiamy wartość iter na 512
```
iter=512
```
Interesuje nas układ o rozmiarze 64x64 gdzie jedna komórka reprezentowana będzie przez pojedynczy piksel
```
fieldSize=1
boardSize=64
```
Stan końcowy układu chcemy otrzymać jako plik tekstowy oraz wszystkie iteracje jako pliki graficzne (8+2)
```
outputMode=10
```
Jako stan początkowu układu ustawiamy strukturę *Gosper glider gun* w następujący sposób:
```
init=25-1,25-2,23-2,13-3,14-3,21-3,22-3,35-3,36-3,12-4,16-4,21-4,22-4,35-4,36-4,1-5,2-5,11-5,17-5,21-5,22-5,1-6,2-6,11-6,15-6,17-6,18-6,23-6,25-6,11-7,17-7,25-7,12-8,16-8,13-9,14-9,
```

Po uruchomieniu w folderze z programem zostanie utworzone 513 grafik (gdzie pierwsza to stan początkowy) o rozmiarze 64x64.
Przy użyciu programu imagemagick możemy utworzyć gif
```
convert -scale 800% -delay 10 -loop 0 frame*.bmp example1.gif
```
A tak prezentuje się efekt:
[przyklad](example1.gif)

## Wykorzystane biblioteki
[turtle.h](https://w3.cs.jmu.edu/lam2mo/cs240_2015_08/turtle.html)

## Instrukcja kompilacji
### Ubuntu
Aby skompilować program należy użyć tych komend w folderze repozytorium.
```
gcc -c turtle.c -o turtle.o -lm
gcc -Wall -Wextra core.c turtle.o -lm
```
### Visual Studio
1. Sklonuj repozytorium np. poprzez wklejenie linku url
2. Przejdź do
	- File
	- New
	- Project From Existing Code...
	- wybierz Visual C++ 
	- wybierz folder z repozytorium i podaj nazwę projektu
	- zakończ tworzenie
3. Przejdź do
	- Build
	- Batch Build... 
	- Wybierz odpowiednią architekruerę i skompiluj program
	- pamiętaj dodać plik konfiguracyjny, aby program mógł działać poprawnie