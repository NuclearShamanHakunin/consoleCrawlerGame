//Marko Jovanović RT-28/15

#include <fstream>
#include <iostream>
#include <string>
#include <stdlib.h>
#include <conio.h>
#include <time.h>
#include <Windows.h>

#pragma warning(disable : 4996)
#define _CRT_SECURE_NO_WARNINGS

//svi ovi karakteri predstavljaju različite zidove, komentarisani su zato sto su mi potrebni samo kao beleska
//#define top_left_corner_wall 201 // karakter ╔
//#define top_right_corner_wall 187 // karakter ╗
//#define bottom_left_corner_wall 200 // karakter ╚
//#define bottom_right_corner_wall 188 // karakter ╝
//#define vertical_wall 186 // karakter ║
//#define horizontal_wall 205 // karakter ═
//#define top_junction_wall 202 // karakter ╩
//#define bottom_junction_wall 203 // karakter ╦
//#define left_junction_wall 185 // karakter ╣
//#define right_junction_wall 204 // karakter ╠
//#define junction_wall 206 // karakter ╬
//#define doorcl 254 // karakter ■ - vrata kroz koja smo prosli

//ovi karakteri predstavljaju teren po kome igrač može da se kreće
#define terrain_safe 46 // karakter .
#define terrain_trap 42 // karakter *
#define terrain_exit 44 // karakter ,

//ovi  karakteri predstavljaju stvorenja u igri i igrača
#define character_player 33 // karakter !
#define character_merchant 77 // karakter M
#define mob_orc 79 // karakter O
#define mob_troll 84 // karakter T
#define mob_wraith 87 // karakter W
#define mob_zombie 90 // karakter Z

#define object_locked_door_vertical 124 // karakter "|", koji prestavlja zaključana vrata(vertikalna)
#define object_locked_door_horizontal char(196) // karakter "─", koji predstavlja zaključana vrata(horizontalna); karakteri veci od 127 moraju da se forsiraju u char
#define object_lootchest 67 // karakter "C", koji predstavlja kovčeg
#define object_lever 76 // karakter "L", koji predstavlja polugu
#define object_lever_activated 95 // karakter "_" koji predstavlja spustenu polugu

#define placeholder_wall 43 // karakter "+" ; bice obrisano, predstavlja zid

#define LEVEL_HEIGHT 18
#define LEVEL_WIDTH 25

//definisanje input tastera

#define KEY_UP 72  //taster "strelica gore"
#define KEY_DOWN 80 //taster "strelica dole"
#define KEY_LEFT 75 //taster "strelica levo"
#define KEY_RIGHT 77 //taster "strelica desno"
#define KEY_USE 101 //taster "e"
#define KEY_ATTACK 97 //taster "a"
#define KEY_DRINK 100 //taster "d"

using namespace std;


//funkcija koja proverava da li je jedan objekat uz drugi objekat u igri (traži se "item" oko pozvanih koordinata)
bool check_near(unsigned char map[LEVEL_HEIGHT][LEVEL_WIDTH], int &x, int &y, int item)
{
	if ((map[x + 1][y]) == item
		|| (map[x - 1][y]) == item
		|| (map[x][y + 1]) == item
		|| (map[x][y - 1]) == item
		|| (map[x + 1][y + 1]) == item
		|| (map[x + 1][y - 1]) == item
		|| (map[x - 1][y + 1]) == item
		|| (map[x - 1][y - 1] == item))
		return true;
	else
		return false;
}

//funkcija za crtanje mape i informacija
void draw_map(unsigned char map[LEVEL_HEIGHT][LEVEL_WIDTH], int hp, int max_hp, int pot, int zhp, int ohp, int thp, int g, int hs)
{
	cout << "\n";
	for (int i = 0; i<LEVEL_HEIGHT; i++)
	{
		std::cout << "\t\t";
		for (int j = 0; j<LEVEL_WIDTH; j++)
		{
			std::cout << (char)map[i][j];
		}
		//stampanje legende
		if (i == 0)
			std::cout << "\tNEPRIJATELJI (napadaju se tasterom A):" << endl;
		else if (i == 1)
			if (zhp > 0)
				std::cout << "\t'Z' - zivi mrtvac, prati i napada igraca, njegov zivot: " << zhp << "/40" << endl;
			else
				std::cout << "\t'Z' - zivi mrtvac, prati i napada igraca, trenutno ga nema na mapi, ili je mrtav" << endl;
		else if (i == 2)
			if (thp > 0)
				std::cout << "\t'T' - trol, tumara i napada igraca ako se dosta priblizi, njegov zivot: " << thp << "/80" << endl;
			else
				std::cout << "\t'T' - trol, tumara i napada igraca ako se dosta priblizi, trenutno ga nema na mapi, ili je mrtav" << endl;
		else if (i == 3)
			if (ohp > 0)
				std::cout << "\t'O' - ork, drzi strazu i napada igraca ako se priblizi, njegov zivot: " << ohp << "/50" << endl;
			else
				std::cout << "\t'O' - ork, drzi strazu i napada igraca ako se priblizi, trenutno ga nema na mapi, ili je mrtav" << endl;
		else if (i == 4)
			std::cout << "\t'W' - izgubljena dusa, tumara i napada igraca ako se priblizi, besmrtna! " << endl;
		else if (i == 6)
			std::cout << "\tINTERAKTIVNI OBJEKTI (aktiviraju se tasterom E):" << endl;
		else if (i == 7)
			std::cout << "\t'L' - podignuta poluga, _ spustena poluga (nije interaktivna)" << endl;
		else if (i == 8)
			std::cout << "\t'C' - kovceg sa blagom i napitkom (napici se piju na taster D) " << endl;
		else if (i == 9)
			std::cout << "\t'|' i '" << char(196) << "' - zatvorena vrata, koja se otvaraju povlacenjem poluge" << endl;
		else if (i == 11)
			std::cout << "\tTEREN (kretnja se izvrsava strelicama na tastaturi)" << endl;
		else if (i == 12)
			std::cout << "\t'*' - nagazna zamka" << endl;
		else if (i == 13)
			std::cout << "\t'.' - siguran teren za kretanje igraca" << endl;
		else if (i == 14)
			std::cout << "\t',' - prelazak na sledeci nivo" << endl;
		else if (i == 16)
			std::cout << "\tIGRAC:" << endl;
		else if (i == 17)
			std::cout << "\t'!' - karatker koji predstavlja igraca" << endl;
		//novi red u matrici
		else
			std::cout << endl;
	}
	std::cout << "\n\n\tTrenutno zivota: " << hp << "/" << max_hp << "\t\tBroj napitaka: " << pot << "\tTrenutno zlata: " << g << " zlatnika" << "\tNajbolji rezultat: " << hs << " zlatnika" << endl;
}
//funkcija za ucitavanje koordinata likova u igru
void character_load(unsigned char map[LEVEL_HEIGHT][LEVEL_WIDTH], int &px, int &py, int &zx, int &zy, int &ox, int &oy, int &tx, int &ty, int &wx, int &wy)
{
	for (int i = 0; i < LEVEL_HEIGHT; i++)
	{
		for (int j = 0; j < LEVEL_WIDTH; j++)
		{
			if (map[i][j] == character_player)
			{
				px = i;
				py = j;
			}
			if (map[i][j] == mob_zombie)
			{
				zx = i;
				zy = j;
			}
			if (map[i][j] == mob_wraith)
			{
				wx = i;
				wy = j;
			}
			if (map[i][j] == mob_orc)
			{
				ox = i;
				oy = j;
			}
			if (map[i][j] == mob_troll)
			{
				tx = i;
				ty = j;
			}
		}
	}
}

//funkcija za pomeranje na gore
void move_up(unsigned char map[LEVEL_HEIGHT][LEVEL_WIDTH], int &x, int &y, int &hp, bool &next_level)
{
	if (map[x - 1][y] == terrain_safe)
	{
		map[x - 1][y] = character_player;
		map[x][y] = terrain_safe;
		x--;
	}
	if (map[x - 1][y] == terrain_trap)
	{
		map[x - 1][y] = character_player;
		map[x][y] = terrain_safe;
		x--;
		hp = hp - 3;
	}
	if (map[x - 1][y] == terrain_exit)
	{
		next_level = true;
	}
}
//funkcija za pomeranje na dole
void move_down(unsigned char map[LEVEL_HEIGHT][LEVEL_WIDTH], int &x, int &y, int &hp, bool &next_level)
{
	if (map[x + 1][y] == terrain_safe)
	{
		map[x + 1][y] = character_player;
		map[x][y] = terrain_safe;
		x++;
	}
	if (map[x + 1][y] == terrain_trap)
	{
		map[x + 1][y] = character_player;
		map[x][y] = terrain_safe;
		x++;
		hp = hp - 3;
	}
	if (map[x + 1][y] == terrain_exit)
	{
		next_level = true;
	}
}
//funkcija za pomeranje na levo
void move_left(unsigned char map[LEVEL_HEIGHT][LEVEL_WIDTH], int &x, int &y, int &hp, bool &next_level)
{
	if (map[x][y - 1] == terrain_safe)
	{
		map[x][y - 1] = character_player;
		map[x][y] = terrain_safe;
		y--;
	}
	if (map[x][y - 1] == terrain_trap)
	{
		map[x][y - 1] = character_player;
		map[x][y] = terrain_safe;
		y--;
		hp = hp - 3;
	}
	if (map[x][y - 1] == terrain_exit)
	{
		next_level = true;
	}
}
//funkcija za pomeranje na desno
void move_right(unsigned char map[LEVEL_HEIGHT][LEVEL_WIDTH], int &x, int &y, int &hp, bool &next_level)
{
	if (map[x][y + 1] == terrain_safe)
	{
		map[x][y + 1] = character_player;
		map[x][y] = terrain_safe;
		y++;
	}
	if (map[x][y + 1] == terrain_trap)
	{
		map[x][y + 1] = character_player;
		map[x][y] = terrain_safe;
		y++;
		hp = hp - 3;
	}
	if (map[x][y + 1] == terrain_exit)
	{
		next_level = true;
	}
}

//funkcija za taster d (taster za napitke)
void drink(int &hp, int maxhp, int &potnum)
{
	int healing, healing_done, hp_before;
	healing = 8 + rand() % 5;
	if (potnum > 0)
	{
		if (maxhp < hp + healing)
		{
			hp_before = hp;
			hp = maxhp;
			potnum--;
			healing_done = maxhp - hp_before;
			std::cout << "\n\tIzlecili ste >" << healing_done << "< poena zivota";
		}
		else
		{
			hp_before = hp;
			hp = hp + healing;
			potnum--;
			healing_done = hp - hp_before;
			std::cout << "\n\tIzlecili ste >" << healing_done << "< poena zivota";
		}
	}

	else
		std::cout << "\n\tNemate vise napitaka";
	Sleep(1000);
}
//funckija za taster e (taster za korišćenje)
void use(unsigned char map[LEVEL_HEIGHT][LEVEL_WIDTH], int &x, int &y, int &pot, int&g)
{
	//u slucaju da je uz lika iz igre poluga
	if (check_near(map, x, y, object_lever))
	{
		for (int i = 0; i < LEVEL_HEIGHT; i++)
		{
			for (int j = 0; j < LEVEL_WIDTH; j++)
			{
				if ((map[i][j] == object_locked_door_vertical) || (map[i][j] == object_locked_door_horizontal))
					map[i][j] = terrain_safe;
				if ((map[i][j] == object_lever))
					map[i][j] = object_lever_activated;
			}
		}
	}
	if (check_near(map, x, y, object_lootchest))
	{
		pot = pot + 1;
		g = g + 30 + rand() % 10;
		for (int i = x - 1; i < x + 2; i++)
		{
			for (int j = y - 1; j < y + 2; j++)
			{
				if (check_near(map, x, y, object_lootchest))
				{
					if (map[i][j] == object_lootchest)
						map[i][j] = terrain_safe;
					else
						continue;
				}
				else
					break;
			}
		}
	}
}
//funkcija koja vraca vrednost stete koju igrac pravi
int player_damage(int dice, int mod, int roll)
{
	int damage = 0;
	do
	{
		damage = damage + (rand() % dice + 1);
		roll--;
	} while (roll > 0);

	damage = damage + mod;
	return damage;
}
//funckija koja pokrece igracev napad
void attack(unsigned char map[LEVEL_HEIGHT][LEVEL_WIDTH], int px, int py, int &zx, int &zy, int &zhp, int &ox, int &oy, int &ohp, int &tx, int &ty, int &thp, int dice, int mod, int roll, int &g)
{
	if (check_near(map, px, py, mob_zombie))
	{
		zhp = zhp - player_damage(dice, mod, roll);
		if (zhp == 0 || zhp < 0)
		{
			map[zx][zy] = terrain_safe;
			zx = NULL;
			zy = NULL;
			g = g + 4 + rand() % 7;
		}
	}
	if (check_near(map, px, py, mob_orc))
	{
		ohp = ohp - player_damage(dice, mod, roll);
		if (ohp == 0 || ohp < 0)
		{
			map[ox][oy] = terrain_safe;
			ox = NULL;
			oy = NULL;
			g = g + 8 + rand() % 7;
		}
	}
	if (check_near(map, px, py, mob_troll))
	{
		thp = thp - player_damage(dice, mod, roll);
		if (thp == 0 || thp < 0)
		{
			map[tx][ty] = terrain_safe;
			tx = NULL;
			ty = NULL;
			g = g + 14 + rand() % 7;
		}
	}
	std::cout << "\n\nUdarili ste za >" << player_damage(dice, mod, roll) << "< poena stete";
	Sleep(800);
}
//funkcija za utvrđivanje da li je igračev potez
bool player_turn(int count)
{
	count = count % 4;

	if (count == 0 || count == 1)
		return true;
	else
		return false;
}

//funkcija koja pokrece wraith/duh neprijatelja
void wraith_ai(unsigned char map[LEVEL_HEIGHT][LEVEL_WIDTH], int &x, int &y, int px, int py, int &hp)
{
	if (check_near(map, x, y, character_player))
	{
		hp = hp - 10; //TRENUTNA STETA KOJU DUH PRAVI IGRACU
	}
	else
	{
		int r = rand() % 4;
		switch (r)
		{
		case 0:
			if (map[x - 1][y] == terrain_safe)
			{
				map[x - 1][y] = mob_wraith;
				map[x][y] = terrain_safe;
				x--;
			}
			break;
		case 1:
			if (map[x + 1][y] == terrain_safe)
			{
				map[x + 1][y] = mob_wraith;
				map[x][y] = terrain_safe;
				x++;
			}
			break;
		case 2:
			if (map[x][y - 1] == terrain_safe)
			{
				map[x][y - 1] = mob_wraith;
				map[x][y] = terrain_safe;
				y--;
			}
			break;
		case 3:
			if (map[x][y + 1] == terrain_safe)
			{
				map[x][y + 1] = mob_wraith;
				map[x][y] = terrain_safe;
				y++;
			}
			break;
		}
	}
}
//funkcija koja pokrece zombie/"zivog mrtvaka" neprijatelja
void zombie_ai(unsigned char map[LEVEL_HEIGHT][LEVEL_WIDTH], int &x, int &y, int &px, int &py, int &hp)
{
	if (check_near(map, x, y, character_player))
	{
		hp = hp - 2; //TRENUTNA STETA KOJU ZOMBI PRAVI IGRACU
	}
	else
	{
		if (abs(x - px) == abs(y - py)) //ako su razdaljine razlika (gore-dole i levo-desno) jednake
		{
			if (py < y) //zombi levo
			{
				if (map[x][y - 1] == terrain_safe)
				{
					map[x][y - 1] = mob_zombie;
					map[x][y] = terrain_safe;
					y--;
				}
			}
			else if (py > y) //zombi desno
			{
				if (map[x][y + 1] == terrain_safe)
				{
					map[x][y + 1] = mob_zombie;
					map[x][y] = terrain_safe;
					y++;
				}
			}

		}
		else  if (((abs(x - px)) < (abs(y - py))) && (py < y)) //zombi levo
		{
			if (map[x][y - 1] == terrain_safe)
			{
				map[x][y - 1] = mob_zombie;
				map[x][y] = terrain_safe;
				y--;
			}
		}
		else if (((abs(x - px)) < abs((y - py))) && (py > y)) //zombi desno
		{
			if (map[x][y + 1] == terrain_safe)
			{
				map[x][y + 1] = mob_zombie;
				map[x][y] = terrain_safe;
				y++;
			}
		}
		else if (((abs(x - px) > (abs(y - py))) && (px < x))) //zombi gore
		{
			if (map[x - 1][y] == terrain_safe)
			{
				map[x - 1][y] = mob_zombie;
				map[x][y] = terrain_safe;
				x--;
			}
		}
		else if (((abs(x - px) >(abs(y - py))) && (px > x))) // zombi dole
		{
			if (map[x + 1][y] == terrain_safe)
			{
				map[x + 1][y] = mob_zombie;
				map[x][y] = terrain_safe;
				x++;
			}

		}
	}
}
//funckija koja pokreće orkove
void orc_ai(unsigned char map[LEVEL_HEIGHT][LEVEL_WIDTH], int &x, int &y, int &px, int &py, int &hp)
{
	if (check_near(map, x, y, character_player))
	{
		hp = hp - 4; //TRENUTNA STETA KOJU ORK PRAVI IGRACU
	}
	else
	{
		if (abs(x - px) < 5 && abs(y - py) < 5) //ork se aktivira ako igrač priđe bliže od 4 kocke
		{
			if (abs(x - px) == abs(y - py)) //ako su razdaljine razlika (gore-dole i levo-desno) jednake
			{
				if (py < y) //ork levo
				{
					if (map[x][y - 1] == terrain_safe)
					{
						map[x][y - 1] = mob_orc;
						map[x][y] = terrain_safe;
						y--;
					}
				}
				else if (py > y) //ork desno
				{
					if (map[x][y + 1] == terrain_safe)
					{
						map[x][y + 1] = mob_orc;
						map[x][y] = terrain_safe;
						y++;
					}
				}

			}
			else  if (((abs(x - px)) < (abs(y - py))) && (py < y)) //ork levo
			{
				if (map[x][y - 1] == terrain_safe)
				{
					map[x][y - 1] = mob_orc;
					map[x][y] = terrain_safe;
					y--;
				}
			}
			else if (((abs(x - px)) < abs((y - py))) && (py > y)) //ork desno
			{
				if (map[x][y + 1] == terrain_safe)
				{
					map[x][y + 1] = mob_orc;
					map[x][y] = terrain_safe;
					y++;
				}
			}
			else if (((abs(x - px) > (abs(y - py))) && (px < x))) //ork gore
			{
				if (map[x - 1][y] == terrain_safe)
				{
					map[x - 1][y] = mob_orc;
					map[x][y] = terrain_safe;
					x--;
				}
			}
			else if (((abs(x - px) >(abs(y - py))) && (px > x))) //ork dole
			{
				if (map[x + 1][y] == terrain_safe)
				{
					map[x + 1][y] = mob_orc;
					map[x][y] = terrain_safe;
					x++;
				}

			}

		}
	}
}
//funkcija koja pokreće trolove
void troll_ai(unsigned char map[LEVEL_HEIGHT][LEVEL_WIDTH], int &x, int &y, int &px, int &py, int&hp)
{
	if (check_near(map, x, y, character_player))
	{
		hp = hp - 6; //TRENUTNA STETA KOJU PRAVI TROL
	}
	{
		int r = rand() % 4;
		if (abs(x - px) < 3 && abs(y - py) < 3) //ako igrač priđe blize od 3 koraka trol se aktivira
		{
			if (abs(x - px) == abs(y - py)) //ako su razdaljine razlika (gore-dole i levo-desno) jednake
			{
				if (py < y) //troll levo
				{
					if (map[x][y - 1] == terrain_safe)
					{
						map[x][y - 1] = mob_troll;
						map[x][y] = terrain_safe;
						y--;
					}
				}
				else if (py > y) //troll desno
				{
					if (map[x][y + 1] == terrain_safe)
					{
						map[x][y + 1] = mob_troll;
						map[x][y] = terrain_safe;
						y++;
					}
				}

			}
			else  if (((abs(x - px)) < (abs(y - py))) && (py < y)) //troll levo
			{
				if (map[x][y - 1] == terrain_safe)
				{
					map[x][y - 1] = mob_troll;
					map[x][y] = terrain_safe;
					y--;
				}
			}
			else if (((abs(x - px)) < abs((y - py))) && (py > y)) //troll desno
			{
				if (map[x][y + 1] == terrain_safe)
				{
					map[x][y + 1] = mob_troll;
					map[x][y] = terrain_safe;
					y++;
				}
			}
			else if (((abs(x - px) > (abs(y - py))) && (px < x))) //troll gore
			{
				if (map[x - 1][y] == terrain_safe)
				{
					map[x - 1][y] = mob_troll;
					map[x][y] = terrain_safe;
					x--;
				}
			}
			else if (((abs(x - px) >(abs(y - py))) && (px > x))) //troll dole
			{
				if (map[x + 1][y] == terrain_safe)
				{
					map[x + 1][y] = mob_troll;
					map[x][y] = terrain_safe;
					x++;
				}

			}

		}
		else
		{
			switch (r)
			{
			case 0:
				if (map[x - 1][y] == terrain_safe)
				{
					map[x - 1][y] = mob_troll;
					map[x][y] = terrain_safe;
					x--;
				}
				break;
			case 1:
				if (map[x + 1][y] == terrain_safe)
				{
					map[x + 1][y] = mob_troll;
					map[x][y] = terrain_safe;
					x++;
				}
				break;
			case 2:
				if (map[x][y - 1] == terrain_safe)
				{
					map[x][y - 1] = mob_troll;
					map[x][y] = terrain_safe;
					y--;
				}
				break;
			case 3:
				if (map[x][y + 1] == terrain_safe)
				{
					map[x][y + 1] = mob_troll;
					map[x][y] = terrain_safe;
					y++;
				}
				break;
			}
		}
	}
}
//funkcija koja ucitava mapu iz fajlova
void load_map(unsigned char map[LEVEL_HEIGHT][LEVEL_WIDTH], FILE* level_file)
{
	for (int i = 0; i < LEVEL_HEIGHT; ++i)
	{
		for (int j = 0; j < LEVEL_WIDTH; ++j)
		{
			unsigned int val;
			fscanf(level_file, "%u", &val);
			if (val <= 255)
				map[i][j] = val;

		}
	}
}

//struktura podataka za oruzija
struct weapon
{
	int dice;
	int modifier;
	int roll_number;
};


int main()
{

	system("mode 150");
	unsigned char  maplogic[LEVEL_HEIGHT][LEVEL_WIDTH];
	int i, j;
	int player_position_x, player_position_y, wraith_position_x, wraith_position_y, zombie_position_x, zombie_position_y;
	int orc_position_x, orc_position_y, troll_position_x, troll_position_y;
	int c, max_health, current_health, zombie_health, orc_health, troll_health, turn_count, potion_number, gold, top_gold;
	int level_number, last_level_number;
	bool next_level; //vrednost koja sluzi da kaze programu da ucita sledeci fajl
	int dice, modifier, roll_number;
	srand(time(NULL));


	//definisanje vrednosti oruzija
	weapon* weapons[3];
	for (i = 0; i < 3; i++)
		weapons[i] = (weapon*)malloc(sizeof(weapon)); // dinamicka alokacija memorije

	weapon &axe1 = *weapons[0];
	axe1.dice = 12;
	axe1.modifier = 2;
	axe1.roll_number = 1;
	weapon &sword1 = *weapons[1];
	sword1.dice = 6;
	sword1.modifier = 1;
	sword1.roll_number = 2;
	weapon &mace1 = *weapons[2];
	mace1.dice = 4;
	mace1.modifier = 1;
	mace1.roll_number = 3;

	// ucitavanje highscore vrednosti iz fajla
	FILE* highscore_file = fopen("highscore.txt", "r");
	if (highscore_file)
	{
		fscanf(highscore_file, "%d", &top_gold);

		fclose(highscore_file);
	}
	else
	{
		top_gold = 0;
	}

new_game: // labela za pocinjanje nove igre (nadam se da cu smisliti pametniji nacin)

	std::cout << "\n\t\tIZABERITE POCETNO ORUZIJE TAKO STO CE TE KLIKNUTI NA TASTER (1,2 ili 3)" << endl << "\n\t\tSteta koju oruzije nanosi se racuna na sledeci nacin:";
	std::cout << "\t steta=(nd+m)\n\n\t\t - d predstavlja velicinu kockice (d6 je obicna kockica sa 6 strana, d12 kocka sa 12 strana itd.) \n\t\t - n je broj bacanja kocke (rezultat svih bacanja se sabira)";
	std::cout << " \n\t\t - k je modifikator koji se samo dodaje na zbir svih bacanja kockica" << endl;
	std::cout << "\n\t\t1 - Bojna sekira:\t Dzelatova Nevesta (1d12+2)" << "\n\t\t2 - Mac:\t\t Bleda Pravda (2d6+1)" << "\n\t\t3 - Buzdovan:\t\t Usudov Glasnik  (3d4+1)";

	//biranje pocetnog oruzija
	do
	{
		switch (c = _getch())
		{
		case '1':
			dice = axe1.dice;
			modifier = axe1.modifier;
			roll_number = axe1.roll_number;
			break;
		case '2':
			dice = sword1.dice;
			modifier = sword1.modifier;
			roll_number = sword1.roll_number;
			break;
		case '3':
			dice = mace1.dice;
			modifier = mace1.modifier;
			roll_number = mace1.roll_number;
			break;
		default:
			break;
		}
	} while (c != '1' && c != '2' && c != '3');

	//oslobadjanje alocirane memorije kada je struct iskoriscen
	for (int i = 0; i < 3; i++)
	{
		free(weapons[i]);
		weapons[i] = NULL;
	}

	system("CLS");

	gold = 0;
	turn_count = 0;
	potion_number = 1;
	max_health = 60;
	current_health = 60;
	zombie_health = 40;
	orc_health = 50;
	troll_health = 80;

	level_number = 1;
	last_level_number = 3;
	next_level = false;

	//deo koda koji odredjuje koji fajl se ucitava
	char level_name[32];
	sprintf(level_name, "nivo%d.txt", level_number);

	//ucitavanje mape iz fajla
	FILE* myFile = fopen(level_name, "r");
	if (myFile) // radi samo ako se fajl otvorio
	{
		load_map(maplogic, myFile);
		fclose(myFile);
	}
	else
	{
		std::cout << endl << "GRESKA: NEDOSTAJE FAJL SA MAPOM";
		Sleep(1500);
		exit(1);
	}
	//ucitavanje koordinata likova u igru
	character_load(maplogic, player_position_x, player_position_y, zombie_position_x, zombie_position_y, orc_position_x, orc_position_y, troll_position_x, troll_position_y, wraith_position_x, wraith_position_y);

	draw_map(maplogic, current_health, max_health, potion_number, zombie_health, orc_health, troll_health, gold, top_gold);
	std::cout << endl << "\n\n\t\tCilj igre je da predjete 3 nivoa tako sto cete stici do ',' karaktera koji predstavljaju izlaz sa nivoa." << endl;
	std::wcout << "\t\tIgra je potezna, i po potezu imate mogucnost da iskoristite 2 akcije (2 pritiska na taster). \n\t\tKada iskoristite sve akcije u potezu, cudovista imaju svoj potez." << endl;
	std::cout << "\t\tU kovcezima i kod cudovista se nalazi zlato, pokusajte da skupite vise zlata nego prosli igrac!";

	while (current_health > 0)
	{
		//ispituje da li je potez igračev;
		if (player_turn(turn_count))
		{
			switch ((c = _getch()))
			{
			case KEY_UP: //akcija za strelicu gore
				move_up(maplogic, player_position_x, player_position_y, current_health, next_level);
				turn_count++;
				break;
			case KEY_DOWN: //akcija za strelicu dole
				move_down(maplogic, player_position_x, player_position_y, current_health, next_level);
				turn_count++;
				break;
			case KEY_LEFT: //akcija za strelicu levo
				move_left(maplogic, player_position_x, player_position_y, current_health, next_level);
				turn_count++;
				break;
			case KEY_RIGHT: //akcija za strelicu desno
				move_right(maplogic, player_position_x, player_position_y, current_health, next_level);
				turn_count++;
				break;
			case KEY_USE: //akcija za taster za korišćenje "e"
				use(maplogic, player_position_x, player_position_y, potion_number, gold);
				turn_count++;
				break;
			case KEY_ATTACK: //akcija za taster za napad "a"
				attack(maplogic, player_position_x, player_position_y, zombie_position_x, zombie_position_y, zombie_health, orc_position_x, orc_position_y, orc_health, troll_position_x, troll_position_y, troll_health, dice, modifier, roll_number, gold);
				turn_count++;
				break;
			case KEY_DRINK: //akcija za pijenje napitaka taster "d"
				drink(current_health, max_health, potion_number);
				break;
			default: //sve ostalo
				break;
			}

			if (next_level)
			{
				next_level = false; //vraca bool vrednost na false da ne bi ucitavao sledeci nivo kad ne treba

				if (level_number == last_level_number) //kada igrac izadje iz poslednjeg nivoa i pobedi
				{
					// deo koda koji dozvoljava igracu da izabere da li hoce da ugasi igru ili da igra ponovo
					gold = gold + 50;
					if (gold > top_gold)
					{
						top_gold = gold;

						// pise trenutni highscore u fajl
						FILE* highscore_file = fopen("highscore.txt", "w");
						if (highscore_file)
						{
							fprintf(highscore_file, "%d", top_gold);

							fclose(highscore_file);
						}
						else
						{
							std::cout << "File highsore.txt couldn't be open for writing!\n";
						}
					}
					system("CLS");
					std::cout << "KRAJ IGRE, cestitamo pobedili ste! " << "Skupili ste " << gold << " zlatnika, a highscore je: " << top_gold << "zlatnika" << endl;
					std::cout << endl << "Da li zelite da igrate ponovo? (d/n)" << endl;
					do
					{

						switch ((c = _getch()))
						{
						case 'd':
							system("CLS");
							goto new_game; //u slucaju da igrac zeli opet da igra, skace na labelu new_game
						case 'n':
							system("CLS");
							std::cout << "\n\n\t\tHvala Vam na igranju" << endl;
							Sleep(1500);
							exit(1);
						}

					} while (c != 'd' && c != 'n');
				}

				turn_count--;
				level_number++;

				sprintf(level_name, "nivo%d.txt", level_number);

				//ucitavanje mape iz fajla
				FILE* myFile = fopen(level_name, "r");
				if (myFile) // radi samo ako se fajl otvorio
				{
					load_map(maplogic, myFile);
					fclose(myFile);
				}
				else
				{
					std::cout << endl << "GRESKA: NEDOSTAJE FAJL SA MAPOM";
					Sleep(1500);
					exit(1);
				}
				//ucitavanje koordinata likova u igru
				character_load(maplogic, player_position_x, player_position_y, zombie_position_x, zombie_position_y, orc_position_x, orc_position_y, troll_position_x, troll_position_y, wraith_position_x, wraith_position_y);

				zombie_health = 40; //vracanje vrednosti zivota cudovista na pocetne vrednosti
				orc_health = 50;
				troll_health = 80;
			}
		}
		else
		{
			//U OVOM "ELSE" SE IZVRŠAVAJU POTEZI OD AI NEPRIJATELJA
			//Sleep se koristi da bi igrač mogao videti poteze kompjutera
			Sleep(400);
			wraith_ai(maplogic, wraith_position_x, wraith_position_y, player_position_x, player_position_y, current_health);
			zombie_ai(maplogic, zombie_position_x, zombie_position_y, player_position_x, player_position_y, current_health);
			orc_ai(maplogic, orc_position_x, orc_position_y, player_position_x, player_position_y, current_health);
			troll_ai(maplogic, troll_position_x, troll_position_y, player_position_x, player_position_y, current_health);
			FlushConsoleInputBuffer(GetStdHandle(STD_INPUT_HANDLE)); //izbacuje sve karaktere viska koje je korisnik uneo tokom svog poteza
			turn_count++;
		}
		system("CLS");
		draw_map(maplogic, current_health, max_health, potion_number, zombie_health, orc_health, troll_health, gold, top_gold);
		if (player_turn(turn_count))
		{
			std::cout << "\n\tPotez: " << (turn_count / 4) + 1 << "\tIskoricene Akcije: " << turn_count % 4 << "/2" <<"\t\tIgracev potez" << endl;
		}
		else
		{
			std::cout << "\n\tPotez: " << (turn_count / 4) + 1 << "\t\tRacunarev potez"<< endl;
		}
	}

	if (current_health < 0 || current_health == 0) //kada igrac umre
	{
		if (gold > top_gold)
		{
			top_gold = gold;

			// pise trenutni highscore u fajl
			FILE* highscore_file = fopen("highscore.txt", "w");
			if (highscore_file)
			{
				fprintf(highscore_file, "%d", top_gold);

				fclose(highscore_file);
			}
			else
			{
				std::cout << "File highsore.txt couldn't be open for writing!\n";
			}
		}
		system("CLS");
		std::cout << "KRAJ IGRE, nazalost ste izgubili, vise srece sledeci put" << "Skupili ste " << gold << " zlatnika, a highscore je: " << top_gold << " zlatnika" << endl;
		std::cout << endl << "Da li zelite da igrate ponovo? (d/n)" << endl;
		do
		{
			switch ((c = _getch()))
			{
			case 'd':
				system("CLS");
				goto new_game; //u slucaju da igrac zeli opet da igra, skace na labelu new_game
				break;
			case 'n':
				system("CLS");
				std::cout << "\n\n\t\tHvala Vam na igranju" << endl;
				Sleep(1500);
				break;
			}

		} while (c != 'd' && c != 'n');
	}


	return 0;
}
