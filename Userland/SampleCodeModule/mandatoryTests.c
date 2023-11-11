#include <mandatoryTests.h>
#include <shell.h>
#include <library.h>

void loadAllTests(){
    loadModule("testPriority", "Parameters: none", &testPriority,0);
    loadModule("test_prio", "Runs actual test", &test_prio, 0);
    loadModule("testMM", "Parameters: <max storage>", &testMM, 1);
    loadModule("test_mm", "Runs test mm", &test_mm, 0);
    loadModule("testSync", "Parameters: <number of iterations> <sync(1)/async(0)>", &testSync, 2);
    loadModule("test_sync", "Runs test sync", &test_sync, 1);
    loadModule("testProcesses", "Parameters: <number of processes>", &testProcess,0);
    loadModule("test_process", "Runs actual test", &test_processes,0);
    loadModule("my_process_inc", "Increments a global variable", &my_process_inc, 3);
    loadModule("endless_loop", "Runs endless loop", &endless_loop,0);
    loadModule("bussy_wait", "Does busy waiting", &bussy_wait, 1);
    loadModule("endless_loop_print", "Runs endless loop with print", &endless_loop_print, 1);
}

//---------------------------------------------WRAPPERS---------------------------------------------//

void testProcess(char ** params){
    char * *argvExec;
    argvExec = (char**) malloc(5 * sizeof(char *));
    for (int i = 0; i < 5; i++){
        argvExec[i] = (char*) malloc(3 * sizeof(char));
    }
    int index = getIndexModule("test_process");
    numToStr(index, 10, argvExec[0]);
    memcpy(argvExec[1], (strcmp(params[1], "&")) ? "0": "1", 1);
    memcpy(argvExec[2], params[0], strlen(params[0]));
    execveNew(argvExec);
    waitChildren();
}

void testSync(char ** params){
    char * *argvExec;
    argvExec = (char**) malloc(5 * sizeof(char *));
    for (int i = 0; i < 5; i++){
        argvExec[i] = (char*) malloc(40 * sizeof(char));
    }
    int index = getIndexModule("test_sync");
    numToStr(index, 10, argvExec[0]);
    memcpy(argvExec[1], (strcmp(params[2], "&")) ? "0": "1", 1);
    memcpy(argvExec[2], params[0], strlen(params[0]));
    memcpy(argvExec[3], params[1], strlen(params[1]));
    execveNew(argvExec);
}

void testMM(char** params){
    char * *argvExec;
    argvExec = (char**) malloc(5 * sizeof(char *));
    for (int i = 0; i < 3; i++){
        argvExec[i] = (char*) malloc(15 * sizeof(char));
    }
    int index = getIndexModule("test_mm");
    numToStr(index, 10, argvExec[0]);
    memcpy(argvExec[1], (strcmp(params[1], "&")) ? "0": "1", 1);
    memcpy(argvExec[2], params[0], strlen(params[0]));
    execveNew(argvExec);
}

void testPriority(char** params){
    char * *argvExec;
    argvExec = (char**) malloc(5 * sizeof(char *));
    for (int i = 0; i < 3; i++){
        argvExec[i] = (char*) malloc(15 * sizeof(char));
    }
    int index = getIndexModule("test_prio");
    numToStr(index, 10, argvExec[0]);
    memcpy(argvExec[1], (params != 1 && strcmp(params[0], "&")) ? "0": "1", 1);
    execveNew(argvExec);
}

//---------------------------------------------TEST MM---------------------------------------------//

uint64_t test_mm(uint64_t argc, char *argv[]) {
    mm_rq mm_rqs[MAX_BLOCKS];
    uint8_t rq;
    uint32_t total;
    uint64_t max_memory;
    uint8_t test;
    test = 1;

    if (argc != 1){
        exit();
        return -1;
    }

    if ((max_memory = satoi(argv[1])) <= 0){
        exit();
        return -1;
    }

    while (1) {
        rq = 0;
        total = 0;
        bussy_wait(WAIT);
        printf("\n --------------------------TEST %d-----------------------\n", test++);
        // Request as many blocks as we can
        while (rq < MAX_BLOCKS && total < max_memory) {
            mm_rqs[rq].size = GetUniform(max_memory - total - 1) + 1;
            mm_rqs[rq].address = syscall_malloc(mm_rqs[rq].size);
        
            if (mm_rqs[rq].address) {
                total += mm_rqs[rq].size;
                bussy_wait(WAIT);
                printf("################# malloc size %d ####################### total storage used: %d / %d\n", mm_rqs[rq].size, total, max_memory);
                rq++;
            }
        }
        bussy_wait(WAIT);
        printf("se pudieron crear %d bloques dado un maximo de %d bloques\n", rq, MAX_BLOCKS);
        // Set
        uint32_t i;
        for (i = 0; i < rq; i++)
            if (mm_rqs[i].address)
                memset(mm_rqs[i].address, i, mm_rqs[i].size);

        bussy_wait(WAIT);
        printf("se setean los bloques con el size que ocupa c/u\n");
        // Check
        for (i = 0; i < rq; i++)
        if (mm_rqs[i].address)
            if (!memcheck(mm_rqs[i].address, i, mm_rqs[i].size)) {
            printf("test_mm ERROR\n");
            return -1;
            }

        bussy_wait(WAIT);
        printf("ahora se los libera\n");
        // Free
        for (i = 0; i < rq; i++)
        if (mm_rqs[i].address)
            syscall_free(mm_rqs[i].address);
        
        bussy_wait(WAIT);
        printf("\n --------------------TEST %d EXITOSO-------------------\n", test++);
    }
}

//---------------------------------------------TEST SYNC---------------------------------------------//
int64_t global; // shared memory

void slowInc(int64_t *p, int64_t inc) {
    uint64_t aux = *p;
    my_yield(); // This makes the race condition highly probable
    aux += inc;
    *p = aux;
}

uint64_t my_process_inc(uint64_t argc, char *argv[]) {
    uint64_t n ;
    int8_t inc;
    int8_t use_sem;
    
    if (argc != 3)
        return -1;

    if ((n = satoi(argv[1])) <= 0)
        return -1;
    if ((inc = satoi(argv[2])) == 0)
        return -1;
    if ((use_sem = satoi(argv[3])) < 0)
        return -1;
    
    printf("entre al proceso\n");
    if (use_sem)
        if (my_sem_open(SEM_ID, 1) == -1) {
        printf("test_sync: ERROR opening semaphore\n");
        exit();
        }

    uint64_t i;
    for (i = 0; i < n; i++) {
        if (use_sem)
            my_sem_wait(SEM_ID);
        slowInc(&global, inc);
        if (use_sem)
            my_sem_post(SEM_ID);
    }

    if (use_sem)
        my_sem_close(SEM_ID);
    
    printf("salio todo ok\n");
    exit();
}

uint64_t test_sync(uint64_t argc, char *argv[]) { //{n, use_sem, 0}
    uint64_t pids[2 * TOTAL_PAIR_PROCESSES];

    
    if (argc != 2)
        return -1;
        
    char **argvDec = malloc(4 * sizeof(char *));
    argvDec[0] = "my_process_inc";
    argvDec[1] = argv[1];
    argvDec[2] = "-1";
    argvDec[3] = argv[2];
    char **argvInc = malloc(4 * sizeof(char *));
    argvInc[0] = "my_process_inc";
    argvInc[1] = argv[1];
    argvInc[2] = "1";
    argvInc[3] = argv[2];


    global = 0;
    uint64_t i;
    for (i = 0; i < TOTAL_PAIR_PROCESSES; i++) {
        pids[i] = my_create_process("my_process_inc", 3, argvDec);
        pids[i + TOTAL_PAIR_PROCESSES] = my_create_process("my_process_inc", 3, argvInc);
    }

    
    for (i = 0; i < TOTAL_PAIR_PROCESSES; i++) {
        my_wait(pids[i]);
        my_wait(pids[i + TOTAL_PAIR_PROCESSES]);
    }
    
    printf("Final value: %d\n", global);

    exit();
}

//---------------------------------------------TEST PROCESSES---------------------------------------------//

void endless_loop() {
    while (1)
    ;
}

int64_t test_processes(uint64_t argc, char *argv[]) {
  uint8_t rq;
  uint8_t alive = 0;
  uint8_t action;
  uint64_t max_processes = 4;
  char *argvAux[] = {"endless_loop",0};

  if (argc != 1){
    exit();
    return -1;
  }

 if ((max_processes = satoi(argv[1])) <= 0){
    exit();
    return -1; 
 }

  p_rq p_rqs[max_processes];
  int test = 0;

  while (1) {
    if (test){
        printf("-----------------------TEST %d EXITOSO---------------------\n", test);
    }
    
    printf("-----------------------PREPARANDO TEST %d---------------------\n", ++test);
    // Create max_processes processes
    for (rq = 0; rq < max_processes; rq++) {
        p_rqs[rq].pid = my_create_process("endless_loop", 0, argvAux);
        bussy_wait(WAIT);

        if (p_rqs[rq].pid == -1) {
        printf("test_processes: ERROR creating process\n");
        exit();
        return -1;
        } else {
        printf("Se creo el proceso con pid: %d\n", p_rqs[rq].pid);
        p_rqs[rq].state = RUN;
        alive++;
        }
    }
    printf("\n --------------------------TEST %d-----------------------\n", test);
    // Randomly kills, blocks or unblocks processes until every one has been killed
    while (alive > 0) {

      for (rq = 0; rq < max_processes; rq++) {
        action = GetUniform(100) % 2;

        switch (action) {
          case 0:
            if (p_rqs[rq].state == RUN|| p_rqs[rq].state == BLOCK) {
              if (my_kill(p_rqs[rq].pid) == -1) {
                printf("test_processes: ERROR killing process\n");
                exit();//agregado para asegurarse que muere
                return -1;
              }
              bussy_wait(WAIT);
              printf("murio el proceso con pid: %d\n", p_rqs[rq].pid);
              p_rqs[rq].state = KILLED;
              alive--;
            }
            break;

          case 1:
            if (p_rqs[rq].state == RUN) {
              if (my_block(p_rqs[rq].pid) == -1) {
                printf("test_processes: ERROR blocking process\n");
                exit();
                return -1;
              }
              bussy_wait(WAIT);
              printf("bloqueo el proceso con pid: %d\n", p_rqs[rq].pid);
              p_rqs[rq].state = BLOCK;
            }
            break;
        }
      }

      // Randomly unblocks processes
      for (rq = 0; rq < max_processes; rq++)
        if (p_rqs[rq].state == BLOCK && GetUniform(100) % 2) {
          if (my_unblock(p_rqs[rq].pid) == -1) {
            printf("test_processes: ERROR unblocking process\n");
            exit();
            return -1;
          }
          bussy_wait(WAIT);
          printf("desbloqueo el proceso con pid: %d\n", p_rqs[rq].pid);
          p_rqs[rq].state = RUN;
        }
    }
  }
  exit();
}

//---------------------------------------------TEST PRIORITIES---------------------------------------------//
int64_t prio[TOTAL_PROCESSES] = {LOWEST, MEDIUM, HIGHEST};

void bussy_wait(uint64_t n) {
    uint64_t i;
    for (i = 0; i < n; i++)
        ;
}

void endless_loop_print(uint64_t wait) {
    int64_t pid = my_getpid();

    while (1) {
        printf("%d ", pid);
        bussy_wait(wait);
    }
}

void test_prio() {
    int64_t pids[TOTAL_PROCESSES];
    char *argv[] = {"endless_loop_print",100000, 0};
    uint64_t i;

    for (i = 0; i < TOTAL_PROCESSES; i++)
        pids[i] = my_create_process("endless_loop_print", 1, argv);

    bussy_wait(WAIT);
    printf("\nCHANGING PRIORITIES...\n");

    for (i = 0; i < TOTAL_PROCESSES; i++)
        my_nice(pids[i], prio[i]);

    bussy_wait(WAIT);
    printf("\nBLOCKING...\n");

    for (i = 0; i < TOTAL_PROCESSES; i++)
        my_block(pids[i]);

    printf("CHANGING PRIORITIES WHILE BLOCKED...\n");

    for (i = 0; i < TOTAL_PROCESSES; i++)
        my_nice(pids[i], MEDIUM);

    printf("UNBLOCKING...\n");

    for (i = 0; i < TOTAL_PROCESSES; i++)
        my_unblock(pids[i]);

    bussy_wait(WAIT);
    printf("\nKILLING...\n");

    for (i = 0; i < TOTAL_PROCESSES; i++)
        my_kill(pids[i]);
    
    exit();
}