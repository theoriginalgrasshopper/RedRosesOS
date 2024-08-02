#include "random.h"
#include "convert_to_int.h"
#include <sprint.h>
#include <include/constants.h>
#include "timer.h"
#include <stdint.h>

uint32_t hardware_random() {
    return clock_ticks ^ seconds ^ (clock_ticks >> 3) ^ (seconds << 5);
}

int give_random(int first_num, int second_num) {
    int range = second_num - first_num + 1;

    uint32_t random_num = hardware_random();
    random_num ^= (random_num << 14);
    random_num ^= (random_num >> 12);
    random_num ^= (random_num << 9);

    random_num = (random_num % range) + first_num;

    if (random_num < first_num || random_num > second_num) {
        random_num = first_num + (random_num % range);
    }
    if (random_num < 0) {
        random_num = -random_num;
    }
    return(random_num);
}