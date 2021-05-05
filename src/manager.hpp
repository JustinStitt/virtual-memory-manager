#pragma once
#include <iostream>
#include <cstring>
#include <string>
#include "file.hpp"
#include "address.hpp"
#include "LRU_tlb.hpp"

#define PHYS_MEM_SIZE 65536
#define PAGE_TABLE_SIZE 256
#define PAGE_SIZE 256
#define FRAME_SIZE 256
#define SIMULATION_SIZE 1000 // number of lines in addresses.txt and correct.txt

class Manager{
private:
    SBYTE physical_memory[PHYS_MEM_SIZE];
    int current_frame;

    /* stores addressess at index mapping to physical memory */
    int page_table[PAGE_TABLE_SIZE]; 
    int page_first_open_idx;

    /* translation lookaside buffer */
    LRU_tlb tlb;

    File* addys, *correct, *bstore;
public:
    Manager() : current_frame(0), page_first_open_idx(0) {
        addys   = new File("../data/addresses.txt");
        correct = new File("../data/correct.txt");
        bstore  = new File("../data/BACKING_STORE.bin");

        memset(page_table, -1, sizeof(page_table));
    }

    /* Destructor to handle raw pointers --- RAII */
    ~Manager(){
        delete addys;
        delete correct;
        delete bstore;
    }

    /* get value of physical memory using virtual address */
    SBYTE getValue(int address){
        LogicalAddress la(address);
        /* get the page and the offset */
        BYTE page_number = la.getPage(), 
             offset      = la.getOffset();

        int physical_addy;

        /* Check tlb before consulting page table */
        bool tlb_hit = tlb.contains(page_number);

        if(tlb_hit){ // we found this result in the tlb!
            printf("TLB HIT!\n");
            physical_addy = tlb[page_number] + offset;
        }else{ // consult page table! (possible page fault!)
            printf("TLB MISS!\n");
            /* Check page table at index page_number */
            physical_addy = page_table[page_number];
            /* PAGE FAULT */
            if(physical_addy == -1){
                physical_addy = handlePageFault(page_number);
            }
            /* insert into tlb for future use */
            tlb.push(page_number, physical_addy);
            physical_addy += offset;
        }

        printf("Virtual address: %d Physical address: %d Value: %d ", 
                    address, physical_addy, physical_memory[physical_addy]);

        /* perform lookup on physical memory */
        return physical_memory[physical_addy];
    }

    /* When tlb miss and page table miss, page fault! */
    int handlePageFault(BYTE pn){
        printf("PAGE FAULT AT: %d\n", pn);

        // read in 256-byte page from bstore
        int pz = PAGE_SIZE;
        int new_physical_address = current_frame*FRAME_SIZE;

        for(int x{}; x < pz; ++x){
            physical_memory[new_physical_address + x] = (*bstore)[pn*pz + x];
        }

        page_table[pn] = new_physical_address;

        current_frame = (current_frame + 1) % FRAME_SIZE;
        return new_physical_address;
    }

    /* Run simulation on addresses.txt and check answers with correct.txt */
    void simulate(){
        int total{}, total_correct{};
        /* read in addressess and getValue() then compare to correct.txt */
        int value, cvalue;
        for(int x{}; x < SIMULATION_SIZE; ++x, ++total){
            value = this->getValue(addys->getAddress());
            cvalue = correct->parseValue();
            printf("Correct: %d\n", cvalue);
            // printf("correct: %d\n", cvalue);
            if(value == cvalue) ++total_correct;
        }
        float accuracy = (float)total_correct/(float)total;
        printf("----Accuracy: %.0f%%----", accuracy*100.0); 
        auto hits = tlb.getHits(), misses = tlb.getMisses();
        printf("TLB HITS: %d , TLB MISSES: %d , rate: %.0f%%\n", 
                        hits, misses, 
                            (float(hits)/float(hits + misses))*100);
    }

    /* -Debugging- LogicalAddress class */
    void test(int address){
        LogicalAddress la(address);
        printf("page: %d, offset: %d\n", 
                        la.getPage(), la.getOffset());
    }
};