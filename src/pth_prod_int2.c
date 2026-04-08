#define _GNU_SOURCE // Necessário para usar funções de afinidade
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <sched.h>

#define TAMANHO 100000000 // 100 milhoes de elementos
#define VEZES 100 // Número de vezes que o cálculo será repetido
#define NUM_THREADS 4 // Define o número de threads

// Estrutura para passar dados para as threads
typedef struct {
    double *vecA;
    double *vecB;
    int inicio;
    int fim;
    int tid;
    double resultado_parcial;
} ThreadData;

double tempoAtual() {
    struct timespec tempo;
    clock_gettime(CLOCK_MONOTONIC_RAW, &tempo);
    return tempo.tv_sec + tempo.tv_nsec / 1000000000.0;
}

// Função que cada thread executará
void* calcular_parcial(void* arg) {
    ThreadData* data = (ThreadData*)arg;
    register double temp;
    int cpu_alvo = data->tid;
    cpu_set_t cpuset;

    // Limpa a máscara de CPUs
    CPU_ZERO(&cpuset);
    // Adiciona o núcleo desejado à máscara (ex: núcleo 0)
    CPU_SET(cpu_alvo, &cpuset);

    // Aplica a afinidade à thread atual
    pthread_t thread = pthread_self();
    int s = pthread_setaffinity_np(thread, sizeof(cpu_set_t), &cpuset);

    for(int j = 0; j < VEZES; j++) {
      temp = 0.0;
      for (int i = data->inicio; i < data->fim; i++) {
          temp += data->vecA[i] * data->vecB[i];
      }
    }
    data->resultado_parcial = temp;
    pthread_exit(NULL);
}

int main() {
    double *vecA = (double *)malloc(TAMANHO * sizeof(double));
    double *vecB = (double *)malloc(TAMANHO * sizeof(double));
    double resultado = 0.0;

    if (vecA == NULL || vecB == NULL) return 1;

    for (int i = 0; i < TAMANHO; i++) {
        vecA[i] = 1.0;
        vecB[i] = 2.0;
    }

    printf("Iniciando cálculo paralelo (%d threads) para %d elementos...\n", NUM_THREADS, TAMANHO);

    pthread_t threads[NUM_THREADS];
    ThreadData thread_data[NUM_THREADS];
    int chunk = TAMANHO / NUM_THREADS;


    // Laço externo original
    	double inicio_tempo = tempoAtual();
        // --- INÍCIO DA PARALELIZAÇÃO DO LAÇO INTERNO ---
        for (int t = 0; t < NUM_THREADS; t++) {
            thread_data[t].vecA = vecA;
            thread_data[t].vecB = vecB;
            thread_data[t].inicio = t * chunk;
            // Garante que o último chunk pegue o resto da divisão
            thread_data[t].fim = (t == NUM_THREADS - 1) ? TAMANHO : (t + 1) * chunk;
	    thread_data[t].tid=t;
            pthread_create(&threads[t], NULL, calcular_parcial, (void*)&thread_data[t]);
        }
        for (int t = 0; t < NUM_THREADS; t++) {
            pthread_join(threads[t], NULL);
            resultado += thread_data[t].resultado_parcial;
        }
        // --- FIM DA PARALELIZAÇÃO DO LAÇO INTERNO ---

    double fim_tempo = tempoAtual();

    double esperado = (double)TAMANHO * 2.0;
    printf("--- Resultados ---\n");
    printf("Tempo de Execução: %.6f segundos\n", fim_tempo - inicio_tempo);
    printf("Resultado Obtido:  %.2f | Esperado: %.2f\n", resultado, esperado);
    printf("Verificação: %s\n", (resultado == esperado) ? "SUCESSO" : "FALHA");

    free(vecA); free(vecB);
    return 0;
}
