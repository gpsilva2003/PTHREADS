#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>

typedef struct {
    long long inicio;
    long long fim;
    long long qtd_primos;
} Faixa;

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
    Faixa *faixa = (Faixa *) arg;
    register long long qtd_local = 0; 
    for (long long i = faixa->inicio; i <= faixa->fim; i++) {
        if (ehPrimo(i)) {
            qtd_local++;
        }
    }
    faixa->qtd_primos = qtd_local;
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
    Faixa *faixas = malloc(T * sizeof(Faixa));

    if (threads == NULL || faixas == NULL) {
        printf("Erro de alocacao.\n");
        return 1;
    }

    long long tamanho = (N + 1) / T;
    long long resto = (N + 1) % T;
    long long atual = 0;

    clock_gettime(CLOCK_MONOTONIC_RAW, &inicio);

    for (int i = 0; i < T; i++) {
        long long extra = (i < resto) ? 1 : 0;

        faixas[i].inicio = atual;
        faixas[i].fim = atual + tamanho + extra - 1;
        faixas[i].qtd_primos = 0;

        atual = faixas[i].fim + 1;

        if (pthread_create(&threads[i], NULL, contaPrimos, &faixas[i]) != 0) {
            printf("Erro ao criar thread %d.\n", i);
            free(threads);
            free(faixas);
            return 1;
        }
    }

    long long total = 0;

    for (int i = 0; i < T; i++) {
        pthread_join(threads[i], NULL);
        total += faixas[i].qtd_primos;
    }

    clock_gettime(CLOCK_MONOTONIC_RAW, &fim);
    tempo_gasto = (fim.tv_sec - inicio.tv_sec) +
                  (fim.tv_nsec - inicio.tv_nsec) / 1e9;
    printf("Quantidade de primos entre 0 e %lld: %lld\n", N, total);
    printf("Tempo de execucao: %.6f segundos\n", tempo_gasto);

    free(threads);
    free(faixas);
    return 0;
}