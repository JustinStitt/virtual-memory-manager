#pragma once
#include <iostream>
#include <cstring>
#include <string>
#include "file.hpp"
#include "address.hpp"

#define PHYS_MEM_SIZE 65536
#define TLB_ENTRIES 16
#define PAGE_TABLE_SIZE 256
#define PAGE_SIZE 256
#define FRAME_SIZE 256

class Manager{
private:
    SBYTE physical_memory[PHYS_MEM_SIZE]; // 65536 bytes of physical memory
    int current_frame;

    int page_table[PAGE_TABLE_SIZE]; // stores addressess at index mapping to physical memory
    int page_first_open_idx;

    int tlb[TLB_ENTRIES][2];
    int tlb_idx;

    File* addys, *correct, *bstore;
public:
    Manager() : current_frame(0), tlb_idx(0), 
                        page_first_open_idx(0) {
        addys   = new File("../data/addresses.txt");
        correct = new File("../data/correct.txt");
        bstore  = new File("../data/BACKING_STORE.bin");

        memset(page_table, -1, sizeof(page_table));
    }

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
        /* Check page table at index page_number */
        int physical_addy = page_table[page_number];

        /* PAGE FAULT */
        if(physical_addy == -1){
            physical_addy = handlePageFault(page_number);
        }
        physical_addy += offset;

        printf("Virtual address: %d Physical address: %d Value: %d\n", 
                    address, physical_addy, physical_memory[physical_addy]);

        /* perform lookup on physical memory */
        return physical_memory[physical_addy];
    }

    int handlePageFault(BYTE pn){
        printf("PAGE FAULT AT: %d\n", pn);

        // read in 256-byte page from bstore
        int pz = PAGE_SIZE;
        int new_physical_address = current_frame*FRAME_SIZE;

        for(int x{}; x < pz; ++x){
            physical_memory[new_physical_address + x] = (*bstore)[pn*pz + x];
        }

        page_table[pn] = new_physical_address;

        current_frame = (current_frame + 1) % pz;
        return new_physical_address;
    }

    void simulate(){
        int total{}, total_correct{};
        /* read in addressess and getValue() then compare to correct.txt */
        int value, cvalue;
        for(int x{}; x < 30; ++x, ++total){
            value = this->getValue(addys->getAddress());
            cvalue = correct->parseValue();
            // printf("correct: %d\n", cvalue);
            if(value == cvalue) ++total_correct;
        }
        float accuracy = (float)total_correct/(float)total;
        printf("----Accuracy: %.0f%%----", accuracy*100.0); 
    }

    void test(int address){
        LogicalAddress la(address);
        printf("page: %d, offset: %d\n", la.getPage(), la.getOffset());
    }
};