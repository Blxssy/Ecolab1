/*
 * <кодировка символов>
 *   Cyrillic (UTF-8 with signature) - Codepage 65001
 * </кодировка символов>
 *
 * <сводка>
 *   EcoLab1 — Bubble Sort 
 * </сводка>
 *
 * <описание>
 *   Генерирует массивы, сортирует их:
 *   - COM: bubble sort
 *   - stdlib: qsort
 *   Печатает таблицу и ждёт ввод в конце.
 * </описание>
 */

/* Eco OS */
#include "IEcoSystem1.h"
#include "IdEcoMemoryManager1.h"
#include "IdEcoInterfaceBus1.h"
#include "IdEcoFileSystemManagement1.h"
#include "IdEcoLab1.h"

#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <stdio.h>
#include <stdint.h>

#define START_EXP 6   
#define END_EXP  16   

#ifdef _WIN32
#include <conio.h> 
#endif

static int __cdecl compare(const void* a, const void* b) {
    int32_t va = *(const int32_t*)a;
    int32_t vb = *(const int32_t*)b;
    return (va > vb) - (va < vb);
}

int16_t EcoMain(IEcoUnknown* pIUnk) {
    int16_t result = -1;

    IEcoSystem1*          pISys     = 0;
    IEcoInterfaceBus1*    pIBus     = 0;
    IEcoMemoryAllocator1* pIMem     = 0;
    IEcoLab1*             pIEcoLab1 = 0;

    int32_t* aCOM = 0;
    int32_t* aSTD = 0;
    uint32_t n = 0, i = 0, exp = 0;

    clock_t t0 = 0, t1 = 0;
    double msCOM = 0.0, msSTD = 0.0;

    srand((unsigned int)time(NULL));

    result = pIUnk->pVTbl->QueryInterface(pIUnk, &GID_IEcoSystem, (void**)&pISys);
    if (result != 0 || pISys == 0) { goto Release; }

    result = pISys->pVTbl->QueryInterface(pISys, &IID_IEcoInterfaceBus1, (void**)&pIBus);
    if (result != 0 || pIBus == 0) { goto Release; }

#ifdef ECO_LIB
    result = pIBus->pVTbl->RegisterComponent(
        pIBus, &CID_EcoLab1, (IEcoUnknown*)GetIEcoComponentFactoryPtr_1F5DF16EE1BF43B999A434ED38FE8F3A);
    if (result != 0) { goto Release; }
#endif

    result = pIBus->pVTbl->QueryComponent(pIBus, &CID_EcoMemoryManager1, 0,
                                          &IID_IEcoMemoryAllocator1, (void**)&pIMem);
    if (result != 0 || pIMem == 0) { goto Release; }

    result = pIBus->pVTbl->QueryComponent(pIBus, &CID_EcoLab1, 0, &IID_IEcoLab1, (void**)&pIEcoLab1);
    if (result != 0 || pIEcoLab1 == 0) { goto Release; }

    printf("==========================================\n");
    printf("%-12s | %-10s | %-10s | %-8s | %-7s\n",
           "Array Size", "COM (ms)", "qsort (ms)", "Faster", "Ratio");
    printf("-------------+------------+------------+----------+---------\n");

    for (exp = START_EXP; exp <= END_EXP; ++exp) {
        n = (uint32_t)pow(2.0, (double)exp);

        aCOM = (int32_t*)pIMem->pVTbl->Alloc(pIMem, n * sizeof(int32_t));
        aSTD = (int32_t*)pIMem->pVTbl->Alloc(pIMem, n * sizeof(int32_t));
        if (aCOM == 0 || aSTD == 0) { goto Release; }

        for (i = 0; i < n; ++i) {
            int32_t rnd = (int32_t)(rand() % 100000) - 50000;
            aCOM[i] = rnd;
            aSTD[i] = rnd;
        }

        t0 = clock();
        result = pIEcoLab1->pVTbl->MyFunction(pIEcoLab1, aCOM, n);
        t1 = clock();
        if (result != 0) {
            pIMem->pVTbl->Free(pIMem, aCOM);
            pIMem->pVTbl->Free(pIMem, aSTD);
            goto Release;
        }
        msCOM = ((double)(t1 - t0) / (double)CLOCKS_PER_SEC) * 1000.0;


        for (i = 0; i + 1 < n; ++i) {
            if (aCOM[i] > aCOM[i + 1]) {
                printf("ERROR: COM not sorted at size=%u\n", n);
                pIMem->pVTbl->Free(pIMem, aCOM);
                pIMem->pVTbl->Free(pIMem, aSTD);
                goto Release;
            }
        }

        t0 = clock();
        qsort(aSTD, n, sizeof(int32_t), compare);
        t1 = clock();
        msSTD = ((double)(t1 - t0) / (double)CLOCKS_PER_SEC) * 1000.0;

        printf("%-12u | %-10.3f | %-10.3f | ",
               n, msCOM, msSTD);

        if (msCOM == 0.0 && msSTD == 0.0) {
            printf("%-8s | %-7s\n", "equal", "1.00x");
        }
        else if (msCOM == 0.0 && msSTD > 0.0) {
            printf("%-8s | %-7s\n", "COM", ">1000x");
        }
        else if (msSTD == 0.0 && msCOM > 0.0) {
            printf("%-8s | %-7s\n", "qsort", ">1000x");
        }
        else {
            if (msCOM < msSTD) {
                double ratio = msSTD / msCOM;
                printf("%-8s | %-7.2fx\n", "COM", ratio);
            }
            else if (msCOM > msSTD) {
                double ratio = msCOM / msSTD;
                printf("%-8s | %-7.2fx\n", "qsort", ratio);
            }
            else {
                printf("%-8s | %-7s\n", "equal", "1.00x");
            }
        }

        pIMem->pVTbl->Free(pIMem, aCOM);
        pIMem->pVTbl->Free(pIMem, aSTD);
        aCOM = 0; aSTD = 0;
    }

    printf("==========================================\n");
    result = 0;

Release:
    if (aCOM) { pIMem->pVTbl->Free(pIMem, aCOM); }
    if (aSTD) { pIMem->pVTbl->Free(pIMem, aSTD); }
    if (pIBus) { pIBus->pVTbl->Release(pIBus); }
    if (pIMem) { pIMem->pVTbl->Release(pIMem); }
    if (pIEcoLab1) { pIEcoLab1->pVTbl->Release(pIEcoLab1); }
    if (pISys) { pISys->pVTbl->Release(pISys); }

    printf("\nPress any key to close...");
    fflush(stdout);
#ifdef _WIN32
    (void)_getch();
#else
    (void)getchar();
#endif

    return result;
}
