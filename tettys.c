/* Tietrys! Heil Aleksiej Pazinov
   jest Twoj, robta co chceta. */

#include<time.h> /* do obslugi czasu */
#include<curses.h> /* do wyswietlania (kolorowych) znakow */

int gra; /* stan gry: 1 trwa, 0 zakonczona */

typedef char Kostka; /* moze i smieszne ale pomysl ze mozesz zawsze
		        zamienic ten char np na struct{int kolor,char znak;}
		        albo jakies inne cudo. */

Kostka pusta='.',pelna='#';

/* tetrion - studnia do ktorej wpadaja klocki. tradycyjna ma wymiary 10x20.
             poniewaz jednak dbamy o elastycznosc nazwiemy te wartosci
             SZE i WYS, zeby zmiana rozmiarow tetrionu mogla nastapic latwo
             i bezbolesnie. */
enum{SZE=10,WYS=20};
Kostka Tetrion[SZE][WYS];

Kostka cotetrion(int x,int y)
/* zwraca zawartosc tetrionu na pozycji x,y; przyjmuje ze poza
   sama studnia wszedzie jest "mur" a ponad nia jest pustka. */
{
  if(x<0 || x>=SZE || y>=WYS) return pelna;
  if(y<0) return pusta;
  return Tetrion[x][y];
}

void wstawtetrion(int x,int y,Kostka k)
/* taka niby niezbyt ambitna funkcja, ale zabezpiecza nam
   dostep do tetrino[][] - ze nie wylezie ktos poza ta tablice itp. */
{
  if(x>=0 && x<SZE && y>=0 && y<WYS) Tetrion[x][y]=k;
}

/* tetrino - klocek tetrisa (4 polaczone kostki). istnieje 7 takich klockow,
             o ksztaltach liter L J S Z T O I, ktore widac w tetrina[].
             */
typedef Kostka Tetrino[4][4];

Tetrino tetrina[7]={{".X..",
 		     ".X..",
 		     ".XX.",
 		     "...."},

                    {"..%.",
 		     "..%.",
 		     ".%%.",
 		     "...."},

                    {".0..",
 		     ".00.",
 		     "..0.",
 		     "...."},

                    {"..@.",
 		     ".@@.",
 		     ".@..",
 		     "...."},

                    {"....",
 		     ".$$$",
 		     "..$.",
 		     "...."},

                    {"....",
 		     ".**.",
 		     ".**.",
 		     "...."},

                    {".#..",
 		     ".#..",
 		     ".#..",
 		     ".#.."}};

/* spadajace tetrino mozna opisac przez 4 wartosci:
   - typ klocka (0-6, gdzie 0 to L, 1 to J itd)
   - pozycja x,y gornego lewego rogu klocka
   - obrot klocka (0:wcale, 1:90st w prawo, 2:180st, 3:90st w lewo)
     
     #    ###   #   # 
     #    #     #   ###
     ##        ##

      0    1    2    3   */

struct {int typ,obrot,x,y;} obecne;
int nastepne;

Kostka cotetrino(int typ,int obrot,int i,int j)
/* zwraca kostke klocka TYP o obrocie OBROT znajdujaca sie na
   pozycji (i,j) gdzie 0<=i,j<4. sedno tej funkcji to uwzglednienie
   obrotu. */
{
  int x,y;

  switch(obrot)
    {
    case 0: x=i;y=j; break;
    case 1: x=3-j;y=i; break;
    case 2: x=3-i;y=3-j; break;
    case 3: x=j;y=3-i; break;
    }
  return tetrina[typ][x][y];
}

void wylosuj_nastepne(){nastepne=rand()%7;} /* yeah */

void nastepne_tetrino() /* nowe tetrino u gory... */
{
  obecne.typ=nastepne;
  obecne.obrot=rand()%4;
  obecne.x=SZE/2-2;
  obecne.y=0;
  wylosuj_nastepne();
}

int dozwolony_ruch(int dx,int dy,int obr)
/* zwraca 1 gdy obecne tetrino moze znalezc sie na pozycji
   przesunietej o dx,dy wzgledem jegoz obecnej przy obrocie
   obr nie nachodzac przy tym na zadna czesc tetrionu
   (ze scianami i podloga wlacznie), lub 0 w przeciwnym razie. */
{
  int i,j;

  for(i=0;i<4;i++)
    for(j=0;j<4;j++)
      if(cotetrino(obecne.typ,obr,i,j)!=pusta
	 && cotetrion(obecne.x+dx+i,obecne.y+dy+j)!=pusta) return 0;
  return 1;
}

void zrzuc(int linia)
/* przesuwa wszystkie wiersze od 0 do LINIA-1 o 1 w dol. */
{
  int i,j;

  for(j=linia;j>=0;j--)
    for(i=0;i<SZE;i++)
      wstawtetrion(i,j,cotetrion(i,j-1));  
}

void trach()
/* czyli co sie dzieje kiedy klocek uderza o dno studni,
   albo o inne wczesniej-spadniete-klocki. kasowanie linii,
   sprawdzanie czy to nie przegrana. */
{
  int i,j;
  Kostka k;

  for(i=0;i<4;i++)
    for(j=0;j<4;j++)
      if((k=cotetrino(obecne.typ,obecne.obrot,i,j))!=pusta)
	wstawtetrion(obecne.x+i,obecne.y+j,k);
  /* jakies linie do usuniecia? */
  for(j=0;j<WYS;j++)
    {
      for(i=0;i<SZE;i++)
	if(cotetrion(i,j)==pusta) break;
      if(i==SZE) /* linia j na calej szerokosci zapelniona! */ 
        zrzuc(j);
    }
  if(obecne.y<=0) gra=0;
  nastepne_tetrino();
}

void rysuj()
{
  int i,j;
  Kostka c;

  attron(COLOR_PAIR(1));
  for(j=0;j<WYS;j++)
    {
      move(j+1,1);
      for(i=0;i<SZE;i++)
	printw("%c",cotetrion(i,j));
    }
  for(i=0;i<4;i++)
    for(j=0;j<4;j++)
      {
	move(2+j,SZE+3+i);
	printw("%c",cotetrino(nastepne,0,i,j));
      }
  attron(COLOR_PAIR(2));
  for(i=0;i<4;i++)
    for(j=0;j<4;j++)
      if((c=cotetrino(obecne.typ,obecne.obrot,i,j))!=pusta)
       {
	 move(1+obecne.y+j,1+obecne.x+i);
	 printw("%c",c);
       }

  refresh();
}


void inicjuj_ekran()
{
  int i;
  clear();
  attron(COLOR_PAIR(3));
  for(i=0;i<WYS;i++)
    {
      move(i+1,0);printw("|");
      move(i+1,SZE+1);printw("|");
    }
  for(i=0;i<SZE;i++)
    {
      move(WYS+1,i+1); printw("=");
    }
  move(1,SZE+2); printw("NASTEPNY:");
  move(8,SZE+4); printw("4 lub z - w lewo");
  move(9,SZE+4); printw("6 lub c - w prawo");
  move(10,SZE+4); printw("2 lub x - spada szybciej");
  move(11,SZE+4); printw("8 lub a - obrot (w prawo)");
  refresh();
}

void inicjuj_tetrion()
{
  int i,j;

  for(i=0;i<SZE;i++)
    for(j=0;j<WYS;j++)
      Tetrion[i][j]=pusta;
}

void inicjuj_curses()
/* pierdoly zwiazane z biblioteka curses. */
{
  initscr(); /* zacznij uzywac curses */
  start_color(); /* zacznij uzywac kolorow */
  init_pair(1,COLOR_MAGENTA,COLOR_BLACK); /* trzy zestawy kolorow: 1:fiolet, */
  init_pair(2,COLOR_GREEN,COLOR_BLACK);   /* 2:zielen, 3:seledyn (wszystkie  */
  init_pair(3,COLOR_CYAN,COLOR_BLACK);    /* na czarnym tle).                */
  noecho(); /* nie wyswietla wciskanych znakow */
  cbreak(); /* ciekawe co to, ale powinno tu stac wg dokumentacji*/
  nodelay(stdscr,TRUE); /* nie czeka az cos wcisniesz przy getch(),
			   dzieki czemu tetrina spadaja nawet gdy gracz
			   nic nie wciska. */
}

void zakoncz_curses(){endwin();} /* ta funkcja hakuje internet i usuwa go */

int main() /* acha! tu jestes! */
{
  clock_t wtedy,teraz,co_ile; /* zeby mierzyc czas. patrz nizej. */
  int dx,dy,obr;

  co_ile=CLOCKS_PER_SEC/4;  /* szybkosc gry. tu: 4 razy na sek. */
  inicjuj_curses(); /* zeby moc elegancko wyswietlac znaki */
  srand(time(0)); /* zeby liczby pseudolosowe byly bardziej losowe */
  inicjuj_tetrion();
  inicjuj_ekran(); /* studnia, info o obsludze total wiesz */
  wylosuj_nastepne(); 
  nastepne_tetrino(); 
  gra=1;
  while(gra) /* petla gry */
    {
      rysuj();
      wtedy=teraz=clock();
      while(teraz-wtedy<co_ile)
	{
	  dx=dy=0;
	  obr=obecne.obrot;
	  switch(getch())
	    {
	    case 'q': gra=0; teraz+=co_ile; break;
	    case 'z': case '4': dx--; break;
	    case 'c': case '6': dx++; break;
	    case 'x': case '5': case '2': dy++; break;
	    case 'a': case 'r': case '8': if((++obr)>3) obr=0;
	    }	  
	  if(dozwolony_ruch(dx,dy,obr))
	    {
	      obecne.x+=dx;
	      obecne.y+=dy;
	      obecne.obrot=obr;
	      rysuj();
	    }
	  teraz=clock();
	}
      if(dozwolony_ruch(0,1,obecne.obrot)) obecne.y++;
      else trach();
    }
  /* no i koniec... */
  zakoncz_curses();
  return 0;
}
