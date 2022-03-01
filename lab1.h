#ifndef LAB1_H_INCLUDED
#define LAB1_H_INCLUDED

#include <stdio.h>
#include <pthread.h>
#include <pthread.h>


// ************************* Константы ************************************

// Продолжительность жизни (измеряется в количествах сделанных переходов)
const int kLifeTime = 10;    

// Продолжительность голодания (измеряется в количествах сделанных переходов)
const int kStarvationTime = 5;  

// Размеры карты 
const unsigned int kMapSizeX = 8;
const unsigned int kMapSizeY = 8;

// ************************************************************************


// **************************** Описание объектов *************************

// Виды животных
typedef enum {ANIMAL_1, ANIMAL_2, ANIMAL_3} TypeAnimal;

// Передвижение
typedef enum {RIGHT, LEFT, UP, DOWN} Direction;

// Координаты
typedef struct {
    int x;
    int y;
} Coordinate;

// Атрибуты животного
typedef struct {
    Coordinate coord;
    TypeAnimal type;
    int life_time;
    int startvation_time;
} AnimalAttributes;

// Атрибуты карты
typedef struct {
    pthread_t thread_id;
    TypeAnimal type;
    int dead; 
} MapAttributes;

// ************************************************************************


// *************************** Глобальные перменные ***********************

// Поле по которым перемещаются животные
MapAttributes** map;

// Мьютекс
pthread_mutex_t** mutexes;

// Указатель на файл
FILE *fp;

// ************************************************************************


// *************************** Прототипы функций **************************

void* Animal(void* atr);
void CreateThreads(pthread_t* tr,int count, TypeAnimal type);
int get_rand_range_int(int min, int max);
pthread_mutex_t** CreateArrayMutexes(unsigned int row, unsigned int column);
void DeleteArrayMutexes(pthread_mutex_t** array_mutexes, unsigned int row, unsigned int column);

// ************************************************************************


#endif /* LAB1_H_INCLUDED */