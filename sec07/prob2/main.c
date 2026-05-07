#define _GNU_SOURCE
#include <pwd.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <dirent.h>
#include <string.h>
#include <errno.h>
#include <sys/time.h>
#include <pthread.h>
#include <stdint.h>

const size_t MALLOC_ALIGNMENT = 16;
const size_t MALLOC_METADATA_SIZE = 16; // sizeof(malloc_element)は8だが、16バイト境界に丸める

const size_t MIN_SBRK_SIZE = 128 * 1024; // 128KB以上解放できる場合、sbrk()を実行する

typedef struct malloc_element {
    size_t size;
} malloc_element;

// 最小サイズでも、MALLOC_ALIGNMENT+MALLOC_METADATA_SIZE=32バイトであるため、free_element分のサイズが常にある
typedef struct free_element {
    size_t size;
    struct free_element *prev;
    struct free_element *next;
} free_element;

free_element *free_list = NULL;

size_t align_size(size_t size) {
    return (size + MALLOC_ALIGNMENT - 1) & ~((size_t)MALLOC_ALIGNMENT - 1);
}

size_t minimum_free_element_size() {
    return MALLOC_METADATA_SIZE + MALLOC_ALIGNMENT;
}

// リストの先頭にelementを追加する
void add_free_element(free_element *new_free_element) {
    new_free_element->prev = NULL;
    new_free_element->next = free_list;
    if (free_list != NULL) {
        free_list->prev = new_free_element;
    }
    free_list = new_free_element;
}

// currentをsize分だけ残して分割する
// 分割後のfree_elementをfree_listに追加する
// 返り値:
// 0: 分割できなかった
// 1: 分割できた
int split_free_element(free_element *current, size_t size) {
    size_t remaining_size = current->size - size;

    // 残りが最小サイズより小さい場合は分割しない
    if (remaining_size < minimum_free_element_size()) {
        return 0;
    }

    free_element *new_free_element =
        (free_element *)((char *)current + MALLOC_METADATA_SIZE + size);

    new_free_element->size = remaining_size - MALLOC_METADATA_SIZE;
    add_free_element(new_free_element);

    current->size = size;
    return 1;
}

// currentをfree_listから削除する
void remove_free_element(free_element *current) {
    if (current->prev != NULL) {
        current->prev->next = current->next;
    } else {
        free_list = current->next;
    }

    if (current->next != NULL) {
        current->next->prev = current->prev;
    }
    current->prev = NULL;
    current->next = NULL;
}



// first-fit
// 仮定:
//    - my_mallocとmy_free以外でヒープは操作されない
//    - 最初のprogram breakは16バイト境界にアラインされている
void *my_malloc(size_t size) {

    if (size == 0) {
        size = 1; // 0の場合は最小のメモリ領域を確保(TLPI p147)
    }

    // GCCのマニュアルによると、malloc関数やrealloc関数で割り付けられるメモリのアドレスは、32bitシステムでは8の倍数に、64bitシステムでは16の倍数になるように割り付けられるそうです。つまり、アライメントされています。
    // 引用元: https://note.com/ymzo76/n/na6b335bf3010
    // 参考: https://www5d.biglobe.ne.jp/~noocyte/Programming/MallocAlignment.html
    // アラインする必要があるのはアドレスだけだが、実装を簡単にするためサイズ自体を丸める
    size = align_size(size);

    // free_list を走査して、size 以上の空き領域を探す
    free_element *current = free_list;
    while (current != NULL) {
        if (current->size >= size) {
            split_free_element(current, size);
            remove_free_element(current);

            // currentからMALLOC_METADATA_SIZEバイト進んだ値を返す
            return (char *)current + MALLOC_METADATA_SIZE;
        }
        current = current->next;
    }
    // 空き領域が見つからなかった場合は、sbrk()で新しい領域を割り当てる
    void *ptr = sbrk(size + MALLOC_METADATA_SIZE);
    if (ptr == (void *)-1) {
        perror("sbrk");
        return NULL;
    }
    // 新しい領域を free_list に追加
    malloc_element *new_element = (malloc_element *)ptr;
    new_element->size = size;
    // ptrからMALLOC_METADATA_SIZEバイト進んだ値を返す
    return (char *)ptr + MALLOC_METADATA_SIZE;
}

char *free_element_end(free_element *element) {
    return (char *)element + MALLOC_METADATA_SIZE + element->size;
}

int is_left_adjacent(free_element *left, free_element *right) {
    return free_element_end(left) == (char *)right;
}

free_element *_do_coalesce_free_elements(free_element *element_in_list_1,free_element *element_in_list_2) {
    if (is_left_adjacent(element_in_list_1, element_in_list_2)) {
        element_in_list_1->size += MALLOC_METADATA_SIZE + element_in_list_2->size;
        remove_free_element(element_in_list_2);
        return element_in_list_1;
    } else if (is_left_adjacent(element_in_list_2, element_in_list_1)) {
        element_in_list_2->size += MALLOC_METADATA_SIZE + element_in_list_1->size;
        remove_free_element(element_in_list_1);
        return element_in_list_2;
    }
    // 隣接していない場合はNULLを返す
    return NULL;
} 


free_element *coalesce_free_element(free_element *new_free_element_in_list) {
    free_element *current = free_list;

    while (current != NULL) {
        free_element *next = current->next;

        if (current != new_free_element_in_list) {
            if (is_left_adjacent(current, new_free_element_in_list) || is_left_adjacent(new_free_element_in_list, current)) {
                new_free_element_in_list = _do_coalesce_free_elements(current, new_free_element_in_list);
                new_free_element_in_list = coalesce_free_element(new_free_element_in_list);
                return new_free_element_in_list;
            }
        }

        current = next;
    }
    return new_free_element_in_list;
}

void my_free(void *ptr) {
    if (ptr == NULL) {
        return;
    }

    free_element *element = (free_element *)((char *)ptr - MALLOC_METADATA_SIZE);

    add_free_element(element);

    element = coalesce_free_element(element);

    if (free_element_end(element) != (char *)sbrk(0) || element->size+MALLOC_METADATA_SIZE < MIN_SBRK_SIZE) {
        return;
    }
    intptr_t sbrk_size = element->size + MALLOC_METADATA_SIZE;
    remove_free_element(element);
    sbrk(-sbrk_size);
}

void print_free_list(const char *label, void *base_brk) {
    void *current_brk = sbrk(0);

    printf("\n=== %s ===\n", label);
    printf("base sbrk(0):    %p\n", base_brk);
    printf("current sbrk(0): %p\n", current_brk);
    printf("heap diff:       %lld bytes\n",
           (long long)((uintptr_t)current_brk - (uintptr_t)base_brk));

    printf("free_list:\n");

    free_element *current = free_list;
    int index = 0;

    while (current != NULL) {
        printf("  [%d] addr=%p, end=%p, size=%zu, total=%zu, prev=%p, next=%p\n",
               index,
               (void *)current,
               (void *)free_element_end(current),
               current->size,
               current->size + MALLOC_METADATA_SIZE,
               (void *)current->prev,
               (void *)current->next);

        current = current->next;
        index++;
    }

    if (index == 0) {
        printf("  empty\n");
    }
}

int main(int argc, char *argv[]) {
    (void)argc;
    (void)argv;

    // printf が内部でmallocを使うらしい。そのため、最初にmallocを使っておく。
    printf("warmup...\n");

    void *base_brk = sbrk(0);

    print_free_list("initial", base_brk);

    /*
     * 64KB にしている理由:
     * 追加3だけ free しても 128KB 未満なので sbrk() で返却されない。
     * その後、追加2を free すると、追加2 + 追加3 が結合されて
     * 128KB 以上になるため、ヒープ末尾なら sbrk() で返却される。
     */
    size_t alloc_size = 64 * 1024;

    void *p1 = my_malloc(alloc_size);
    printf("\n追加1: p1=%p\n", p1);
    print_free_list("after malloc p1", base_brk);

    void *p2 = my_malloc(alloc_size);
    printf("\n追加2: p2=%p\n", p2);
    print_free_list("after malloc p2", base_brk);

    void *p3 = my_malloc(alloc_size);
    printf("\n追加3: p3=%p\n", p3);
    print_free_list("after malloc p3", base_brk);

    printf("\n削除3: free(p3)\n");
    my_free(p3);
    print_free_list("after free p3", base_brk);

    printf("\n削除2: free(p2)\n");
    my_free(p2);
    print_free_list("after free p2", base_brk);

    printf("\n削除1: free(p1)\n");
    my_free(p1);
    print_free_list("after free p1", base_brk);

    void *p4 = my_malloc(alloc_size/2);
    printf("\n追加4: p4=%p\n", p4);
    print_free_list("after malloc p4", base_brk);    

    return 0;
}