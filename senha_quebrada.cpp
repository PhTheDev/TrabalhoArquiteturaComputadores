#include <mpi.h>
#include <iostream>
#include <string>
#include <vector>
#include <cstring>
#include <unistd.h>
#include <cmath>

using namespace std;

// Configurações da senha
const string TARGET_PASSWORD = "aB3@z"; // Senha a ser quebrada (5 caracteres)
const int PASSWORD_LENGTH = 5;

// Conjunto de caracteres possíveis
const string CHARSET = "abcdefghijklmnopqrstuvwxyz"
                      "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                      "0123456789"
                      "!@#$%^&*()_+-=[]{}|;:,.<>?";

const int CHARSET_SIZE = CHARSET.length();
bool password_found = false;

// Função para obter informações do nó
void get_node_info(int rank) {
    char hostname[256];
    gethostname(hostname, sizeof(hostname));
    
    cout << "Processo " << rank << " executando no nó: " << hostname << endl;
}

// Função para converter número para string baseada no charset
string number_to_password(long long num, int length) {
    string result(length, CHARSET[0]);
    
    for (int i = length - 1; i >= 0; i--) {
        result[i] = CHARSET[num % CHARSET_SIZE];
        num /= CHARSET_SIZE;
    }
    
    return result;
}

// Função para calcular total de combinações possíveis
long long calculate_total_combinations() {
    long long total = 1;
    for (int i = 0; i < PASSWORD_LENGTH; i++) {
        total *= CHARSET_SIZE;
    }
    return total;
}

// Função principal de quebra de senha
void bruteforce_password(int rank, int size) {
    long long total_combinations = calculate_total_combinations();
    
    // Dividir o trabalho entre os processos
    long long combinations_per_process = total_combinations / size;
    long long start_index = rank * combinations_per_process;
    long long end_index = (rank == size - 1) ? total_combinations : start_index + combinations_per_process;
    
    cout << "Processo " << rank << " verificando de " << start_index 
         << " até " << end_index - 1 << " (" << end_index - start_index << " combinações)" << endl;
    
    long long attempts = 0;
    for (long long combination = start_index; combination < end_index && !password_found; combination++) {
        attempts++;
        
        // Converter número da combinação para string
        string attempt = number_to_password(combination, PASSWORD_LENGTH);
        
        // Verificar se é a senha correta
        if (TARGET_PASSWORD == attempt) {
            cout << "\n*** SENHA ENCONTRADA! ***" << endl;
            cout << "Processo " << rank << " encontrou a senha: " << attempt << endl;
            cout << "Tentativas realizadas pelo processo " << rank << ": " << attempts << endl;
            password_found = true;
            
            // Notificar todos os outros processos
            for (int i = 0; i < size; i++) {
                if (i != rank) {
                    MPI_Send(&password_found, 1, MPI_C_BOOL, i, 0, MPI_COMM_WORLD);
                }
            }
            return;
        }
        
        // Verificar se algum outro processo encontrou a senha
        if (attempts % 10000 == 0) { // Verificar a cada 10000 tentativas
            int flag;
            MPI_Status status;
            MPI_Iprobe(MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &flag, &status);
            if (flag) {
                bool found_signal;
                MPI_Recv(&found_signal, 1, MPI_C_BOOL, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
                if (found_signal) {
                    cout << "Processo " << rank << " recebeu sinal de parada após " << attempts << " tentativas." << endl;
                    password_found = true;
                    return;
                }
            }
        }
        
        // Mostrar progresso a cada 100000 tentativas
        if (attempts % 100000 == 0) {
            double progress = (double)(combination - start_index) / (end_index - start_index) * 100.0;
            cout << "Processo " << rank << " - Progresso: " << progress << "% (Última tentativa: " << attempt << ")" << endl;
        }
    }
    
    if (!password_found) {
        cout << "Processo " << rank << " terminou sua busca sem encontrar a senha. Total de tentativas: " << attempts << endl;
    }
}

// Versão com loops aninhados (para senhas pequenas)
void bruteforce_nested_loops(int rank, int size) {
    cout << "Processo " << rank << " iniciando busca com loops aninhados..." << endl;
    
    long long attempts = 0;
    int start_char = rank * (CHARSET_SIZE / size);
    int end_char = (rank == size - 1) ? CHARSET_SIZE : start_char + (CHARSET_SIZE / size);
    
    for (int i = start_char; i < end_char && !password_found; i++) {
        for (int j = 0; j < CHARSET_SIZE && !password_found; j++) {
            for (int k = 0; k < CHARSET_SIZE && !password_found; k++) {
                for (int l = 0; l < CHARSET_SIZE && !password_found; l++) {
                    for (int m = 0; m < CHARSET_SIZE && !password_found; m++) {
                        attempts++;
                        
                        string attempt = "";
                        attempt += CHARSET[i];
                        attempt += CHARSET[j];
                        attempt += CHARSET[k];
                        attempt += CHARSET[l];
                        attempt += CHARSET[m];
                        
                        if (TARGET_PASSWORD == attempt) {
                            cout << "\n*** SENHA ENCONTRADA! ***" << endl;
                            cout << "Processo " << rank << " encontrou a senha: " << attempt << endl;
                            cout << "Tentativas realizadas pelo processo " << rank << ": " << attempts << endl;
                            password_found = true;
                            
                            // Notificar todos os outros processos
                            for (int p = 0; p < size; p++) {
                                if (p != rank) {
                                    MPI_Send(&password_found, 1, MPI_C_BOOL, p, 0, MPI_COMM_WORLD);
                                }
                            }
                            return;
                        }
                        
                        // Verificar se outro processo encontrou
                        if (attempts % 10000 == 0) {
                            int flag;
                            MPI_Status status;
                            MPI_Iprobe(MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &flag, &status);
                            if (flag) {
                                bool found_signal;
                                MPI_Recv(&found_signal, 1, MPI_C_BOOL, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
                                if (found_signal) {
                                    cout << "Processo " << rank << " recebeu sinal de parada após " << attempts << " tentativas." << endl;
                                    password_found = true;
                                    return;
                                }
                            }
                        }
                        
                        // Mostrar progresso
                        if (attempts % 50000 == 0) {
                            cout << "Processo " << rank << " - " << attempts << " tentativas (Última: " << attempt << ")" << endl;
                        }
                    }
                }
            }
        }
    }
    
    if (!password_found) {
        cout << "Processo " << rank << " terminou sua busca sem encontrar a senha. Total de tentativas: " << attempts << endl;
    }
}

int main(int argc, char** argv) {
    int rank, size;
    
    // Inicializar MPI
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    
    // Mostrar informações do nó
    get_node_info(rank);
    
    if (rank == 0) {
        cout << "\n=== QUEBRA DE SENHA MPI AVANÇADA ===" << endl;
        cout << "Senha alvo: " << TARGET_PASSWORD << endl;
        cout << "Número de processos: " << size << endl;
        cout << "Comprimento da senha: " << PASSWORD_LENGTH << endl;
        cout << "Tamanho do conjunto de caracteres: " << CHARSET_SIZE << endl;
        cout << "Caracteres possíveis: " << CHARSET << endl;
        
        long long total = calculate_total_combinations();
        cout << "Total de combinações possíveis: " << total << endl;
        cout << "Combinações por processo (aprox): " << total / size << endl;
        cout << "=====================================" << endl;
    }
    
    // Sincronizar todos os processos antes de começar
    MPI_Barrier(MPI_COMM_WORLD);
    
    double start_time = MPI_Wtime();
    
    // Escolher método baseado no número de combinações
    long long total_combinations = calculate_total_combinations();
    
    if (total_combinations > 1000000000LL) { // Mais de 1 bilhão
        cout << "Processo " << rank << " usando método de distribuição matemática (muitas combinações)" << endl;
        bruteforce_password(rank, size);
    } else {
        cout << "Processo " << rank << " usando método de loops aninhados (poucas combinações)" << endl;
        bruteforce_nested_loops(rank, size);
    }
    
    double end_time = MPI_Wtime();
    
    // Sincronizar todos os processos antes de finalizar
    MPI_Barrier(MPI_COMM_WORLD);
    
    if (rank == 0) {
        cout << "\n=====================================" << endl;
        cout << "Tempo total de execução: " << (end_time - start_time) << " segundos" << endl;
        if (password_found) {
            cout << "STATUS: SENHA ENCONTRADA COM SUCESSO!" << endl;
        } else {
            cout << "STATUS: SENHA NÃO ENCONTRADA" << endl;
        }
        cout << "=====================================" << endl;
    }
    
    // Finalizar MPI
    MPI_Finalize();
    return 0;
}

/* 
INSTRUÇÕES DE COMPILAÇÃO E EXECUÇÃO:

1. Compilar:
   mpic++ -O2 -Wall -o quebra_senha quebra_senha_mpi.cpp

2. Executar localmente:
   mpirun -np 4 ./quebra_senha

3. Executar no cluster:
   mpirun -np 8 -hostfile hosts.txt ./quebra_senha

4. Para criar arquivo hosts.txt:
   echo "192.168.1.10 slots=2" > hosts.txt
   echo "192.168.1.11 slots=2" >> hosts.txt
   echo "192.168.1.12 slots=2" >> hosts.txt

5. Exemplo de execução com mais processos:
   mpirun -np 16 -hostfile hosts.txt ./quebra_senha

CONFIGURAÇÕES PERSONALIZÁVEIS:
- TARGET_PASSWORD: Mude para a senha que quer quebrar
- PASSWORD_LENGTH: Comprimento da senha (atualmente 5)
- CHARSET: Conjunto de caracteres possíveis
- Frequências de verificação (10000, 100000, 50000)

NOTA: Com 5 caracteres e 94 caracteres possíveis, são aproximadamente
7.3 bilhões de combinações possíveis!
*/