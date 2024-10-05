#include <include/constants.h>
int string_to_int(char* str) {
    int result = 0;
    int sign = 1; 

    if (*str == '-') {
        sign = -1;
        str++;
    }
    // HEX
    if (*str == '0' && (*(str + 1) == 'x' || *(str + 1) == 'X')) {
        str += 2;  

        while (*str != '\0') {
            if (*str >= '0' && *str <= '9') {
                result = result * 16 + (*str - '0');
            } else if (*str >= 'a' && *str <= 'f') {
                result = result * 16 + (*str - 'a' + 10);
            } else if (*str >= 'A' && *str <= 'F') {
                result = result * 16 + (*str - 'A' + 10);
            } else {
                break;
            }
            str++;
        }
    } else {
        // NORMAL
        while (*str != '\0') {
            if (*str >= '0' && *str <= '9') {
                result = result * 10 + (*str - '0');
            } else {
                break;
            }
            str++;
        }
    }

    result *= sign;

    return result;
}

void int_to_str(int num, char *str) {
    int i = 0;
    int is_negative = 0;
    
    if (num < 0) {
        is_negative = 1;
        num = -num;  
    }
    
    // Handle special case of 0
    if (num == 0) {
        str[i++] = '0';
    }
    else {
        
        while (num != 0) {
            int digit = num % 10;  
            str[i++] = digit + '0';  
            num = num / 10;  
        }
        
        if (is_negative) {
            str[i++] = '-';
        }
        
        int start = 0;
        int end = i - 1;
        while (start < end) {
            char temp = str[start];
            str[start] = str[end];
            str[end] = temp;
            start++;
            end--;
        }
    }
    
    str[i] = '\0';
}

void sprint_int(int theint){
    char output[512];
    int_to_str(theint, output);
    sprint(output, white);
}
