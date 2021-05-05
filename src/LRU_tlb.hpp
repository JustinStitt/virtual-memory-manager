#pragma once
#include <unordered_map>
#include <queue>

#define TLB_SIZE 16
/*
    Least-Recently Used Translation Lookaside Buffer
    Check with tlb before page table.
    page numbers not referenced recently will be tossed out.
    maintains a size N <= TLB_SIZE.
*/
class LRU_tlb{
private:
    std::unordered_map<int, int> tlb;
    using HashItr = std::unordered_map<int, int>::iterator;
    std::queue<int> lru_cache;

    /* stat-tracking */
    int tlb_hits, tlb_misses;
public:
    LRU_tlb() : tlb_hits(0), tlb_misses(0) { }

    /* check if we have this page number stored in tlb */
    bool contains(int pn){
        auto f = tlb.find(pn);
        if(f == tlb.end()){ ++tlb_misses; return false; }
        ++tlb_hits;
        return true;
    }

    /* add to tlb */
    void push(int pn, int phys_addy){
        printf("TLB INSERTION\n");
        if(tlb.size() > TLB_SIZE){
            /* least recently used in tlb */
            auto lru = lru_cache.front();
            lru_cache.pop();
            tlb.erase(lru);
        }
        tlb[pn] = phys_addy;
        lru_cache.push(pn);
    }

    /* access value from key (hash table lookup) */
    decltype(tlb)::mapped_type operator[](int pn){
        return tlb[pn];
    }

    decltype(tlb_hits) getHits(){ return this->tlb_hits; }
    decltype(tlb_misses) getMisses(){ return this->tlb_misses; }
};