#include <bits/stdc++.h>
using namespace std;

/*
Memory Management Simulator
Implements:
- First Fit
- Best Fit
- Worst Fit
- Dynamic allocation & deallocation
- Coalescing
- Fragmentation statistics
*/

enum AllocatorType { FIRST_FIT, BEST_FIT, WORST_FIT };

struct Block {
    int start;      // starting address
    int size;       // block size
    bool free;      // free or allocated
    int id;         // block id (-1 if free)
};

/* ---------------- GLOBAL STATE ---------------- */
vector<Block> memory;
int MEM_SIZE = 0;
int next_id = 1;
AllocatorType alloc_strategy = FIRST_FIT;

/* Statistics */
int alloc_requests = 0;
int alloc_success = 0;
int alloc_failure = 0;

/* ---------------- INIT ---------------- */
void init_memory(int size) {
    MEM_SIZE = size;
    memory.clear();
    memory.push_back({0, size, true, -1});
    next_id = 1;

    // Reset statistics
    alloc_requests = 0;
    alloc_success = 0;
    alloc_failure = 0;

    cout << "Memory initialized with size " << size << "\n";
}

/* ---------------- ALLOCATION ---------------- */
int malloc_mem(int size) {
    alloc_requests++;

    if (size <= 0) {
        cout << "Invalid allocation size\n";
        alloc_failure++;
        return -1;
    }

    int idx = -1;

    if (alloc_strategy == FIRST_FIT) {
        for (int i = 0; i < memory.size(); i++)
            if (memory[i].free && memory[i].size >= size) {
                idx = i;
                break;
            }
    }
    else if (alloc_strategy == BEST_FIT) {
        int best = INT_MAX;
        for (int i = 0; i < memory.size(); i++)
            if (memory[i].free && memory[i].size >= size && memory[i].size < best) {
                best = memory[i].size;
                idx = i;
            }
    }
    else { // WORST_FIT
        int worst = -1;
        for (int i = 0; i < memory.size(); i++)
            if (memory[i].free && memory[i].size >= size && memory[i].size > worst) {
                worst = memory[i].size;
                idx = i;
            }
    }

    if (idx == -1) {
        cout << "Allocation failed\n";
        alloc_failure++;
        return -1;
    }

    int addr = memory[idx].start;
    int remaining = memory[idx].size - size;

    // Allocate block
    memory[idx].size = size;
    memory[idx].free = false;
    memory[idx].id = next_id++;

    // Split remaining free block
    if (remaining > 0) {
        memory.insert(
            memory.begin() + idx + 1,
            {addr + size, remaining, true, -1}
        );
    }

    alloc_success++;

    cout << "Allocated block id=" << memory[idx].id
         << " at address=0x" << hex << addr << dec << "\n";

    return memory[idx].id;
}

/* ---------------- FREE ---------------- */
void free_mem(int id) {
    for (int i = 0; i < memory.size(); i++) {
        if (!memory[i].free && memory[i].id == id) {
            memory[i].free = true;
            memory[i].id = -1;

            // Merge with next block
            if (i + 1 < memory.size() && memory[i + 1].free) {
                memory[i].size += memory[i + 1].size;
                memory.erase(memory.begin() + i + 1);
            }

            // Merge with previous block
            if (i - 1 >= 0 && memory[i - 1].free) {
                memory[i - 1].size += memory[i].size;
                memory.erase(memory.begin() + i);
            }

            cout << "Block " << id << " freed and merged\n";
            return;
        }
    }
    cout << "Invalid block id\n";
}

/* ---------------- DUMP ---------------- */
void dump_memory() {
    cout << "Memory layout:\n";
    for (auto &b : memory) {
        cout << "[0x" << hex << b.start
             << " - 0x" << (b.start + b.size - 1) << dec << "] ";
        if (b.free) cout << "FREE\n";
        else cout << "USED (id=" << b.id << ")\n";
    }
}

/* ---------------- STATS ---------------- */
void stats() {
    int used = 0, free = 0, largest_free = 0;

    for (auto &b : memory) {
        if (b.free) {
            free += b.size;
            largest_free = max(largest_free, b.size);
        } else {
            used += b.size;
        }
    }

    double ext_frag = (free == 0) ? 0.0 :
        100.0 * (free - largest_free) / free;

    double utilization = (MEM_SIZE == 0) ? 0.0 :
        100.0 * used / MEM_SIZE;

    // Internal fragmentation is zero because blocks are split exactly
    int internal_frag = 0;

    cout << "\n--- Memory Statistics ---\n";
    cout << "Total memory: " << MEM_SIZE << "\n";
    cout << "Used memory: " << used << "\n";
    cout << "Free memory: " << free << "\n";
    cout << "Memory utilization: " << utilization << "%\n";
    cout << "Internal fragmentation: " << internal_frag << "\n";
    cout << "External fragmentation: " << ext_frag << "%\n";
    cout << "Allocation requests: " << alloc_requests << "\n";
    cout << "Successful allocations: " << alloc_success << "\n";
    cout << "Failed allocations: " << alloc_failure << "\n";
    cout << "--------------------------\n";
}

/* ---------------- CLI ---------------- */
int main() {
    string cmd;
    cout << "memsim started\n";

    while (true) {
        cout << "> ";
        cin >> cmd;

        if (cmd == "init") {
            int size; cin >> size;
            init_memory(size);
        }
        else if (cmd == "set") {
            string t; cin >> t;
            if (t == "first_fit") alloc_strategy = FIRST_FIT;
            else if (t == "best_fit") alloc_strategy = BEST_FIT;
            else if (t == "worst_fit") alloc_strategy = WORST_FIT;
            else cout << "Unknown allocator\n";
        }
        else if (cmd == "malloc") {
            int s; cin >> s;
            malloc_mem(s);
        }
        else if (cmd == "free") {
            int id; cin >> id;
            free_mem(id);
        }
        else if (cmd == "dump") dump_memory();
        else if (cmd == "stats") stats();
        else if (cmd == "exit") break;
        else cout << "Unknown command\n";
    }
}
