//
// Created by noah on 08.07.20.
//

#include <util/log.h>
#include <util/typedefs.h>
#include <parser/parser.h>
#include <elf/loadElf.h>
#include <common.h>
#include <stdbool.h>
#include <linux/mman.h>
#include <env/exit.h>


typedef struct {
    uint32_t imm1;
    uint32_t imm2;
    size_t count;
    struct node *next;
} t_node;

typedef struct {
    t_node *head;
    //t_node *tail;
    size_t count;
} t_list;

typedef struct {
    struct bitfield {
        unsigned char rd1EqRs11Flag: 1;
        unsigned char rd1EqRs21Flag: 1;
        unsigned char rs11EqRs21Flag: 1;
        unsigned char rd1EqRs12ORs22Flag: 1;
        unsigned char rd2EqRsx2Flag: 1;
        unsigned char rs12EqRs22Flag: 1;
    } flags;
    t_list immediateList;
} t_thirdLevel;

typedef struct {
    t_thirdLevel *arr[(1 << 6)];
    size_t count;
} t_secondLevel;

struct firstLevel {
    t_secondLevel *followingMnem[N_MNEM];
} array[N_MNEM];

// function to sort a singly linked list using insertion sort
void insertionSort(t_node **head);

void add_instruction(t_risc_addr addr, uint64_t *mnem_count, uint64_t *reg_count);

void analyze(const char *file_path) {
    log_analyze("Started file analysis...\n");

    if (file_path == NULL) {
        dprintf(2, "Bad. Invalid file path.\n");
        panic(FAIL_INVALID_PATH);
    }

    t_risc_elf_map_result result = mapIntoMemory(file_path);
    if (!result.valid) {
        dprintf(2, "Bad. Failed to map into memory.\n");
        panic(FAIL_MAP_FILE);
    }

    t_risc_addr startAddr = result.execStart;
    t_risc_addr endAddr = result.execEnd;

    //create array for mnemomics
    uint64_t mnem[N_MNEM];
    for (int i = 0; i < N_MNEM; i++) {
        mnem[i] = 0;
    }

    //create array for registers
    uint64_t reg[N_REG];
    for (int i = 0; i < N_REG; i++) {
        reg[i] = 0;
    }

    //loop over full segment
    for (t_risc_addr addr = startAddr; addr < endAddr; addr += 4) {
        add_instruction(addr, mnem, reg);
    }

    log_analyze("Done reading file.\n");

    if (flag_do_analyze_mnem) {
        ///rank mnem by usage
        int mnemRanked[N_MNEM];
        for (int i = 0; i < N_MNEM; i++) {
            mnemRanked[i] = i;
        }
        ///insertion sort:
        {
            int key, j;
            for (int i = 1; i < N_MNEM; i++) {
                key = mnemRanked[i];
                j = i - 1;

                ///move move elements with index < i && element > i one to the left
                while (j >= 0 && mnem[mnemRanked[j]] < mnem[key]) {
                    mnemRanked[j + 1] = mnemRanked[j];
                    j--;
                }

                ///insert former element i to correct position
                mnemRanked[j + 1] = key;
            }
        }

        log_analyze("Mnemonics...\n");
        log_analyze("==========\n");
        for (int i = 0; i < N_MNEM; i++) {
            if (mnem[mnemRanked[i]] == 0) break;
            log_analyze("Mnem %s is used %li times.\n", mnem_to_string(mnemRanked[i]), mnem[mnemRanked[i]]);
        }
        log_analyze("\n");
    }

    if (flag_do_analyze_reg) {
        //rank registers by usage
        int regRanked[N_REG];
        for (int i = 0; i < N_REG; i++) {
            regRanked[i] = i;
        }
        ///insertion sort:
        {
            int key, j;
            for (int i = 1; i < N_REG; i++) {
                key = regRanked[i];
                j = i - 1;

                ///move move elements with index < i && element > i one to the left
                while (j >= 0 && reg[regRanked[j]] < reg[key]) {
                    regRanked[j + 1] = regRanked[j];
                    j--;
                }

                ///insert former element i to correct position
                regRanked[j + 1] = key;
            }
        }

        log_analyze("Registers...\n");
        log_analyze("==========\n");
        for (int i = 0; i < N_REG; i++) {
            if (reg[regRanked[i]] == 0) break;
            log_analyze("Register %s (%s) is used %li times.\n", gp_to_string(regRanked[i]),
                        gp_to_alias(regRanked[i]),
                        reg[regRanked[i]]);
        }
        log_analyze("\n");
    }


    if (flag_do_analyze_pattern) {
        log_analyze("Combinations...\n");
        log_analyze("==========\n");

        long lvl2Count = 0;
        for (int i = 0; i < N_MNEM; ++i) {
            for (int j = 0; j < N_MNEM; ++j) {
                t_secondLevel *lvl2 = array[i].followingMnem[j];
                t_thirdLevel **lvl3 = lvl2->arr;
                if (lvl2 != NULL) {
                    lvl2Count++;
                    //Mnem combination i; j happened
                    int k = 1;
                    while (k < (1 << 6)) {
                        t_thirdLevel *x = lvl3[k];
                        if (x != NULL) {
                            insertionSort(&x->immediateList.head);
                            int l = k - 1;
                            while (l >= 0 &&
                                    (lvl3[l] == NULL || lvl3[l]->immediateList.count < x->immediateList.count)) {
                                lvl3[l + 1] = lvl3[l];
                                l--;
                            }
                            lvl3[l + 1] = x;
                        }
                        k++;
                    }
                }
            }
        }

        struct sortedlvl2 {
            t_secondLevel *lvl2;
            t_risc_mnem firstLvlMnem;
            t_risc_mnem secondLvlMnem;
        };
        struct sortedlvl2 sortedlvl2[lvl2Count];
        memset(sortedlvl2, 0, lvl2Count * sizeof(struct sortedlvl2));

        long k = 0;
        for (t_risc_mnem i = 0; i < N_MNEM; ++i) {
            for (t_risc_mnem j = 0; j < N_MNEM; ++j) {
                t_secondLevel *lvl2 = array[i].followingMnem[j];
                struct sortedlvl2 *A = sortedlvl2;
                if (lvl2 != NULL) {
                    //Mnem combination i; j happened

                    long l = k - 1;
                    while (l >= 0 && (A[l].lvl2 == NULL || A[l].lvl2->count < lvl2->count)) {
                        A[l + 1] = A[l];
                        l--;
                    }
                    A[l + 1] = (struct sortedlvl2) {.lvl2=lvl2, .firstLvlMnem=i, .secondLvlMnem=j};
                    k++;
                }
            }
        }


        for (int i = 0; i < lvl2Count; ++i) {
            if (i > 200) {
                log_analyze("List limited to first 200 combinations\n");
                break;
            }
            log_analyze("Mnemonic combination %s %s occured %li times. Of which:\n",
                        mnem_to_string(sortedlvl2[i].firstLvlMnem),
                        mnem_to_string(sortedlvl2[i].secondLvlMnem), sortedlvl2[i].lvl2->count);
            for (t_thirdLevel **arr = sortedlvl2[i].lvl2->arr; arr[0]; arr++) {
                struct bitfield flags = arr[0]->flags;
                log_analyze(
                        "\t\t%li had: rd_1 == rs1_1: %i; rd_1 == rs2_1: %i; rs1_1 == rs2_1: %i; rd_1 == rs1_2: %i; rd_2 == (rs1_2 || rs2_2): %i, rs1_2 == rs2_2: %i. The most used immediate values were: ",
                        arr[0]->immediateList.count, flags.rd1EqRs11Flag, flags.rd1EqRs21Flag, flags.rs11EqRs21Flag,
                        flags.rd1EqRs12ORs22Flag, flags.rd2EqRsx2Flag, flags.rs12EqRs22Flag);
                int j = 0;
                for (t_node *node = arr[0]->immediateList.head; node; node = (t_node *) node->next) {
                    printf("0x%x, 0x%x (%li times); ", node->imm1, node->imm2, node->count);
                    if (++j > 7) {
                        break;
                    }
                }
                printf("\n");
            }
        }
    }

    //check for unsupported instructions
    //CSRR
    //CSRRW, CSRRS, CSRRC, CSRRWI, CSRRSI, CSRRCI,
    bool usesCSRR = false;
    for (int i = CSRRW; i <= CSRRCI; i++) {
        if (mnem[i] != 0) {
            usesCSRR = true;
            break;
        }
    }

    //---RV32A---
    //LRW, SCW, AMOSWAPW, AMOADDW, AMOXORW, AMOANDW, AMOORW, AMOMINW, AMOMAXW, AMOMINUW, AMOMAXUW,
    bool usesRV32A = false;
    for (int i = LRW; i <= AMOMAXUW; i++) {
        if (mnem[i] != 0) {
            usesRV32A = true;
            break;
        }
    }

    //---RV64A---
    //LRD, SCD, AMOSWAPD, AMOADDD, AMOXORD, AMOANDD, AMOORD, AMOMIND, AMOMAXD, AMOMINUD, AMOMAXUD
    bool usesRV64A = false;
    for (int i = LRD; i <= AMOMAXUD; i++) {
        if (mnem[i] != 0) {
            usesRV64A = true;
            break;
        }
    }

    if (usesCSRR) log_analyze("Warning: Guest binary uses CSSR!\n");
    if (usesRV32A) log_analyze("Warning: Guest binary uses RV32A!\n");
    if (usesRV64A) log_analyze("Warning: Guest binary uses RV64A!\n");
}

t_risc_instr cur = {0};
t_risc_instr prev = {0};

t_node *nextFree = NULL;
t_node *lastOfAlloc = NULL;

t_node *getOrAllocateNode() {
    if (nextFree == NULL) {
        nextFree = mmap(NULL, 4096, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
        if (BAD_ADDR(nextFree)) {
            dprintf(2, "Memory allocation fault");
            panic(FAIL_HEAP_ALLOC);
        }
        lastOfAlloc = (t_node *) ((uintptr_t) nextFree + 4096);
    }
    t_node *ret = nextFree;
    nextFree++;

    if (nextFree + 1 > lastOfAlloc) {
        nextFree = NULL;
    }
    return ret;
}


void add_instruction(t_risc_addr addr, uint64_t *mnem_count, uint64_t *reg_count) {
    prev = cur;
    cur = (t_risc_instr) {0};
    cur.addr = addr;

    parse_instruction(&cur);

    //update statistics
    mnem_count[cur.mnem]++;
    reg_count[cur.reg_src_1]++;
    reg_count[cur.reg_src_2]++;
    reg_count[cur.reg_dest]++;

    if (prev.reg_dest == cur.reg_src_1 ||
            prev.reg_dest == cur.reg_src_2) {
        t_secondLevel **lvl2Ptr;
        lvl2Ptr = array[prev.mnem].followingMnem + cur.mnem;
        if (*lvl2Ptr == NULL) {
            *lvl2Ptr = mmap(NULL, sizeof(t_secondLevel), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
            if (BAD_ADDR(*lvl2Ptr)) {
                dprintf(2, "Memory allocation fault");
                panic(FAIL_HEAP_ALLOC);
            }
        }
        union flagUnion {
            unsigned char asChar;
            struct bitfield asBitField;
        } flags;
        flags.asBitField = (struct bitfield) {
                prev.reg_dest == prev.reg_src_1,
                prev.reg_dest == prev.reg_src_2,
                prev.reg_src_1 == prev.reg_src_2,
                prev.reg_dest == cur.reg_src_1,
                cur.reg_dest ==
                        (prev.reg_dest == cur.reg_src_1 ? cur.reg_src_1 :
                                cur.reg_src_2),
                cur.reg_src_1 == cur.reg_src_2};

        t_secondLevel *lvl2 = *lvl2Ptr;
        lvl2->count++;
        t_thirdLevel **lvl3ptr = lvl2->arr + flags.asChar;
        if (*lvl3ptr == NULL) {
            *lvl3ptr = mmap(NULL, sizeof(t_thirdLevel), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
            if (BAD_ADDR(*lvl3ptr)) {
                dprintf(2, "Memory allocation fault");
                panic(FAIL_HEAP_ALLOC);
            }
        }

        t_thirdLevel *lvl3 = *lvl3ptr;
        lvl3->immediateList.count++;
        lvl3->flags = flags.asBitField;
        t_node *curNode = lvl3->immediateList.head;
        t_node *prevNode = NULL;
        while (curNode != NULL &&
                (curNode->imm1 != (uint32_t) prev.imm || curNode->imm2 != (uint32_t) cur.imm)) {
            prevNode = curNode;
            curNode = (t_node *) curNode->next;
        }
        if (curNode == NULL) {
            curNode = getOrAllocateNode();
            if (lvl3->immediateList.head == NULL) {
                lvl3->immediateList.head = curNode;
            }
            if (prevNode != NULL) {
                prevNode->next = (struct node *) curNode;
            }

            curNode->imm1 = prev.imm;
            curNode->imm2 = cur.imm;
            curNode->count = 0;
            curNode->next = NULL;
        }
        curNode->count++;
    }


}

// Function to insert a given node in a sorted linked list
void sortedInsert(t_node **, t_node *);


/* function to insert a new_node in a list. Note that this
  function expects a pointer to head_ref as this can modify the
  head of the input linked list (similar to push())*/
void sortedInsert(t_node **head, t_node *new_node) {
    t_node *node;
    /* Special case for the head end */
    if (*head == NULL || (*head)->count <= new_node->count) {
        new_node->next = (struct node *) *head;
        *head = new_node;
    } else {
        /* Locate the node before the point of insertion */
        node = *head;
        while (node->next != NULL &&
                ((t_node *) node->next)->count > new_node->count) {
            node = (t_node *) node->next;
        }
        new_node->next = node->next;
        node->next = (struct node *) new_node;
    }
}

void insertionSort(t_node **head) {
    // Initialize sorted linked list
    t_node *sorted = NULL;

    // Traverse the given linked list and insert every
    // node to sorted
    t_node *node = *head;
    while (node != NULL) {
        // Store next for next iteration
        t_node *next = (t_node *) node->next;

        // insert current in sorted linked list
        sortedInsert(&sorted, node);

        // Update current
        node = next;
    }

    // Update head_ref to point to sorted linked list
    *head = sorted;
}
