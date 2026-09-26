# Codexion — Guia de Estudos Acadêmico (Português)

*Simulação de Concorrência: POSIX Threads, Algoritmos de Escalonamento e Primitivas de Sincronização*

---

## Informações do Curso

| Campo | Detalhe |
|-------|---------|
| **Projeto** | Codexion (Currículo 42) |
| **Domínio** | Programação de Sistemas / Concorrência em Tempo Real |
| **Linguagem** | C (C99, POSIX.1-2008) |
| **Pré-requisitos** | Ponteiros em C, gerenciamento de memória, conceitos básicos de SO |
| **Tempo Estimado Total** | 25–35 horas |

---

## Mapa de Conceitos & Tempo Estimado de Estudo

| # | Conceito | Subtópicos | Tempo Est. | Pré-requisitos |
|---|----------|------------|------------|----------------|
| 1 | **POSIX Threads (pthreads)** | Ciclo de vida, atributos, join/detach, design thread-safe | 4–5 h | Funções C, ponteiros |
| 2 | **Exclusão Mútua (Mutexes)** | Lock/unlock, trylock, mutex recursivo vs. rápido, hierarquias de locks | 3–4 h | Conceito 1 |
| 3 | **Variáveis de Condição** | Wait/signal/broadcast, espera temporizada, wakeups espúrios, loops de predicado | 4–5 h | Conceitos 1–2 |
| 4 | **Teoria & Prevenção de Deadlock** | Condições de Coffman, grafos de alocação, ordenação de locks, backoff com trylock | 3–4 h | Conceitos 2–3 |
| 5 | **Algoritmos de Escalonamento** | FIFO vs. EDF, optimalidade de Liu & Layland, limites de utilização, teste de densidade | 4–5 h | Matemática discreta, algoritmos |
| 6 | **Filas de Prioridade (Heaps Binários)** | Propriedade de heap, swim/sink, insert/extract O(log n), heapify, representação em array | 3–4 h | Estruturas de dados, arrays |
| 7 | **Restrições de Tempo Real** | Deadline monotonic, detecção de burnout, cooldown, temporização de precisão | 2–3 h | Conceitos 5–6 |
| 8 | **Gerenciamento de Memória & Recursos** | Propriedade (ownership), padrões RAII em C, prevenção de vazamentos, valgrind/helgrind | 2–3 h | C malloc/free |

> **Nota Pedagógica**: Conceitos 1–3 formam a *fundação de sincronização*; 4 é a *garantia de correção*; 5–6 são o *núcleo algorítmico*; 7–8 são *integração de sistemas*. Estude na ordem.

---

## Índice de Referências

| # | Citação | Tipo | Capítulos/Seções Principais |
|---|---------|------|----------------------------|
| **[R1]** | Kerrisk, M. *The Linux Programming Interface*. No Starch Press, 2010. | Livro-texto | Cap. 29–31 (Threads, Mutexes, CVs), Cap. 23 (Timers) |
| **[R2]** | Butenhof, D. *Programming with POSIX Threads*. Addison-Wesley, 1997. | Livro-texto | Cap. 2–5 (Mutexes, CVs, Deadlock), Cap. 8 (Performance) |
| **[R3]** | Arpaci-Dusseau, R. & A. *Operating Systems: Three Easy Pieces*. 2018. | Livro-texto (Gratuito) | Cap. 26–31 (Locks, CVs, Deadlock) |
| **[R4]** | Herlihy, M. & Shavit, N. *The Art of Multiprocessor Programming*. Morgan Kaufmann, 2012. | Livro-texto | Cap. 2 (Exclusão Mútua), Cap. 3 (Spin Locks), Cap. 8 (Filas de Prioridade) |
| **[R5]** | Liu, C.L. & Layland, J.W. "Scheduling Algorithms for Multiprogramming in a Hard-Real-Time Environment." *JACM* 20(1), 1973. | Artigo Seminal | Artigo completo (optimalidade EDF, limite RM) |
| **[R6]** | Liu, J.W.S. *Real-Time Systems*. Prentice Hall, 2000. | Livro-texto | Cap. 3–6 (EDF, RM, Escalonabilidade, Densidade) |
| **[R7]** | Coffman, E.G. et al. "System Deadlocks." *ACM Computing Surveys* 3(2), 1971. | Artigo Seminal | Quatro condições necessárias |
| **[R8]** | Cormen, T.H. et al. *Introduction to Algorithms* (CLRS), 3ª ed. MIT Press, 2009. | Livro-texto | Cap. 6 (Heapsort), Cap. 19 (Heaps Binomiais) |
| **[R9]** | Sedgewick, R. & Wayne, K. *Algorithms*, 4ª ed. Princeton, 2011. | Livro-texto | §2.4 (Filas de Prioridade, Heaps Binários) |
| **[R10]** | LLNL. *POSIX Threads Programming Tutorial*. Lawrence Livermore National Lab. | Tutorial | https://computing.llnl.gov/tutorials/pthreads/ |
| **[R11]** | Baker, T.P. "Stack-Based Scheduling of Real-Time Processes." *Advances in Real-Time Systems*, 1993. | Artigo | Prova de densidade EDF |
| **[R12]** | Baruah, S. et al. "Algorithms and Complexity Concerning the Preemptive Scheduling of Periodic Real-Time Tasks." *Real-Time Systems*, 1990. | Artigo | Critério de demanda de processador |
| **[R13]** | Dertouzos, M.L. "Control Robotics: The Procedural Control of Physical Processes." *IFAC*, 1974. | Artigo | Optimalidade EDF (argumento de troca) |
| **[R14]** | Solaris/IBM. *Multithreaded Programming Guide*. Oracle/IBM Docs. | Referência | Hierarquias de locks, atributos de condvar |
| **[R15]** | Michael, M.M. & Scott, M.L. "Simple, Fast, and Practical Non-Blocking and Blocking Concurrent Queue Algorithms." *PODC*, 1996. | Artigo | Alternativas lock-free (avançado) |

---

## 1. POSIX Threads — A Fundação da Concorrência

### 1.1 Modelo de Thread & Ciclo de Vida

POSIX threads (pthreads) fornecem uma API C padronizada para concorrência de memória compartilhada. Cada thread possui sua própria pilha, conjunto de registradores e armazenamento local à thread (TLS), mas compartilha o heap do processo, descritores de arquivo e manipuladores de sinais.

**API Principal** (de **[R1] Cap. 29**, **[R2] Cap. 2**):

```c
// Criação de thread
int pthread_create(pthread_t *thread, const pthread_attr_t *attr,
                   void *(*start_routine)(void *), void *arg);

// Término de thread
void pthread_exit(void *retval);
int pthread_join(pthread_t thread, void **retval);
int pthread_detach(pthread_t thread);
```

**Aplicação no Codexion**: Cada coder é um `pthread_t` criado em `simulation.c:start_coders()`. A thread monitora executa `monitor_routine()`. Todas as threads são unidas (joined) em `cleanup_simulation()`.

> **Exercício de Estudo**: Modifique `coder_routine` para aceitar um `struct timespec` com tempos de "pensamento" aleatórios. Observe como a não-determinismo do escalonador de threads afeta a ordenação dos logs.

### 1.2 Atributos de Thread & Escalonamento

`pthread_attr_t` controla tamanho de pilha, páginas de guarda, política de escalonamento (`SCHED_FIFO`, `SCHED_RR`, `SCHED_OTHER`) e herança. O Codexion usa atributos padrão (`NULL`), significando `SCHED_OTHER` (time-sharing) — o escalonador do SO decide qual thread roda. Nosso escalonador *de nível de aplicação* (FIFO/EDF) opera *acima* do escalonador do SO controlando o acesso aos dongles.

> **Leitura Avançada**: **[R1] §29.7** (Escalonamento de threads), **[R2] §3.4** (Políticas de escalonamento). Políticas de tempo real requerem `CAP_SYS_NICE` e são raramente usadas em código de usuário.

---

## 2. Exclusão Mútua — Mutexes

### 2.1 Semântica de Mutex

Um mutex (exclusão mútua) é uma primitiva de sincronização com duas operações atômicas: `lock` e `unlock`. O mutex POSIX garante:

1. **Exclusão Mútua**: No máximo uma thread segura o lock.
2. **Progresso**: Se nenhuma thread segura o lock, uma thread esperando eventualmente o adquire.
3. **Espera Limitada**: Nenhuma thread espera para sempre (assumindo escalonamento justo).

**API Principal** (**[R1] §30.2**, **[R2] §3.2**, **[R3] Cap. 26**):

```c
int pthread_mutex_init(pthread_mutex_t *mutex, const pthread_mutexattr_t *attr);
int pthread_mutex_lock(pthread_mutex_t *mutex);
int pthread_mutex_trylock(pthread_mutex_t *mutex);  // Não-bloqueante
int pthread_mutex_unlock(pthread_mutex_t *mutex);
int pthread_mutex_destroy(pthread_mutex_t *mutex);
```

### 2.2 Tipos de Mutex

| Tipo | Comportamento | Caso de Uso |
|------|---------------|-------------|
| `PTHREAD_MUTEX_DEFAULT` (rápido) | Comportamento indefinido em lock recursivo | Maioria dos casos |
| `PTHREAD_MUTEX_ERRORCHECK` | Retorna `EDEADLK` em lock recursivo | Depuração |
| `PTHREAD_MUTEX_RECURSIVE` | Permite mesma thread travar múltiplas vezes | Código reentrante |
| `PTHREAD_MUTEX_NORMAL` | Sem verificação de erro, sem recursão | Crítico para performance |

O Codexion usa mutexes padrão (rápidos). **Nunca** chame `pthread_mutex_lock` duas vezes no mesmo mutex rápido pela mesma thread — causa deadlock.

### 2.3 Hierarquias de Locks — Prevenindo Deadlock por Design

A técnica mais eficaz de prevenção de deadlock é a **ordenação global de locks** (**[R7]**, **[R14]**, **[R3] Cap. 30**).

**Teorema**: Se todas as threads adquirem múltiplos locks em uma ordem total globalmente consistente, espera circular (condição #4 de Coffman) é impossível.

**Esboço da Prova**: Um ciclo no grafo de espera requer que alguma thread segure lock `L_i` e espere por `L_j` onde `j < i` (aresta "para trás"). Se todas as threads só adquirem locks em ordem crescente, não existem arestas para trás → não há ciclos.

**Implementação no Codexion** (`coder_utils.c:acquire_dongles`):

```c
void acquire_dongles(t_coder *coder)
{
    if (coder->sim->num_coders == 1) { /* dongle único */ }
    // Ordem global: menor ID de dongle primeiro
    if (coder->left_dongle->id < coder->right_dongle->id)
        acquire_first_dongle(coder, coder->left_dongle, coder->right_dongle);
    else
        acquire_first_dongle(coder, coder->right_dongle, coder->left_dongle);
}
```

**Por que funciona**: Com N coders em círculo, coder `i` precisa dos dongles `i` e `(i+1)%N`. Sem ordenação, coder 1 segura 0→espera 1, coder 2 segura 1→espera 2, ..., coder N segura N-1→espera 0: **ciclo**. Ao sempre pegar o menor ID primeiro, impomos ordem total `0 < 1 < ... < N-1`. O ciclo é quebrado.

> **Insight Crítico**: Ordenação de locks funciona *apenas se todo caminho de código a obedece*. Uma única função perdida que trava na ordem inversa reintroduz o bug. Roteie toda aquisição multi-lock através de um único helper (`acquire_first_dongle`).

### 2.4 Alternativa: Backoff com `pthread_mutex_trylock`

Quando ordenação global é impraticável (ex.: locks escolhidos em tempo de execução de um grande conjunto), use **trylock com backoff** (**[R2] §3.3**, **[R14]**):

```c
// Trava primeiro mutex normalmente
pthread_mutex_lock(&m1);
// Tenta segundo; se ocupado, libera primeiro e tenta novamente
if (pthread_mutex_trylock(&m2) == EBUSY) {
    pthread_mutex_unlock(&m1);
    // Opcional: backoff exponencial
    goto retry;
}
```

**Armadilha**: Esquecer de liberar o primeiro lock em `EBUSY` vaza o lock — o bug mais comum de trylock.

---

## 3. Variáveis de Condição — Espera Eficiente

### 3.1 O Problema da Espera Ocupada (Busy Waiting)

Um waiter ingênuo poderia girar:

```c
while (!condition) { /* busy wait */ }
```

Isso desperdiça ciclos de CPU. Variáveis de condição (CVs) permitem que threads **durmam** até serem sinalizadas.

### 3.2 Mecânica de CV

Uma CV é **sempre pareada com um mutex**. O padrão canônico (**[R1] §30.3**, **[R2] §3.5**, **[R3] Cap. 28**):

```c
// Waiter
pthread_mutex_lock(&mutex);
while (!predicate) {
    pthread_cond_wait(&cond, &mutex);  // Atomicamente unlock mutex + dorme
}
// predicate agora verdadeiro, mutex segurado
pthread_mutex_unlock(&mutex);

// Signaler
pthread_mutex_lock(&mutex);
predicate = true;
pthread_cond_signal(&cond);  // ou broadcast
pthread_mutex_unlock(&mutex);
```

**Por que o loop `while`?** Wakeups espúrios: `pthread_cond_wait` pode retornar sem sinal. A re-verificação do predicado trata isso.

**Por que mutex travado antes do wait?** O wait atomicamente destrava o mutex e bloqueia. Sem o mutex, um sinal poderia ocorrer entre verificar o predicado e chamar wait — o sinal seria perdido.

### 3.3 Esperas Temporizadas — Implementação do Cooldown

`pthread_cond_timedwait` aceita um **timeout absoluto** (`struct timespec`). Codexion usa isso para cooldown de dongle (`dongle_utils.c:wait_for_cooldown`):

```c
void wait_for_cooldown(t_dongle *dongle)
{
    struct timespec ts;
    long now = get_current_time_ms();
    if (now >= dongle->cooldown_end) return;
    long wait_ms = dongle->cooldown_end - now;
    ts.tv_sec = wait_ms / 1000;
    ts.tv_nsec = (wait_ms % 1000) * 1000000;
    pthread_cond_timedwait(&dongle->cond, &dongle->mutex, &ts);
}
```

**Por que tempo absoluto?** POSIX especifica timeout absoluto para evitar deriva de esperas relativas repetidas. O chamador deve computar `now + wait_ms` a cada chamada.

### 3.4 Signal vs. Broadcast

| Função | Acorda | Use Quando |
|--------|--------|------------|
| `pthread_cond_signal` | Pelo menos um waiter | Recurso único, um waiter pode prosseguir |
| `pthread_cond_broadcast` | Todos os waiters | Mudança de estado afeta todos (ex.: shutdown) |

Codexion usa `signal` para liberação de dongle (um waiter o obtém) e `broadcast` para shutdown da simulação (`simulation.c:broadcast_all_dongles`).

---

## 4. Teoria de Deadlock — Fundamentos Formais

### 4.1 As Condições de Coffman ([R7])

Deadlock ocorre **se e somente se** todas as quatro valerem simultaneamente:

| # | Condição | Formulação Formal |
|---|----------|-------------------|
| 1 | **Exclusão Mútua** | ∃ recurso R: apenas uma thread pode segurar R por vez |
| 2 | **Segurar e Esperar** | ∃ thread T segurando R₁ enquanto espera por R₂ |
| 3 | **Não Preempção** | Recursos não podem ser tomados à força dos detentores |
| 4 | **Espera Circular** | ∃ ciclo T₀→R₁→T₁→R₂→...→Tₙ→R₀→T₀ no grafo de espera |

São **necessárias mas não suficientes** — todas quatro podem valer sem deadlock (ex.: timing impede formação do ciclo), mas deadlock *não pode* ocorrer se alguma estiver ausente.

### 4.2 Grafos de Alocação de Recursos (RAG)

Um RAG é um grafo direcionado com:
- **Nós de processo** (círculos)
- **Nós de recurso** (quadrados)
- **Arestas de requisição** P→R (P espera por R)
- **Arestas de atribuição** R→P (R segurado por P)

**Teorema** (**[R1] §8.3.2**): Um ciclo no RAG é **necessário** para deadlock. Se todos os recursos têm instância única, um ciclo também é **suficiente**.

### 4.3 Estratégias de Prevenção (Eliminar Uma Condição)

| Estratégia | Condição Quebrada | Praticidade |
|------------|-------------------|-------------|
| Tornar todos recursos compartilháveis | Exclusão Mútua | Raramente possível (escritas precisam exclusão) |
| Adquirir todos locks atomicamente / liberar antes de requisitar | Segurar e Esperar | Impraticável para necessidades dinâmicas |
| Permitir preempção (trylock + backoff) | Não Preempção | Complexo, risco de livelock |
| **Ordenação global de locks** | **Espera Circular** | **Melhor escolha prática** |

Codexion usa **ordenação de locks** (Condição 4). Ver §2.3.

### 4.4 Detecção & Recuperação (Quando Prevenção Falha)

Se prevenção é impossível, sistemas usam:
- **Detecção**: Detecção periódica de ciclos no RAG (O(n²) para recursos de instância única)
- **Recuperação**: Seleção de vítima → matar thread → liberar locks → reiniciar

**Nota do Codexion**: A thread monitora *detecta* burnout (falha de vivacidade), não deadlock. Nossa ordenação de locks torna deadlock impossível por construção.

---

## 5. Algoritmos de Escalonamento — FIFO vs. EDF

### 5.1 Enunciado do Problema

Temos N coders competindo por N dongles. Cada coder precisa de 2 dongles adjacentes. Quando múltiplos coders esperam pelo mesmo dongle, o **escalonador** decide quem o obtém.

### 5.2 FIFO (First In, First Out)

**Política**: Requisições servidas em ordem de chegada.

**Propriedades**:
- Simples, justo em ordem de chegada
- Sem starvation se sistema estável
- Sem inversão de prioridade

**Implementação no Codexion** (`heap_ops.c:compare_requests`):

```c
if (type == SCHED_TYPE_FIFO) {
    if (a->arrival_time < b->arrival_time) return -1;
    if (a->arrival_time > b->arrival_time) return 1;
    return 0;
}
```

### 5.3 EDF (Earliest Deadline First) — Escalonamento Dinâmico Ótimo

**Política**: Requisição com deadline absoluto mais cedo executa primeiro.

**Fundamentação Teórica** (**[R5]**, **[R6] Cap. 4**, **[R13]**):

**Teorema (Liu & Layland, 1973; Dertouzos, 1974)**: Para jobs independentes, preemptíveis, em processador único, EDF é **ótimo** — se *qualquer* algoritmo pode escalonar um conjunto de tarefas, EDF pode.

**Esboço da Prova (Argumento de Troca)**: Tome qualquer escalonamento viável. Encontre o primeiro ponto onde ele desvia de EDF (job A roda mas job B tem deadline mais cedo). Troque A e B. A troca não pode causar deadline miss porque deadline de B é mais cedo. Repita até o escalonamento coincidir com EDF.

**Teste de Escalonabilidade** (Deadlines implícitas, D = T):

```
U = Σ(Cᵢ / Tᵢ) ≤ 1   ⟺   EDF viável
```

Onde Cᵢ = tempo de execução, Tᵢ = período.

**Deadlines Restritas (D ≤ T)**: Use **densidade** δᵢ = Cᵢ / min(Dᵢ, Tᵢ). Condição suficiente: Σδᵢ ≤ 1 (**[R6] §4.5**, **[R11]**).

**Modelo de Deadline no Codexion**: Deadline de cada coder = `last_compile_start + time_to_burnout`. É um **deadline relativo** a partir do último compile. O escalonador usa deadlines absolutos para comparação.

**Comparador EDF do Codexion** (`heap_ops.c`):

```c
// EDF: deadline mais cedo primeiro
if (a->deadline < b->deadline) return -1;
if (a->deadline > b->deadline) return 1;
// Desempate: tempo de chegada para determinismo
if (a->arrival_time < b->arrival_time) return -1;
if (a->arrival_time > b->arrival_time) return 1;
return 0;
```

**Por que o desempate?** Precisão de milissegundo significa deadlines iguais podem ocorrer. Tempo de chegada garante comportamento determinístico, reproduzível — crítico para avaliação e depuração.

### 5.4 EDF no Codexion — Considerações Especiais

Os "jobs" do Codexion são **requisições de aquisição de dongle**, não tarefas de CPU. O escalonador arbitra *acesso a dongles*, não tempo de CPU. Contudo, o mesmo princípio de optimalidade aplica: servir o coder mais próximo do burnout primeiro maximiza a chance de todos cumprirem deadlines.

**Condição de Viabilidade para Codexion**: O sistema é viável se a demanda total de "compile" cabe na janela de burnout. Aproximadamente:

```
N * time_to_compile / time_to_burnout ≤ 1   (para 2 dongles por coder, mais complexo)
```

Análise exata requer critério de demanda de processador (**[R12]**).

---

## 6. Filas de Prioridade — Implementação com Heap Binário

### 6.1 Tipo Abstrato de Dados

Uma fila de prioridade suporta:
- `insert(chave, prioridade)` — O(log n)
- `extract_max()` / `extract_min()` — O(log n)
- `peek()` — O(1)
- `size()`, `is_empty()` — O(1)

### 6.2 Propriedades do Heap Binário

Um **heap binário** é uma árvore binária completa satisfazendo a **propriedade de heap**:

- **Min-heap**: `chave_pai ≤ chave_filho` (raiz = mínimo)
- **Max-heap**: `chave_pai ≥ chave_filho` (raiz = máximo)

**Representação em Array** (**[R8] Cap. 6**, **[R9] §2.4**):

```
Índice:     1   2   3   4   5   6   7
Árvore:      A
            / \
           B   C
          / \ / \
         D  E F  G
```

Para nó no índice `k` (base-1):
- Pai: `k/2`
- Filho esquerdo: `2k`
- Filho direito: `2k+1`

Codexion usa indexação base-0 (`heap.c`, `heap_ops.c`):
- Pai: `(k-1)/2`
- Esquerdo: `2k+1`
- Direito: `2k+2`

### 6.3 Operações Principais

**Swim (Percolate Up)** — após insert no final:

```c
void heapify_up(t_heap *heap, int idx)
{
    while (idx > 0) {
        int parent = (idx - 1) / 2;
        if (compare(heap->data[idx], heap->data[parent], heap->type) >= 0)
            break;
        swap(heap->data[idx], heap->data[parent]);
        idx = parent;
    }
}
```

**Sink (Percolate Down)** — após extract (raiz substituída pelo último elemento):

```c
void heapify_down(t_heap *heap, int idx)
{
    while (1) {
        int smallest = idx;
        int left = 2 * idx + 1;
        int right = 2 * idx + 2;
        if (left < heap->size && compare(heap->data[left], heap->data[smallest], heap->type) < 0)
            smallest = left;
        if (right < heap->size && compare(heap->data[right], heap->data[smallest], heap->type) < 0)
            smallest = right;
        if (smallest == idx) break;
        swap(heap->data[idx], heap->data[smallest]);
        idx = smallest;
    }
}
```

**Complexidade**: Ambos O(log n) — altura de árvore binária completa é ⌊log₂ n⌋.

### 6.4 Design do Heap no Codexion

- **Heaps por dongle**: Cada dongle tem sua própria fila de requisições (`t_dongle.heap`). Evita contenção de lock global.
- **Comparador parametrizado por escalonador**: Mesma estrutura de heap, diferente função `compare_requests`.
- **Capacidade dinâmica**: Criado com capacidade `num_coders * 10` (suficiente para todos waiters).

> **Exercício de Estudo**: Implemente `heap_decrease_key` (para atualização de prioridade) e analise seu uso no algoritmo de Dijkstra.

---

## 7. Restrições de Tempo Real — Precisão & Vivacidade

### 7.1 Medição de Tempo

Codexion usa `gettimeofday()` para timestamps em milissegundos (**[R1] §23.4**):

```c
long get_current_time_ms(void)
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (tv.tv_sec * 1000 + tv.tv_usec / 1000);
}
```

**Por que não `clock_gettime(CLOCK_MONOTONIC)`?** `gettimeofday` é mais simples e suficiente. `CLOCK_MONOTONIC` é imune a ajustes NTP mas requer `librt` em alguns sistemas. O subject permite `gettimeofday`.

### 7.2 Sleep de Precisão

`nanosleep` com loop de retry trata `EINTR` (interrupção por sinal):

```c
void precise_sleep(long ms)
{
    struct timespec req = { ms / 1000, (ms % 1000) * 1000000 };
    struct timespec rem;
    while (nanosleep(&req, &rem) == -1)
        req = rem;
}
```

**Por que retry?** Se sinal chega durante sleep, `nanosleep` retorna -1 com `errno=EINTR` e escreve tempo restante em `rem`. O loop continua dormindo o restante.

### 7.3 Detecção de Burnout — Monitor de Vivacidade

A thread monitora verifica deadlines a cada 1 ms (`usleep(1000)`). Garante burnout logado dentro de 10 ms do deadline real.

**Argumento de Correção**:
- Período do monitor = 1 ms
- Pior caso: deadline passa logo após verificação → detectado na próxima (≤1 ms depois)
- Logging adiciona overhead desprezível (mutex + printf)
- Total ≤ 10 ms requisitado satisfeito

**Implementação no Codexion** (`monitor.c:monitor_routine`):

```c
while (1) {
    pthread_mutex_lock(&sim->stop_mutex);
    if (sim->stop_simulation) { pthread_mutex_unlock(&sim->stop_mutex); break; }
    pthread_mutex_unlock(&sim->stop_mutex);
    
    long current_time = get_timestamp_ms(sim);
    for (int i = 0; i < sim->num_coders; i++) {
        if (current_time >= sim->coders[i].deadline) {
            log_state(sim, sim->coders[i].id, STATE_BURNED_OUT);
            // ... para simulação
            return NULL;
        }
    }
    usleep(1000);
}
```

### 7.4 Cooldown — Proteção de Recurso

Após liberação, dongle fica indisponível por `dongle_cooldown` ms. Modela tempo de reconexão física do dongle e impede re-aquisição imediata pelo mesmo coder (que poderia starvar outros).

**Implementação**: Timestamp `cooldown_end` + `pthread_cond_timedwait` (ver §3.3).

---

## 8. Gerenciamento de Memória & Recursos

### 8.1 Modelo de Propriedade (Ownership)

| Recurso | Dono | Tempo de Vida |
|---------|------|---------------|
| Array `t_coder[]` | `t_simulation` | Duração da simulação |
| Array `t_dongle[]` | `t_simulation` | Duração da simulação |
| `t_heap` (por dongle) | `t_dongle` | Vida do dongle |
| Nós `t_request` | `t_heap` | Enqueue → dequeue |
| Pilhas de threads | SO | Vida da thread |

### 8.2 Protocolo de Limpeza

`cleanup_simulation()` (**[R2] §2.4**):

1. Join em todas threads coder (aguarda término)
2. Libera array de coders
3. Destroi todos dongles (mutex, condvar, heap)
4. Destroi mutexes/condvars da simulação

**Sem vazamentos de memória**: Todo `malloc` tem `free` correspondente. Todo `pthread_mutex_init` tem `pthread_mutex_destroy`. Todo `pthread_cond_init` tem `pthread_cond_destroy`.

### 8.3 Ferramentas de Verificação

```bash
# Vazamentos de memória
valgrind --leak-check=full --show-leak-kinds=all ./codexion ...

# Erros de thread (data races, deadlocks)
valgrind --tool=helgrind ./codexion ...

# Thread sanitizer (compile-time)
cc -fsanitize=thread -g -O1 ... -o codexion_tsan
./codexion_tsan ...
```

---

## 9. Análise Profunda da Arquitetura Codexion

### 9.1 Estruturas de Dados

```c
// Tipos principais (types.h)
typedef enum { SCHED_TYPE_FIFO, SCHED_TYPE_EDF } t_scheduler;

typedef struct s_request {
    int coder_id;
    long deadline;       // Para EDF
    long arrival_time;   // Para FIFO / desempate
    struct s_request *next;
} t_request;

typedef struct s_heap {
    t_request **data;
    int size, capacity;
    t_scheduler type;
} t_heap;

typedef struct s_dongle {
    pthread_mutex_t mutex;
    pthread_cond_t  cond;
    bool available;
    long cooldown_end;
    int id;
    t_heap *heap;        // Fila de requisições por dongle
} t_dongle;

typedef struct s_coder {
    int id;
    pthread_t thread;
    int compile_count;
    long last_compile_start;
    long deadline;       // last_compile + time_to_burnout
    t_dongle *left_dongle, *right_dongle;
    struct s_simulation *sim;
} t_coder;

typedef struct s_simulation {
    int num_coders;
    long time_to_burnout, time_to_compile, time_to_debug, time_to_refactor;
    int compiles_required;
    long dongle_cooldown;
    t_scheduler scheduler;
    t_coder *coders;
    t_dongle *dongles;
    pthread_mutex_t log_mutex;
    pthread_mutex_t stop_mutex;
    pthread_cond_t  stop_cond;
    bool stop_simulation;
    long start_time;
    int finished_coders;
} t_simulation;
```

### 9.2 Fluxo de Comunicação entre Threads

```
┌─────────────────────────────────────────────────────────────────────┐
│                        SIMULAÇÃO (thread main)                      │
│  init_simulation() → start_coders() → start_monitor()              │
│                           │                    │                    │
│                           ▼                    ▼                    │
│  ┌─────────────────────────────────────────────────────────────┐   │
│  │ THREADS CODER (N threads)              THREAD MONITOR       │   │
│  │ coder_routine()                          monitor_routine()  │   │
│  │                                           │                 │   │
│  │ acquire_dongles() ──────────────────────► │ (verifica deadlines)│
│  │   wait_for_dongle()                       │                 │   │
│  │     lock dongle mutex                     │                 │   │
│  │     enqueue request                       │                 │   │
│  │     cond_wait / timed_wait                │                 │   │
│  │     acquire dongle                        │                 │   │
│  │ release_dongles()                         │                 │   │
│  │   set available=true                      │                 │   │
│  │   set cooldown_end                        │                 │   │
│  │   cond_signal                             │                 │   │
│  │                                           │                 │   │
│  │ log_state() ◄─────────────────────────────┤ (loga burnout)  │
│  │   lock log_mutex                          │                 │   │
│  │   printf                                  │                 │   │
│  │   unlock log_mutex                        │                 │   │
│  └─────────────────────────────────────────────────────────────┘   │
└─────────────────────────────────────────────────────────────────────┘
```

### 9.3 Análise de Seções Críticas

| Seção Crítica | Protegida Por | Tempo Máx. de Posse |
|---------------|---------------|---------------------|
| Estado do dongle (avail, cooldown, heap) | `dongle->mutex` | O(log n) ops heap |
| Saída de log | `sim->log_mutex` | Tempo do printf |
| Flag de parada | `sim->stop_mutex` | Poucas instruções |
| Atualização deadline coder | Nenhuma (thread-local) | N/A |

**Sem aninhamento de locks** entre mutex de dongle e mutex de parada — adquiridos em regiões de código separadas. Isso evita deadlock.

---

## 10. Tópicos Avançados para Estudo Adicional

### 10.1 Teoria de Escalonamento em Tempo Real
- Rate Monotonic (RM) vs. Deadline Monotonic (DM) — prioridade fixa
- Análise de Tempo de Resposta (RTA) para escalonamento FP
- EDF multiprocessador (partitioned, global, semi-partitioned)
- Servidores de escalonamento (deferrable, sporadic, constant bandwidth)

**Fontes**: **[R6] Cap. 7–10**, **[R12]**, **[R15]**

### 10.2 Programação Lock-Free & Wait-Free
- Operações atômicas (C11 `<stdatomic.h>`)
- Compare-and-swap (CAS) loops
- Ordenação de memória (acquire/release/seq_cst)
- Problema ABA, hazard pointers, epoch-based reclamation

**Fontes**: **[R4] Cap. 10–11**, **[R15]**, Michael & Scott (1996)

### 10.3 Verificação Formal
- Model checking (SPIN/Promela, TLA+)
- Provas de linearizabilidade
- Raciocínio rely-guarantee
- Método Owicki-Gries

**Fontes**: Baier & Katoen *Principles of Model Checking*, Lamport *Specifying Systems*

### 10.4 Engenharia de Performance
- False sharing (ping-pong de cache line)
- Colocação de threads NUMA-aware
- Perfilamento de contenção de locks (`perf`, `VTune`)
- Sincronização escalável (MCS locks, CLH locks)

**Fontes**: **[R1] Cap. 6**, McKenney *Is Parallel Programming Hard?*

---

## Checklist de Verificação

### Correção Funcional
- [ ] Coder único completa compiles requeridos sem burnout
- [ ] Múltiplos coders com FIFO — sem deadlock, ordenação justa
- [ ] Múltiplos coders com EDF — deadline mais cedo servido primeiro
- [ ] Burnout detectado e logado dentro de 10 ms do deadline
- [ ] Cooldown de dongle enforçado (sem re-aquisição imediata)
- [ ] Linhas de log nunca intercaladas (saída serializada)
- [ ] Simulação para quando todos coders atingem target de compile
- [ ] Simulação para no primeiro burnout

### Casos de Borda
- [ ] 1 coder (dongle único, auto-loop)
- [ ] 2 coders (configuração mínima propensa a deadlock)
- [ ] N grande (performance do heap, contenção)
- [ ] Cooldown zero
- [ ] Deadlines iguais (determinismo do desempate)
- [ ] `time_to_burnout < time_to_compile` (burnout imediato)

### Estresse & Robustez
- [ ] Alta contenção (muitos coders, tempos curtos)
- [ ] Simulação longa (estabilidade de memória, sem vazamentos)
- [ ] Criação/destruição rápida de threads
- [ ] Interrupção por sinal durante sleep (`nanosleep` retry)

### Conformidade Norminette (42 São Paulo)
- [ ] ≤ 5 funções por arquivo `.c`
- [ ] ≤ 25 linhas por função (incluindo declarações de variáveis)
- [ ] Sem variáveis globais
- [ ] Sem loops `for` (apenas `while`)
- [ ] Sem declarações de variáveis após primeira instrução
- [ ] Conformidade C89/C90 padrão

### Verificação com Ferramentas
```bash
# Compila com flags requeridas
make re

# Norminette
norminette Includes/ src/

# Segurança de memória
valgrind --leak-check=full --error-exitcode=1 ./codexion 3 10000 200 200 200 3 100 fifo

# Segurança de threads
valgrind --tool=helgrind --error-exitcode=1 ./codexion 3 10000 200 200 200 3 100 edf
```

---

## Apêndice: Referência Rápida — API POSIX Usada

| Função | Header | Propósito |
|--------|--------|-----------|
| `pthread_create` | `<pthread.h>` | Criar thread |
| `pthread_join` | `<pthread.h>` | Aguardar término de thread |
| `pthread_mutex_init` | `<pthread.h>` | Inicializar mutex |
| `pthread_mutex_lock` | `<pthread.h>` | Adquirir mutex (bloqueante) |
| `pthread_mutex_trylock` | `<pthread.h>` | Adquirir mutex (não-bloqueante) |
| `pthread_mutex_unlock` | `<pthread.h>` | Liberar mutex |
| `pthread_mutex_destroy` | `<pthread.h>` | Destruir mutex |
| `pthread_cond_init` | `<pthread.h>` | Inicializar variável de condição |
| `pthread_cond_wait` | `<pthread.h>` | Esperar em CV (destrava mutex) |
| `pthread_cond_timedwait` | `<pthread.h>` | Esperar em CV com timeout |
| `pthread_cond_signal` | `<pthread.h>` | Acordar um waiter |
| `pthread_cond_broadcast` | `<pthread.h>` | Acordar todos waiters |
| `pthread_cond_destroy` | `<pthread.h>` | Destruir CV |
| `gettimeofday` | `<sys/time.h>` | Tempo de parede (µs) |
| `nanosleep` | `<time.h>` | Sleep de alta resolução |
| `usleep` | `<unistd.h>` | Sleep em microssegundos |
| `malloc`/`free` | `<stdlib.h>` | Memória dinâmica |
| `printf`/`fprintf` | `<stdio.h>` | Saída formatada |
| `strcmp`/`strlen` | `<string.h>` | Operações de string |
| `atoi`/`atol` | `<stdlib.h>` | String para inteiro |
| `memset` | `<string.h>` | Preenchimento de memória |

---

*Este guia é companheiro do projeto Codexion. Maestria vem de implementar, quebrar e consertar — não apenas ler. Compile com `-Wall -Wextra -Werror -pthread`, rode sob `valgrind` e `helgrind`, e trace todo caminho de execução.*

**— Prof. Programação de Sistemas**