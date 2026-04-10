#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>

typedef struct {
    long long inicio;
    long long fim;
    long long qtd_primos;
} Faixa;

long long N;

// int ehPrimo(long long n) {
//     if (n <= 1) return 0;
//     if (n == 2) return 1;
//     if (n % 2 == 0) return 0;

//     long long limite = (long long) sqrt((double)n);

//     for (long long i = 3; i <= limite; i += 2) {
//         if (n % i == 0) return 0;
//     }

//     return 1;
// }

int ehPrimo(long long n) {
    if (n <= 1) return 0;
    if (n == 2 || n == 3) return 1;
    if (n % 2 == 0 || n % 3 == 0) return 0;

    long long limite = (long long) sqrt((double)n);

    for (long long i = 5; i <= limite; i += 6) {
        if (n % i == 0 || n % (i + 2) == 0) return 0;
    }

    return 1;
}   

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
int i_global = 0;

void *contaPrimos2(void *arg) {
    int total = 0;

    while (1) {
        int i_local;

        pthread_mutex_lock(&mutex);
        if (i_global >= N) {
            pthread_mutex_unlock(&mutex);
            break;
        }
        i_local = i_global++;
        pthread_mutex_unlock(&mutex);

        if (ehPrimo(i_local))
            total++;
    }

    int *ret = malloc(sizeof(int));
    *ret = total;
    pthread_exit(ret);
}


int main() {
    int T;

    printf("Digite N: ");
    scanf("%lld", &N);

    printf("Digite a quantidade de threads: ");
    scanf("%d", &T);

    if (N < 0 || T <= 0) {
        printf("Valores invalidos.\n");
        return 1;
    }

    pthread_t *threads = malloc(T * sizeof(pthread_t));

    if (threads == NULL) {
        printf("Erro de alocacao.\n");
        return 1;
    }


    for (int i = 0; i < T; i++) {
        if (pthread_create(&threads[i], NULL, contaPrimos2, NULL)) {
            printf("Erro ao criar thread %d.\n", i);
            free(threads);
            return 1;
        }
    }

    long long total = 0;


    for (int i = 0; i < T; i++) {
        void *retorno;
        pthread_join(threads[i], &retorno);
        total += *(int*) retorno;
    }

    printf("Quantidade de primos entre 0 e %lld: %lld\n", N, total);

    free(threads);
    pthread_mutex_destroy(&mutex);

    return 0;
}