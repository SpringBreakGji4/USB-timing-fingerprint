#include <stdio.h>
#include <stdlib.h>
#include "func.c"

int main(int argc, char** argv){
	char* device_path = argv[1];
	printf("device path: %s\n",device_path);
	char* file_name = argv[2];
	printf("timing fingerprint file name: %s\n",file_name);
	char tem[10];
	create_fingerprint_filename(device_path, file_name);
	/*for(int i=0 ; i<100 ; i++){
		char s[50] = "timing";
		memset(tem, 0, sizeof(tem));
		sprintf(tem, "%d",i);
		strcat(s, tem);
		strcat(s, ".txt");
		create_fingerprint_filename(device_path, s);
	}*/
	return 0;
}
