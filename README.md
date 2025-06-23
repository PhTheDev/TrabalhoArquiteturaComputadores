# Quebra de Senha Paralela com MPI

Este projeto implementa um programa de quebra de senha por força bruta utilizando o Message Passing Interface (MPI) para paralelização. Ele é projetado para distribuir o trabalho de adivinhação de senhas entre múltiplos processos, acelerando significativamente o processo de descoberta de senhas, especialmente para senhas mais longas e complexas.

## Funcionalidades

*   **Processamento Distribuído**: Utiliza MPI para dividir o espaço de busca da senha entre vários nós ou núcleos de CPU.
*   **Dois Métodos de Força Bruta**:
    *   **Distribuição Matemática**: Para senhas com um grande número de combinações (acima de 1 bilhão), o trabalho é dividido matematicamente entre os processos.
    *   **Loops Aninhados**: Para senhas com um número menor de combinações, um método de loops aninhados é empregado para uma busca mais direta.
*   **Detecção de Senha Encontrada**: Uma vez que um processo encontra a senha, ele notifica os outros processos para que parem a busca, otimizando o tempo de execução.
*   **Monitoramento de Progresso**: Exibe o progresso da busca para cada processo, bem como o tempo total de execução.
*   **Configurável**: Permite fácil personalização da senha alvo, comprimento da senha e conjunto de caracteres.
