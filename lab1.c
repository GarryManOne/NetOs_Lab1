#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <malloc.h>
#include "lab1.h"

// Генерация псевдослучайных чисел на определнном промежутке
int GetRandRangeInt(int min, int max){
    return rand() % (max - min) + min;
}

// // Функция работающая в отдельном потоке
void* Animal(void* atr){

    int* index = (int*) atr;
    int n = 0;

    printf("%u-%d\n", pthread_self(),*index);

    while (1){
        // Смотрим продолжительность жизни 
        if (db_animals[*index].life_time == 0 || db_animals[*index].startvation_time == 0){
            pthread_exit(NULL);
        }

        int x = db_animals[*index].coord.x;
        int y = db_animals[*index].coord.y;

        // Случайное передвижение
        switch (GetRandRangeInt(0, 4)){
            case RIGHT:
                if ((x + 1) < kMapSizeX){
                    x += 1;
                }
                else continue;
                break;
            case LEFT: 
                if ((x - 1) > 0){
                    x -= 1;
                }
                else continue;
                break;
            case UP: 
                if ((y - 1) > 0){
                    y -= 1;
                }
                else continue;
                break;
            case DOWN: 
                if ((y + 1) < kMapSizeY){
                    y += 1;
                }
                else continue;
                break;
            default:
                break;
        }

        pthread_mutex_lock(&mutex);

        // Смотрим продолжительность жизни 
        if (db_animals[*index].life_time == 0){
            pthread_mutex_unlock(&mutex);
            pthread_exit(NULL);
        }

        printf("%u шаг=%d \n", pthread_self(), n++);

        // Проверка какое животное находится в этой ячейке
        if(map[x][y] != 17){
            
            // Спаривание :)
            if (db_animals[*index].type == db_animals[map[x][y]].type){
                // Смотрим продолжительность жизни 
                if (db_animals[*index].life_time == 0){
                    pthread_mutex_unlock(&mutex);
                    pthread_exit(NULL);
                }
                CreateThread(GetRandRangeInt(0, kMapSizeX), GetRandRangeInt(0, kMapSizeY), db_animals[*index].type);
                pthread_mutex_unlock(&mutex);
            }
            // Ест
            else if ((db_animals[*index].type + 1 ) % 3 == db_animals[map[x][y]].type){
                // Смотрим продолжительность жизни 
                if (db_animals[*index].life_time == 0){
                    pthread_mutex_unlock(&mutex);
                    pthread_exit(NULL);
                }

                db_animals[map[x][y]].life_time = 0;

                map[db_animals[*index].coord.x][db_animals[*index].coord.y] = 17;


                db_animals[*index].coord.x = x;
                db_animals[*index].coord.y = y;
                db_animals[*index].startvation_time = kStarvationTime;
                
                map[x][y] = *index;

                PrintMap();

                pthread_mutex_unlock(&mutex);
            }
            // Его едят
            else{
                // Смотрим продолжительность жизни 
                if (db_animals[*index].life_time == 0){
                    pthread_mutex_unlock(&mutex);
                    pthread_exit(NULL);
                }

                map[db_animals[*index].coord.x][db_animals[*index].coord.y] = 17;

                PrintMap();

                pthread_mutex_unlock(&mutex);
                pthread_exit(NULL);
            }
        }
        else
        {
            // Смотрим продолжительность жизни 
            if (db_animals[*index].life_time == 0){
                pthread_mutex_unlock(&mutex);
                pthread_exit(NULL);
            }

            // Обнуление прошлой ячейки
            map[db_animals[*index].coord.x][db_animals[*index].coord.y] = 17;
            
            // Переход
            db_animals[*index].coord.x = x;
            db_animals[*index].coord.y = y;
            db_animals[*index].life_time -= 1;
            db_animals[*index].startvation_time -= 1;   
            map[x][y] = *index;

            PrintMap();
            pthread_mutex_unlock(&mutex);
        }

        // Смотрим продолжительность жизни 
        if (db_animals[*index].life_time == 0){
            pthread_exit(NULL);
        }
        usleep(50000);

    }
    
    return NULL;
}

// Создание потока
void CreateThread(int row, int column, TypeAnimal type){

    pthread_t* animal_id = (pthread_t*)(malloc(sizeof(pthread_t)));
    
    for(int i = 0; i < kMapSizeX*kMapSizeY; i++){
        if (db_animals[i].type == NONE){

            db_animals[i].type = type;              
            db_animals[i].coord.x = row;
            db_animals[i].coord.y = column;
            db_animals[i].life_time = kLifeTime;
            db_animals[i].startvation_time = kStarvationTime;

            map[row][column] = i;
            
            int* index = (int*)malloc(sizeof(int));
            *index = i;

            pthread_create(animal_id, NULL, &Animal, index);
            break;
        }
    }
}

// // Вывод карты в консоль
// void* PrintMap(void* arg){
//     while (1)
//     {
//         pthread_mutex_lock(&mutex);

//             for (int i = 0; i < kMapSizeX; i++){
//                 for (int j = 0; j < kMapSizeY; j++){
//                     if (map[i][j] == 17){
//                         fprintf(log_file, "[*]");
//                     }
//                     else{
//                         fprintf(log_file, "[%d]", db_animals[map[i][j]].type);
//                         // printf("[%d]", db_animals[map[i][j]].type);
//                     }                }
//                 fprintf(log_file, "\n");
//                 // printf("\n");
//             }
//             fprintf(log_file, "\n");
//             // printf("\n");

//         pthread_mutex_unlock(&mutex);
//         // usleep(100000);
//     }
// }

// Вывод карты в консоль
void PrintMap(void){
            for (int i = 0; i < kMapSizeX; i++){
                for (int j = 0; j < kMapSizeY; j++){
                    if (map[i][j] == 17){
                        // fprintf(log_file, "[*]");
                        printf("[*]");
                    }
                    else{
                        // fprintf(log_file, "[%d]", db_animals[map[i][j]].type);
                        printf("[%d]", db_animals[map[i][j]].type);
                    }                }
                // fprintf(log_file, "\n");
                printf("\n");
            }
            // fprintf(log_file, "\n");
            printf("\n");
}

// Открытие файла для записи
FILE* OpenFile(char* fileName){
    FILE* file;
    if ((file = fopen(fileName, "w")) == NULL){
        printf("Не удалось открыть файл");
        getchar();
    }

    return file;
}                               

// Главная функция
int main(int argc, char *argv[]){
    
    srand(time(NULL));
    setbuf(stdout, 0);

    // Проверка на количество параметров
    if (argc != 4){
        printf("Неверное количество параметров!\n");
        return 1;
    }

    // Вывод информационных данных в консоль
    int animal1Count = atoi(argv[1]);
    printf("TypeAnimal 1 type count: %d\n", animal1Count);

    int animal2Count = atoi(argv[2]);
    printf("TypeAnimal 2 type count: %d\n", animal2Count);

    int animal3Count = atoi(argv[3]);
    printf("TypeAnimal 3 type count: %d\n",animal3Count);

    // Проверка на максимальное количество потоков которые возможно создать
    int allAnimalsCount = animal1Count + animal2Count + animal3Count;
    if (allAnimalsCount > kMapSizeX * kMapSizeY){
        printf("Превышенно количество животных! Максимум: %d\n", kMapSizeX * kMapSizeY);
        return 1;
    }

    // Открытие файла
    log_file = OpenFile((char*)"log.txt");


    // Обнуление карты 
    for (int i = 0; i < kMapSizeX; i++){
        for (int j = 0; j < kMapSizeY; j++){
            map[i][j] = 17;
        }
    }

    for (int i = 0; i < kMapSizeX*kMapSizeY; i++){
        db_animals[i].type = NONE;
    }
    
    // Создание и инициализация мьютексов
    pthread_mutex_init(&mutex, NULL);    

    // pthread_t log;
    // pthread_create(&log, NULL, PrintMap, NULL);

    pthread_mutex_lock(&mutex);

    // Создание потоков
    CreateThread(0, 0, ANIMAL_1);
    CreateThread(1, 3, ANIMAL_2);
    CreateThread(3, 2, ANIMAL_3);
    CreateThread(4, 0, ANIMAL_2);

    pthread_mutex_unlock(&mutex);

    // pthread_join(log, NULL);
    sleep(10000);
    // Освобождение памяти
    pthread_mutex_destroy(&mutex);

    return 0;
}