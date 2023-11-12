#include <stdint.h>

void loadAllTests();
void testProcess(char ** params);
void testSync(char ** params);
void testMM(char** params);
void testPriority(char ** params);

//BEGIN TEST MM
#define MAX_BLOCKS 100

typedef struct MM_rq {
  void *address;
  uint32_t size;
} mm_rq;

void test_mm(uint64_t argc, char *argv[]);
//END TEST MM

//BEGIN TEST SYNC
#define SEM_ID "sem"
#define TOTAL_PAIR_PROCESSES 2
void slowInc(int64_t *p, int64_t inc);
void my_process_inc(uint64_t argc, char *argv[]);
void test_sync(uint64_t argc, char *argv[]);
//END TEST SYNC 

//BEGIN TEST PROCESSES
enum State { RUN,
             BLOCK,
             KILLED };

typedef struct P_rq {
  int32_t pid;
  enum State state;
} p_rq;

void endless_loop();
void test_processes(uint64_t argc, char *argv[]);
//END TEST PROCESSES

//BEGIN TEST PRIORITIES
#define MINOR_WAIT 1000000
#define WAIT 90000000 

#define TOTAL_PROCESSES 3
#define LOWEST 0  
#define MEDIUM 1  
#define HIGHEST 4 

void test_prio();
//END TEST PRIORITIES