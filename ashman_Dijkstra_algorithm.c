#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <time.h>

struct sur{ 	//struktura, kde si uchovavam dlzku cesty, a kod - predchadzajuci v poli a aktualny v halde
	int cesta;	
	int kod;
};

// vklada prvok do minimalnej haldy a vracia naspat haldu, v halde zoraduje prvky podla cesty
struct sur *vloz(struct sur *halda, int cesta, int kod, int *velkost_haldy){
	struct sur akt;			
	akt.cesta = cesta;
	akt.kod = kod;
	
	(*velkost_haldy)++;
	
	halda[*velkost_haldy] = akt;
	int i = *velkost_haldy;
	while ((int)(i/2) >= 1){
		if (halda[i].cesta < halda[(int)(i/2)].cesta || halda[(int)(i/2)].cesta < 0){
			akt = halda[i];
			halda[i] = halda[(int)(i/2)];
			halda[(int)(i/2)] = akt;
			i = (int)(i/2);
		}
		else
			break;	
	}
	return halda;
}

//Vybera najmensi prvok z haldy a upravuje haldu
struct sur vyber(struct sur *halda, int *velkost_haldy){
	struct sur vrat = halda[1]; 
	halda[1].cesta = halda[*velkost_haldy].cesta;
	halda[1].kod = halda[*velkost_haldy].kod;
	
	halda[*velkost_haldy].kod = 0;
	halda[*velkost_haldy].cesta = 0;
	(*velkost_haldy)--;
	
	int i = 1, menej;
	
	while(2*i < 2 * (*velkost_haldy) && (halda[2*i].cesta > 0 || halda[2*i + 1].cesta > 0) && (halda[i].cesta > halda[2*i].cesta || halda[i].cesta > halda[2*i+1].cesta)){
		if (halda[2*i].cesta > 0 && (halda[2*i].cesta < halda[2*i+1].cesta || halda[2*i+1].cesta <= 0))
			menej = 2*i;
		else
			menej = 2*i +1;
			
		struct sur akt = halda[i];
		halda[i] = halda[menej];
		halda[menej] = akt;
		i = menej;
	}
	return vrat;
}


int *zachran_princezne(char **mapa, int n, int m, int t, int *dlzka_cesty){
	struct sur *halda = (struct sur*) calloc (100000, sizeof(struct sur));
	int *velkost_haldy = (int*) malloc (sizeof(int));
	*velkost_haldy = 0;

	int i, j, k = 0, pocet_princezien = 0;
	int princezne[5] = {0};
	int *teleporty[10];
	int pocet_teleportov[10]={0};

	struct sur *pole = (struct sur*) malloc (n * m * 128 * sizeof(struct sur));

	// uklada kde su princezne a zrata pocet teleportov
	for (i = 0; i < n; i++)
		for (j = 0; j < m; j++)
			if (mapa[i][j] == 'P'){
				pocet_princezien++;
				princezne[k] = (i * m+j)*128;
				k++;
			}
			else if (mapa[i][j] >= '0' && mapa[i][j] <= '9'){
				pocet_teleportov[mapa[i][j] - '0']++;
			}
			
	// nasledne 3 forcykly nastavia suradnice teleportom a ulozia ich do pola
	for (i = 0; i < 10; i++)
		teleporty[i] = (int *) malloc (pocet_teleportov[i] * sizeof(int)); 
	
	for (i = 0; i < 10; i++)
		for (j = 0; j < pocet_teleportov[i]; j++)
			teleporty[i][j] = -1;
	
	for (i = 0; i < n; i++)
		for (j = 0; j < m; j++)
			if (mapa[i][j] >= '0' && mapa[i][j] <= '9')
				for (k = 0; k < pocet_teleportov[mapa[i][j] - '0']; k++)
					if (teleporty[mapa[i][j] - '0'][k] < 0){
						teleporty[mapa[i][j] - '0'][k] = (i * m) + j;
						break;
					}
					

	for (i = 0; i < n; i++)
		for (j = 0; j < m; j++)
			for (k = 0; k < 128; k++){
				pole[(i*m + j)*128+k].cesta = -1;
				pole[(i*m + j)*128+k].kod = -1;
			}
	
	int stav, pom, zvys, posun = 0, ukoncovak = 0, naj_cesta = INT_MAX, koniec;;
	pole[0].cesta = 0;
	int xx, yy, ss, s = 0, y = 0, x = 0;

	// Zaciatok Dijkstrovho algoritmu
	while (1){
		if (mapa[x][y] >= '0' && mapa[x][y] <= '9' && s >= 64)	
			//v pripade ze som na teleporte po aktivovani generatora tak pripocita k obycajnemu forcyklu (4)aj pocet teleportov, kde sa moze popolvar pohnut 
			pom = pocet_teleportov[mapa[x][y] - '0'];
		else	
			pom = 0;
		for (i = 0; i < 4 + pom; i++){
			//Vyberam smer kde sa ma pohnut popolvar
			xx = x;
			yy = y;
			ss = s;
			if (i == 0)
				xx = x + 1;	
			else if (i == 1)
				xx = x - 1;	
			else if (i == 2)
				yy = y + 1;		
			else if (i == 3)
				yy = y - 1; 	 
			else 
				if (x != teleporty[mapa[x][y] - '0'][i - 4] / m){
					xx = teleporty[mapa[x][y] - '0'][i - 4] / m;
					yy = teleporty[mapa[x][y] - '0'][i - 4] % m; 
				}
				else 
					continue;
			if (xx < 0 || yy < 0 || xx >= n || yy >= m || mapa[xx][yy] == 'N')
				continue;

			switch (mapa[x][y]){	//Ohodnotim jednotlive hrany a pripadne zmenim stav
				case 'P':
					zvys = 1;
					if ((ss >> 5) & 1)
						for (j = 0; j < pocet_princezien; j++)
							if ((x*m + y)*128 == princezne[j]){
								posun = 1;
								posun = posun << j;
								ss = ss | posun;
							}	
		        	break;
		        case 'C':
		        	zvys = 1;
		        	break;
		        case 'H':
		        	zvys = 2;
		        	break;
				case 'D':
					zvys = 1;
					if (((ss >> 5) & 1 ) == 0)
						ss += 32;
					break;
				case 'G':
					zvys = 1;
					if (((ss >> 6) & 1) == 0)
						ss += 64;
					break;	
		        default:
					if ( i < 4 )
						zvys = 1;
					else
						zvys = 0;
		        	break;
			}

			//Nastavujem nove suradnice v poli, a vkladam prvok do haldy
			if (pole[(xx * m + yy) * 128 + ss].cesta < 0 || pole[(xx * m + yy) * 128 + ss].cesta > pole[(x * m + y) * 128 + s].cesta + zvys){
				halda = vloz(halda, pole[(x * m + y) * 128 + s].cesta + zvys, (xx * m + yy) * 128 + ss, velkost_haldy);
				
				pole[(xx * m + yy) * 128 + ss].kod = (x * m + y) * 128 + s;
				pole[(xx * m + yy) * 128 + ss].cesta = pole[(x * m + y) * 128 + s].cesta + zvys;
			}
			//pokial su vsetky princezne zachranene nastavim ukoncovak na 1 a dobehnem haldu pokial je mensia ako aktualna cesta (sice nie je potrebne kedze dlzky hran su 1 alebo 2)
			stav = 1;
			for (j = 0; j < pocet_princezien; j++)
				if (((ss >> j) & 1 ) == 0)
					stav = 0;
			
			if (stav && naj_cesta > pole[(x * m + y) * 128 + s].cesta){
				ukoncovak = 1;
				naj_cesta = pole[(x * m + y) * 128 + s].cesta;
				koniec = (x * m + y) * 128 + s;
			}	
		}
	 	if (ukoncovak && naj_cesta <= halda[1].cesta)
			break;

		//vyberam z haldy prvok s najmensou cestou, nastavim suradnice aj stav
		struct sur dalsi = vyber(halda, velkost_haldy);
		x = (dalsi.kod/128) / m;
		y = (dalsi.kod/128) % m;
		s = dalsi.kod % 128;
	}
	
	int pocet = 0;
	pom = koniec;    
	//zistujem spatne pocet suradnic najkratsej cesty
	while (1){
		pocet++;
		if (koniec/128/m == 0 && koniec/128 % m == 0 && koniec%128 == 0)
			break;
		koniec = pole[koniec].kod;
	} 

	int *p = (int *) malloc (2 * pocet * sizeof(int));	
	koniec = pom;

	//Najkratsiu cestu ukladam do pola ktore potom vratim z funkcie
	for (j = 0; j < pocet; j++){
		p[2*pocet - (2*j) - 2] = koniec/128 % m;
		p[2*pocet - (2 * j + 1)] = koniec/128 / m;
		koniec = pole[koniec].kod;
	}

	*dlzka_cesty = pocet;
	free(pole);	
	free(halda);
	return p;
}


void testovac(int dlzka, int sirka){
	int i,j, cislo;
	char **mapka;
	clock_t start, end;
    double cpu_time_used;
    int *dialka = malloc (sizeof(int));
    
	srand(time(0)); 
    
	mapka = (char **) malloc (dlzka * sizeof(char*));	
	
	for (i = 0; i < dlzka; i++)
		mapka[i] = (char *) malloc (sirka * sizeof(char));
	
	for (i = 0; i < dlzka; i++)
		for (j = 0; j < sirka; j++){
			cislo = rand();
			if (cislo > 15000)
				mapka[i][j] = 'C';
			else if (cislo > 3500)
				mapka[i][j] = 'H';
			else if (cislo > 1000)
				mapka[i][j] = 'N'; 
			else
				mapka[i][j] = (cislo / 200) + '0';
		}
		
	mapka[dlzka / 2][sirka / 3] = 'P';
	mapka[1589 % dlzka][1589 % sirka] = 'P';
	mapka[dlzka / 4][sirka / 2] = 'P';
	mapka[dlzka - 1][sirka / 3] = 'P';
	mapka[dlzka / 2][sirka - 2] = 'P';
	mapka[dlzka / 5][sirka / 2] = 'G';
	mapka[dlzka / 2][dlzka / 2] = 'D';
	mapka[0][0] = 'C';
	
	for (i = 0; i < dlzka; i++){
		for (j = 0; j < sirka; j++)
			printf("%c",mapka[i][j]);
		printf("\n");
	}
	int *vysledok;
		
	start = clock();
	
	vysledok = zachran_princezne(mapka, dlzka, sirka, 2520, dialka);	

	end = clock();
    
	cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
	
	
	for (j = 0; j < 2*(*dialka); j++){
		if (!(j % 2))
			printf("%d %d\n", vysledok[j], vysledok[j+1]);
	}
	
    printf("Cas: %lf\n",cpu_time_used);
    
	free(dialka); 
}


int main() {
	int sirka, dlzka;
	
	while (scanf("%d %d", &dlzka, &sirka) == 2){	
		testovac(dlzka, sirka);
	}

	return 0;
}
