#include "../include/stringutils.h"
#include <stddef.h>
void str_to_upper(char *s){
	if (s == NULL) return;

	for(int i = 0; s[i] !='\0'; i++){
		if(s[i] >= 'a' && s[i] <= 'z'){
			s[i]-=32;
		}
	}
}

int str_count(const char *s){
	int cnt = 0;
	if(s == NULL) return 0;

	for(int i = 0;s[i] != '\0'; i++){
		cnt++;
	}
	return cnt;
}

void str_reverse(char *s){
	if(s == NULL) return;

	int  cnt = str_count(s);

	for(int i = 0;i < cnt/2; i++){
		char x = s[i];
		s[i] = s[cnt - i - 1];
		s[cnt - i - 1] = x;
	}
}
