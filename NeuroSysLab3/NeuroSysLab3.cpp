////////////////////////////////////////////////////////////////
//         Перцептрон. Распознавание 6-х образов:             //
//     распознавание цифр "3", "4", "5", "6", "7", "8"        //
////////////////////////////////////////////////////////////////
#include <iostream>
#include <vector>
#include <cmath>
using namespace std;

const int Teta = 4; // порог
int lmax = 10000; // число шагов обучения
int threshold = 0; // порог обучения
const double DeltaW = 0.3; // вес обучения

const int imN = 6; // количество образов
const int elN = 3; // количество эффекторных клеток
const int N = 20; // размерность клеток

struct LC { // координаты
	int L; // строка
	int C; // столбец
};

int R[N][N]; // слой рецепторов
int A[N][N]; // ассоциативный слой
LC S[N][N][N]; // связи
double W[N][N][elN]; // веса связей
bool fTest = false; // флаг проверки работоспособности


void InitLayers() // инициализация слоѐв
{
	// генерация связей
	for (int L = 0; L < N; L++)
		for (int C = 0; C < N; C++) {
			for (int K = 0; K < N; K++) {
				S[L][C][K].L = rand() % N;
				S[L][C][K].C = rand() % N;
			}
		}
	// очистка весов
	for (int K = 0; K < elN; K++)
		for (int L = 0; L < N; L++)
			for (int C = 0; C < N; C++) W[L][C][K] = 0;
};

string NameDic(string dic) // имя образа: нолик,крестик,плюс,ромб
{
	string res = "не знаю";
	if (dic == "000") res = "3";
	if (dic == "001") res = "4";
	if (dic == "010") res = "5";
	if (dic == "011") res = "6";
	if (dic == "100") res = "7";
	if (dic == "101") res = "8";
	return res;
}

string Obraz(int H) // создание образа по шагу H
{
	for (int L = 0; L < N; L++) // чистка рецепторов
		for (int C = 0; C < N; C++) R[L][C] = 0;
	int sd = H % 6; // выбор образа: 0-3, 1-4, 2-5, 3-6, 4-7, 5-8
	int rr = rand() % (N / 2) - 1; // выбор радиуса образа
	if (rr < 5) rr = 9;
	std::string res = "000"; // код образа
	// выбор места расположения образа
	int Lc = rr + (rand() % int(N - 2 * rr)); // строка центра
	int Cc = rr + (rand() % int(N - 2 * rr)); // столбец центра

	// рисование выбранного образа
	switch (sd) {
	case 0: // рисование нолика
	{
		res = "000"; // код 3
		int gap = (rand() % (N / 2 - 4)) + 1; // промежуток между тремя горизонтальными линиями
		int w = (rand() % rr) + 4; // ширина буквы
		// отрисовка трех горизонтальных линий
		for (size_t i = 0; i < w; i++) {
			R[Lc - rr][Cc - rr / 2 + i] = 1;
			R[Lc][Cc - rr / 2 + i] = 1;
			R[Lc + rr][Cc - rr / 2 + i] = 1;
		}
		// отрисовка вертикальной линии
		for (size_t i = 0; i < rr * 2 + 1; i++) {
			R[Lc - rr + i][Cc - rr / 2 + w] = 1;
		}
		break;
	}
	case 1: // рисование 4
	{
		res = "001"; // код 4
		rr -= 1;
		for (size_t i = 0; i < rr; i++) {
			int L = Lc - rr + i;
			int C = Cc - rr;
			R[L][C] = 1;
			L = Lc - rr + i;
			C = Cc + rr;
			R[L][C] = 1;
			L = Lc + i;
			C = Cc + rr;
			R[L][C] = 1;
			L = Lc;
			C = Cc - rr + i;
			R[L][C] = 1;
			L = Lc;
			C = Cc + rr - i;
			R[L][C] = 1;
		}
		R[Lc][Cc] = 1;
		R[Lc + rr][Cc + rr] = 1;
		break;
	}
	case 2: // рисование 5
		res = "010"; // код 5
		// отрисовка горизонтальных линий
		for (size_t i = 0; i < 2 * rr; i++) {
			int L = Lc - rr;
			int C = Cc - rr + i;
			R[L][C] = 1;
			L = Lc;
			C = Cc - rr + i;
			R[L][C] = 1;
			L = Lc + rr;
			C = Cc - rr + i;
			R[L][C] = 1;
		}
		// отрисовка вертикальныйх линий
		for (size_t i = 0; i < rr; i++) {
			int L = Lc - rr + i;
			int C = Cc - rr;
			R[L][C] = 1;
			L = Lc + i;
			C = Cc + rr;
			R[L][C] = 1;
		}
		R[Lc][Cc] = 1;
		R[Lc + rr][Cc + rr] = 1;
		R[Lc - rr][Cc + rr] = 1;
		break;
	case 3: // рисование 6
		res = "011"; // код 6
		// отрисовка горизонтальных линий
		for (size_t i = 0; i < 2 * rr; i++) {
			int L = Lc - rr;
			int C = Cc - rr + i;
			R[L][C] = 1;
			L = Lc;
			C = Cc - rr + i;
			R[L][C] = 1;
			L = Lc + rr;
			C = Cc - rr + i;
			R[L][C] = 1;
			L = Lc - rr + i;
			C = Cc - rr;
			R[L][C] = 1;
		}
		// отрисовка вертикальныйх линий
		for (size_t i = 0; i < rr; i++) {
			int L = Lc + i;
			int C = Cc + rr;
			R[L][C] = 1;
		}
		R[Lc][Cc] = 1;
		R[Lc + rr][Cc + rr] = 1;
		R[Lc - rr][Cc + rr] = 1;
		break;
	case 4: // рисование 7
		res = "100"; // код 7
		for (size_t i = 0; i < rr; i++) {
			int L = Lc - rr;
			int C = Cc + i;
			R[L][C] = 1;
			L = Lc - rr + i;
			C = Cc + rr;
			R[L][C] = 1;
			L = Lc + i;
			C = Cc + rr;
			R[L][C] = 1;
		}
		R[Lc + rr][Cc + rr] = 1;
		break;
	case 5: // рисование 8
		res = "101"; // код 8
		// отрисовка горизонтальных линий
		for (size_t i = 0; i < 2 * rr; i++) {
			int L = Lc - rr;
			int C = Cc - rr + i;
			R[L][C] = 1;
			L = Lc;
			C = Cc - rr + i;
			R[L][C] = 1;
			L = Lc + rr;
			C = Cc - rr + i;
			R[L][C] = 1;
			L = Lc - rr + i;
			C = Cc - rr;
			R[L][C] = 1;
			L = Lc - rr + i;
			C = Cc + rr;
			R[L][C] = 1;
		}
		R[Lc][Cc] = 1;
		R[Lc + rr][Cc + rr] = 1;
		break;
	}

	// вывод образа на экран во время проверки работы
	if (fTest) {
		cout << "Рисую " + NameDic(res) + "\n";
		for (int L = 0; L < N; L++) {
			for (int C = 0; C < N; C++)
				if (R[L][C] == 0) cout << "_"; else cout << "*";
			cout << "\n";
		}
	};
	return res;
}

void Otobr() // отображение образа
{
	for (int L = 0; L < N; L++) // очистка ассоциативного слоя A
		for (int C = 0; C < N; C++) A[L][C] = 0;
	for (int L = 0; L < N; L++) // отображение в слое A (входы)
		for (int C = 0; C < N; C++)
			if (R[L][C] == 1)
				for (int K = 0; K < N; K++) {
					int Lv = S[L][C][K].L;
					int Cv = S[L][C][K].C;
					A[Lv][Cv]++;
				}
	for (int L = 0; L < N; L++) // отображение в слое A (выходы)
		for (int C = 0; C < N; C++)
			if (A[L][C] > Teta) A[L][C] = 1; else A[L][C] = 0;
}

string Reak() // распознавание образа: 000..110
{
	double E[elN]; // эффекторный слой
	string res = ""; // код распознавания
	for (int K = 0; K < elN; K++) {
		E[K] = 0;
		for (int L = 0; L < N; L++)
			for (int C = 0; C < N; C++)
				E[K] += A[L][C] * W[L][C][K];
		if (E[K] > threshold) res += '1'; else res += '0';
	}

	//bool fNumOneAppeared = false;
	//for (size_t i = 0; i < res.length(); i++) {
	//	if (!fNumOneAppeared && res[i] == '1') {
	//		fNumOneAppeared = true;
	//		continue;
	//	}
	//	if (fNumOneAppeared && res[i] == '1') {
	//		res[i] = '0';
	//	}
	//}
	
	// вывод результата при проверке работоспособности
	if (fTest) {
		cout << "\n" << "Я думаю что это " + NameDic(res) << "\n";
		system("pause");
	}
	return res;
}

void Teach(string sd, string sd1) // обучение нейросети
{
	for (int K = 0; K < elN; K++) {
		if (sd[K] != sd1[K]) {
			for (int L = 0; L < N; L++) {
				for (int C = 0; C < N; C++) {
					if (A[L][C] == 1) { // обучение виноватых
						if (sd[K] == '0') W[L][C][K] -= DeltaW;
						else W[L][C][K] += DeltaW;
					}
				}
			}
		}
	}
}

int main() {
	//setlocale(LC_ALL, "Russian");
	//srand((unsigned)time(NULL));
	cout << "Обучение перцептрона распознаванию шести образов: 3, 4, 5, 6, 7, 8\n";
	int nOk = 0; // число удачных ответов
	InitLayers(); // инициализация слоѐв
	for (int Step = 1; Step <= lmax; Step++) {
		string dic = Obraz(Step); // новый образ
		Otobr(); // отображение
		string dic1 = Reak(); // опознавание
		if (dic == dic1) nOk++;
		else Teach(dic, dic1); // обучение
		// вывод текущей информации на экран
		cout << "Шаг " << Step << " : Доля удачных ответов "
			<< nOk / double(Step) * 100 << " %\n";
		// тестирование за 20 шагов до конца обучения
		if (Step == (lmax - 20)) fTest = true;
	};
	cout << "Конец\n";
}