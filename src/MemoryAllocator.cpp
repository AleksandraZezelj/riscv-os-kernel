#include "../h/MemoryAllocator.hpp"

BlockHeader* MemoryAllocator::freeMemHead = nullptr;

void MemoryAllocator::initMemoryAllocator() {
    freeMemHead = (BlockHeader*)HEAP_START_ADDR;
    size_t size = (size_t)HEAP_END_ADDR - (size_t)HEAP_START_ADDR;

    if (size < sizeof(BlockHeader)) {
        freeMemHead = nullptr;
        return;
    }
    freeMemHead->size = size;
    freeMemHead->next = freeMemHead->prev = nullptr;
}

void* MemoryAllocator::malloc(size_t size) {

    if (size <= 0 ) return nullptr;


    if (freeMemHead == nullptr) {
        initMemoryAllocator();
    }

    size_t newBlockSize = size + sizeof(BlockHeader);
    if ((newBlockSize % MEM_BLOCK_SIZE) != 0) {
        newBlockSize = ((newBlockSize + MEM_BLOCK_SIZE-1)/MEM_BLOCK_SIZE)*MEM_BLOCK_SIZE;
    }

    BlockHeader* curr = freeMemHead;

    while (curr != nullptr) {
        if (curr->size>= newBlockSize) {
            break;
        }
        curr = curr->next;
    }
    // nema dovoljno memorije
    if (curr == nullptr) { return nullptr; }

    size_t remainingSize = curr->size - newBlockSize;

    // secenje bloka samo ako preostali deo moze bar zaglavlje da zadrzi
    if (remainingSize >= sizeof(BlockHeader) + MEM_BLOCK_SIZE) {
        BlockHeader* newFreeBlock= (BlockHeader*)((char*)curr + newBlockSize);
        newFreeBlock->size = remainingSize;

        // uvezujemo novi slobodni blok umesto trenutnog (curr)
        newFreeBlock->next = curr->next;
        newFreeBlock->prev = curr->prev;

        if (curr->prev != nullptr) {
            curr->prev->next = newFreeBlock;
        }else {
            // Ako je curr bio glava liste
            freeMemHead = newFreeBlock;
        }
        if (curr->next != nullptr) {
            curr->next->prev = newFreeBlock;
        }

        curr->size = newBlockSize;
    }else {
        // Ako je ostatak bio premali uzimamo ceo blok nema secenja
        if (curr->prev != nullptr) {
            curr->prev->next = curr->next;
        } else {
            freeMemHead = curr->next;
        }

        if (curr->next != nullptr) {
            curr->next->prev = curr->prev;
        }

    }
    curr->next = curr->prev = nullptr;

    // Korisniku vracamo adresu odmah iza zaglavlja
    return (void*)((char*)curr + sizeof(BlockHeader));

}

void MemoryAllocator::free(void* ptr) {
    if (ptr == nullptr) { return; }

    // Vracamo se unazad za velicinu zaglavlja da dohvatimo BlockHeader
    BlockHeader* header = (BlockHeader*)((char*)ptr - sizeof(BlockHeader));

    // vec je u listi
    if (header->prev != nullptr || header->next != nullptr) {
        return;
    }

    // Ako je lista bila prazna sve je bilo zauzeto
    if (freeMemHead == nullptr) {
        freeMemHead = header;
        header->next = header->prev = nullptr;
        return;
    }

    // Trazimo mesto u listi (sortirana je po adresama)
    BlockHeader* curr = freeMemHead;
    BlockHeader* prev = nullptr;
    while (curr != nullptr && curr < header) {
        prev = curr;
        curr = curr->next;
    }

    // Umetanje izmedju prev i curr
    header->next = curr;
    header->prev = prev;

    if (prev != nullptr) {
        prev->next = header;
    }
    else {
        freeMemHead = header;
    }

    if (curr != nullptr) {
        curr->prev = header;
    }

    //Spajanje: probaj da spojis header sa desnim komsijom (curr)
    if (curr != nullptr) {
        concatenate(header, curr);
    }

    //Spajanje: probaj da spojis levog komsiju (prev) sa header
    if (prev != nullptr) {
        concatenate(prev, header);
    }

}

void MemoryAllocator::concatenate(BlockHeader* first, BlockHeader* second) {
    if (first == nullptr || second == nullptr || (first == second)) { return; }

    if (((char*)first + first->size) == (char*)second) {

        first->next = second->next;
        if (second->next != nullptr) {
            second->next->prev = first;
        }
        first->size = first->size + second->size;
        second->prev = second->next = nullptr;
    }

}

