#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>

typedef struct {
    // Parametros de entrada
    long long fim;
    int nthreads;
    int tid;
    // Parametros de saida
    long long qtd_primos;
} Parametros;

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

void *contaPrimos(void *arg) {
    Parametros *param = (Parametros *) arg;
    register long long primos_local = 0;

    for (long long i = 3 + 2 * param->tid; i <= param->fim; i += 2 * param->nthreads) {
        if (ehPrimo(i)) {
            primos_local++;
        }
    }
    param->qtd_primos = primos_local;
    return NULL;
}

int main() {
    long long N;
    int T;
    struct timespec inicio, fim;
    double tempo_gasto;

    printf("Digite N: ");
    scanf("%lld", &N);

    printf("Digite a quantidade de threads: ");
    scanf("%d", &T);

    if (N < 0 || T <= 0) {
        printf("Valores invalidos.\n");
        return 1;
    }

    pthread_t *threads = malloc(T * sizeof(pthread_t));
    Parametros *param = malloc(T * sizeof(Parametros));

    if (threads == NULL || param == NULL) {
        printf("Erro de alocacao.\n");
        return 1;
    }
    clock_gettime(CLOCK_MONOTONIC_RAW, &inicio);

    for (int i = 0; i < T; i++) {

        param[i].fim = N;
        param[i].nthreads = T;
        param[i].tid = i;

        if (pthread_create(&threads[i], NULL, contaPrimos, &param[i]) != 0) {
            printf("Erro ao criar thread %d.\n", i);
            free(threads);
            free(param);
            return 1;
        }
    }

    long long total = 0;

    for (int i = 0; i < T; i++) {
        pthread_join(threads[i], NULL);
        total += param[i].qtd_primos;
    }

    clock_gettime(CLOCK_MONOTONIC_RAW, &fim);
    tempo_gasto = (fim.tv_sec - inicio.tv_sec) +
                  (fim.tv_nsec - inicio.tv_nsec) / 1e9;
    printf("Quantidade de primos entre 0 e %lld: %lld\n", N, total);
    printf("Tempo de execucao: %.6f segundos\n", tempo_gasto);

    free(threads);
    free(param);

    return 0;
}
