#include <stddef.h>
#include <stdbool.h>
#include "print.h"

void reverse(char s[]){
	int c,i,j;
	for(i=0, j=strlen(s)-1;i<j;i++,j--){
		c=s[i];
		s[i]=s[j];
		s[j]=c;
	}
}

void int_to_ascii(int n, char str[]){
	int i, sign;
	if((sign=n)<0)	n=-n;
	i=0;
	do{
		str[i++]=n%10+'0';
	}while((n/=10)>0);
	if(sign <0)	str[i++]='-';
	str[i]='\0';
	reverse(str);	
}

int strlen(const char s[]){
	int i=0;
	while(s[i]!='\0')	i++;
	return i;	
}

int strcmp(const char str1[], const char str2[]){
    int i=0,flag=0;
   
    while(str1[i]!='\0' && str2[i]!='\0'){
         if(str1[i]!=str2[i]){
             flag=1;
             break;
         }
         i++;
    }

    if (flag==0 && str1[i]=='\0' && str2[i]=='\0')
         return 1;
    else
         return 0;
}
/*

int strcmp(char s1[], char s2[]) {
    int i;
    for (i = 0; s1[i] == s2[i]; i++) {
        if (s1[i] == '\0') return 0;
    }
    return s1[i] - s2[i];
}
*/
void sreverse(char str[], int length) {
    int start = 0;
    int end = length - 1;
    while (start < end) {
        char temp = str[start];
        str[start] = str[end];
        str[end] = temp;
        start++;
        end--;
    }
}
void concat(char* a, char* b, char* c){
	int len = strlen(a)+strlen(b);
	int i = 0;
	while(i < strlen(a)){
		c[i] = a[i];
		i++;
	}
	i = 0;
	while(i < strlen(b)){
		c[i+strlen(a)] = b[i];
		i++;
	}
	c[len] = '\0';
}

// Standard library memcpy with correct parameter order (dest, src, length)
void* memcpy_std(void* dest, const void* src, size_t n) {
	unsigned char* d = (unsigned char*)dest;
	const unsigned char* s = (const unsigned char*)src;
	for (size_t i = 0; i < n; i++) {
		d[i] = s[i];
	}
	return dest;
}

void concatc(char* a, char b, char* c){
	int len = strlen(a)+1;
	int i = 0;
	while(i < strlen(a)){
		c[i] = a[i];
		i++;
	}
	c[i] = b;
	c[len] = '\0';
}

void remchar(char* a, char* b){
	int i = 0;
	while(i < strlen(a)-1){
		b[i] = a[i];
		i++;
	}
	b[i] = '\0';
}

int toHex(char c){
	if(c == '0'){
		return 0x0;
	}if(c == '1'){
		return 0x1;
	}if(c == '2'){
		return 0x2;
	}if(c == '3'){
		return 0x3;
	}if(c == '4'){
		return 0x4;
	}if(c == '5'){
		return 0x5;
	}if(c == '6'){
		return 0x6;
	}if(c == '7'){
		return 0x7;
	}if(c == '8'){
		return 0x8;
	}if(c == '9'){
		return 0x9;
	}if(c == 'a' || c == 'A'){
		return 0xa;
	}if(c == 'b' || c == 'B'){
		return 0xb;
	}if(c == 'c' || c == 'C'){
		return 0xc;
	}if(c == 'd' || c == 'D'){
		return 0xd;
	}if(c == 'e' || c == 'E'){
		return 0xe;
	}if(c == 'f' || c == 'F'){
		return 0xf;
	}
	return -1;
}

int strncmp(const char* a, const char* b, int l){
   int c = 0;
   
   while (a[c] == b[c] && c < l) {
      c++;
   }
 
   if (c == l)
      return 1;
   else
      return 0;
}

int startswith(char* a, char* b){
	return strncmp(a,b,strlen(b));
}

void strcpy(char a[], char b[]){
	int i;
	for(i = 0; i < strlen(a); i++){
		b[i] = a[i];
	}
	b[i] = '\0';
}

char tolower(char a){
	if(a >= 'A' && a <= 'Z')
		return a+32;
	return a;
}
void int_to_string(int value, char* str) {
    int i = 0;
    int is_negative = 0;

    if (value == 0) {
        str[i++] = '0';
        str[i] = '\0';
        return;
    }

    if (value < 0) {
        is_negative = 1;
        value = -value;
    }

    while (value != 0) {
        int digit = value % 10;
        str[i++] = digit + '0';
        value /= 10;
    }

    if (is_negative) {
        str[i++] = '-';
    }

    str[i] = '\0';

    // Omkeren
    for (int j = 0; j < i / 2; j++) {
        char temp = str[j];
        str[j] = str[i - j - 1];
        str[i - j - 1] = temp;
    }
}

void strtolower(char a[], char b[]){
	int i;	
	for(i = 0; i < strlen(a); i++){
		b[i] = tolower(a[i]);
	}
}

void subchar(char a[], char b[], int index){
	int i = index;
	while(i < strlen(a)){
		b[i-index] = a[i];
		i++;
	}
	b[i-index] = '\0';
}

void append(char s[], char n){
	int len=strlen(s);
	s[len]=n;
	s[len+1]='\0';
}

void backspace(char s[]){
	int len=strlen(s);
	s[len-1]='\0';
}

void memcpy(char* src, char* dst, int nbytes){
	int i;
	for(i=0;i<nbytes;i++)
	{
		*(dst+i)=*(src+i);
	}
}



void* memset(void* dest, int val, size_t len) {
    unsigned char* ptr = dest;
    while (len-- > 0) {
        *ptr++ = (unsigned char)val;
    }
    return dest;
}


int memcmp(const void *ptr1, const void *ptr2, size_t num) {
    unsigned char *p1 = (unsigned char *)ptr1;
    unsigned char *p2 = (unsigned char *)ptr2;
    
    for (size_t i = 0; i < num; i++) {
        if (p1[i] != p2[i]) {
            return p1[i] - p2[i];
        }
    }
    return 0;  // De blokken zijn gelijk
}
char* itoa(int num, char* str, int base) {
    int i = 0;
    bool isNegative = false;

    // Handle 0 explicitly, otherwise empty string is printed
    if (num == 0) {
        str[i++] = '0';
        str[i] = '\0';  // null-terminate string
        return str;
    }

    // Handle negative numbers only if base is 10
    if (num < 0 && base == 10) {
        isNegative = true;
        num = -num;  // Make the number positive
    }

    // Process individual digits
    while (num != 0) {
        int rem = num % base;
        str[i++] = (rem > 9) ? (rem - 10) + 'a' : rem + '0';
        num = num / base;
    }

    // Append negative sign for negative numbers
    if (isNegative) {
        str[i++] = '-';
    }

    str[i] = '\0';  // Null-terminate string

    // Reverse the string
    sreverse(str, i);

    return str;
}

void print_int(int top) {
	char temp[100];
	itoa(top, temp, 10);
	print((char*)temp);
}
void strncpy(char* dest, const char* src, int n) {
    int i = 0;
    while (i < n && src[i] != '\0') {
        dest[i] = src[i];
        i++;
    }
    while (i < n) {
        dest[i++] = '\0'; // vul de rest met nullen
    }
}
char* strchr(const char* str, int c) {
    while (*str) {
        if (*str == (char)c) {
            return (char*)str;
        }
        str++;
    }
    return NULL;
}
char* strrchr(const char* str, int c) {
    char* last = NULL;
    while (*str) {
        if (*str == (char)c) {
            last = (char*)str;
        }
        str++;
    }
    return last;
}

char* strtok(char* str, const char* delim) {
    static char* last = NULL;

    if (str) {
        last = str;
    } else if (!last) {
        return NULL;
    }

    // Skip leading delimiters
    while (*last && strchr(delim, *last)) {
        last++;
    }

    if (!*last) {
        return NULL;
    }

    char* token_start = last;

    // Zoek einde van token
    while (*last && !strchr(delim, *last)) {
        last++;
    }

    if (*last) {
        *last = '\0';
        last++;
    } else {
        last = NULL;
    }

    return token_start;
}
char* strcat(char* dest, const char* src) {
    char* d = dest;
    while (*d) d++;           // Ga naar het einde van dest
    while (*src) *d++ = *src++; // Kopieer src naar dest
    *d = '\0';                // Sluit af met null-terminator
    return dest;
}
char* strstr(const char* haystack, const char* needle) {
    if (!*needle) return (char*)haystack;  // lege needle matcht altijd

    for (; *haystack; haystack++) {
        const char* h = haystack;
        const char* n = needle;

        while (*h && *n && *h == *n) {
            h++;
            n++;
        }

        if (!*n) return (char*)haystack;  // volledige match gevonden
    }

    return NULL;  // geen match
}
char* strncat(char* dest, const char* src, size_t n) {
    // Zoek naar het einde van de string dest
    char* dest_ptr = dest;
    while (*dest_ptr != '\0') {
        dest_ptr++;
    }

    // Voeg maximaal n tekens toe van src aan dest
    while (n > 0 && *src != '\0') {
        *dest_ptr = *src;
        dest_ptr++;
        src++;
        n--;
    }

    // Voeg de null-terminator toe aan het einde van dest
    *dest_ptr = '\0';

    return dest;
}
