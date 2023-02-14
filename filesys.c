// ACADEMIC INTEGRITY PLEDGE
//
// - I have not used source code obtained from another student nor
//   any other unauthorized source, either modified or unmodified.
//
// - All source code and documentation used in my program is either
//   my original work or was derived by me from the source code
//   published in the textbook for this course or presented in
//   class.
//
// - I have not discussed coding details about this project with
//   anyone other than my instructor. I understand that I may discuss
//   the concepts of this program with other students and that another
//   student may help me debug my program so long as neither of us
//   writes anything during the discussion or modifies any computer
//   file during the discussion.
//
// - I have violated neither the spirit nor letter of these restrictions.
//
//
//
// Signed: Rhea Huber Date: 12/2/2022

//filesys.c
//Based on a program by Michael Black, 2007
//Revised 11.3.2020 O'Neil

#include <stdio.h>
#include <stddef.h>

int main(int argc, char* argv[])
{
	int i, j, size, noSecs, startPos;

	//open the floppy image
	FILE* floppy;
	floppy=fopen("floppya.img","r+");
	if (floppy==0)
	{
		printf("floppya.img not found\n");
		return 0;
	}

	//load the disk map from sector 256
	char map[512];
	fseek(floppy,512*256,SEEK_SET);
	for(i=0; i<512; i++)
		map[i]=fgetc(floppy);

	//load the directory from sector 257
	char dir[512];
	fseek(floppy,512*257,SEEK_SET);
	for (i=0; i<512; i++)
		dir[i]=fgetc(floppy);

	if (argc == 1) {
		//print disk map
		printf("Disk usage map:\n");
		printf("      0 1 2 3 4 5 6 7 8 9 A B C D E F\n");
		printf("     --------------------------------\n");
		for (i=0; i<16; i++) {
			switch(i) {
				case 15: printf("0xF_ "); break;
				case 14: printf("0xE_ "); break;
				case 13: printf("0xD_ "); break;
				case 12: printf("0xC_ "); break;
				case 11: printf("0xB_ "); break;
				case 10: printf("0xA_ "); break;
				default: printf("0x%d_ ", i); break;
			}
			for (j=0; j<16; j++) {
				if (map[16*i+j]==-1) printf(" X"); else printf(" .");
			}
			printf("\n");
		}

		// print directory
		printf("\nDisk directory:\n");
		printf("Name    Type Start Length\n");
		for (i=0; i<512; i=i+16) {
			if (dir[i]==0) continue;
			for (j=0; j<8; j++) {
				if (dir[i+j]==0) printf(" "); else printf("%c",dir[i+j]);
			}
			if ((dir[i+8]=='t') || (dir[i+8]=='T')) printf("text"); else printf("exec");
			printf(" %5d %6d bytes\n", dir[i+9], 512*dir[i+10]);
		}
	}
	else if (argc > 3) {
		//Too many arguments for any function
		printf("Error: Too many arguments\n");
		fclose(floppy);
		return 0;
	}
	else if (*argv[1] == 'L' || *argv[1] == 'l') {
		//List function selected
		if (argc > 2) {
			//Extra arguments given for list function
			printf("Error: Too many arguments for function L\n");
			fclose(floppy);
			return 0;
		}
		//Print file names and sizes
		int bytesUsed = 0;
		for (i=0; i<512; i=i+16) {
			if (dir[i]==0) continue;
			int spaces = 0;
			for (j=0; j<8; j++) {
				if (dir[i+j]==0) ++spaces; else printf("%c",dir[i+j]);
			}
			if ((dir[i+8]=='t') || (dir[i+8]=='T')) printf(".t"); else printf(".x");
			for (; spaces > 0; --spaces) {
				printf(" ");
			}
			printf(" %6d bytes\n", 512*dir[i+10]);
			bytesUsed += 512 * dir[i+10];
		}
		//Print space used and unused
		printf("\nTotal disk space used: %6d bytes\n", bytesUsed);
		printf("Free space remaining:  %6d bytes\n", (511 * 512) - bytesUsed);
	}
	else if (*argv[1] == 'P' || *argv[1] == 'p') {
		//Print function selected
		if (argc < 3) {
			printf("Error: Function P requires a file name\n");
			fclose(floppy);
			return 0;
		}
		//Find file with provided name
		int nameFound = -1;
		for (i=0; i<512; i=i+16) {
			if (dir[i]==0) {
				printf("Error: File not found\n");
				fclose(floppy);
				return 0;
			}
			for (j=0; j<8; j++) {
				if (dir[i+j]==0 && ( argv[2][j] == '.' || argv[2][j] == '\0')) {
					//Match with name < 8 characters
					nameFound = i;
				} 
				else if (dir[i+j] != argv[2][j]) {
					//Not matching
					break;
				}
				else if (j == 7) {
					//First 8 characters match
					nameFound = i;
				}
			}
			if (nameFound >= 0) break;
		}
		//Check file extension
		if (dir[nameFound+8] != 't' && dir[nameFound+8] != 'T') {
			printf("Error: Executable file cannot be printed\n");
			fclose(floppy);
			return 0;
		}
		startPos = dir[nameFound+9];
		noSecs = dir[nameFound+10];

		//Load the file into a buffer
		char buffer[512*noSecs];
		fseek(floppy,512*startPos,SEEK_SET);
		for(i=0; i<512*noSecs; i++) {
			buffer[i]=fgetc(floppy);
		}
		//Print the buffer contents
		for(i=0; i<512*noSecs; i++) {
			if (buffer[i] == 0) break;
			printf("%c", buffer[i]);
		}
		printf("\n");
	}
	else if (*argv[1] == 'M' || *argv[1] == 'm') {
		//Create function selected
		if (argc < 3) {
			printf("Error: Function M requires a file name\n");
			fclose(floppy);
			return 0;
		}
		//Take user input
		char buffer[512];
		printf("Please input the text contents of your file:\n\n");
		scanf("%[^\n]", buffer);
		//Find directory location for file
		int spaceFound = -1;
		for (i=0; i<512; i=i+16) {
			if (dir[i]==0) {
				spaceFound = i;
				break;
			}
			for (j=0; j<8; j++) {
				if (dir[i+j]==0 && (argv[2][j] == '.' || argv[2][j] == '\0')) {
					//Match with name < 8 characters
					printf("Error: Filename already in use\n");
					fclose(floppy);
					return 0;
				} 
				else if (dir[i+j] != argv[2][j]) {
					//Not matching
					break;
				}
				else if (j == 7) {
					//First 8 characters match
					printf("Error: Filename already in use\n");
					fclose(floppy);
					return 0;
				}
			}
		}
		//Write name in directory entry
		for (j=0; j<8; j++) {
			if (argv[2][j] && argv[2][j] != '.' && argv[2][j-1] != '.') dir[spaceFound + j] = argv[2][j];
			else break;
		}
		for (; j<8; j++) {
			dir[spaceFound + j] = 0;
		}
		dir[spaceFound + 8] = 't';
		//Find free sector
		startPos = -1;
		for (i=0; i < 512; i++) {
			if (map[i] == 0) {
				startPos = i;
				map[i] = -1;
				break;
			}
		}
		if (startPos == -1) {
			printf("Error: Disk is full\n");
			fclose(floppy);
			return 0;
		}
		dir[spaceFound + 9] = startPos;
		dir[spaceFound + 10] = 1;

		//Write user input to the file
		fseek(floppy,512*startPos,SEEK_SET);
		for (i=0; i<512; i++) {
			fputc(buffer[i],floppy);
		}
		//write the map and directory back to the floppy image
		fseek(floppy,512*256,SEEK_SET);
		for (i=0; i<512; i++) fputc(map[i],floppy);

		fseek(floppy,512*257,SEEK_SET);
		for (i=0; i<512; i++) fputc(dir[i],floppy);
	}
	else if (*argv[1] == 'D' || *argv[2] == 'd') {
		//Delete function selected
		if (argc < 3) {
			printf("Error: Function D requires a file name\n");
			fclose(floppy);
			return 0;
		}
		//Find file with provided name
		int nameFound = -1;
		for (i=0; i<512; i=i+16) {
			if (dir[i]==0) {
				printf("Error: File not found\n");
				fclose(floppy);
				return 0;
			}
			for (j=0; j<8; j++) {
				if (dir[i+j]==0 && ( argv[2][j] == '.' || argv[2][j] == '\0')) {
					//Match with name < 8 characters
					nameFound = i;
				} 
				else if (dir[i+j] != argv[2][j]) {
					//Not matching
					break;
				}
				else if (j == 7) {
					//First 8 characters match
					nameFound = i;
				}
			}
			if (nameFound >= 0) break;
		}
		//Remove references to this file
		startPos = dir[nameFound+9];
		noSecs = dir[nameFound+10];
		dir[nameFound] = 0;
		for (i=startPos; i < (startPos + noSecs); i++) {
			map[i] = 0;
		}

		//write the map and directory back to the floppy image
		fseek(floppy,512*256,SEEK_SET);
		for (i=0; i<512; i++) fputc(map[i],floppy);

		fseek(floppy,512*257,SEEK_SET);
		for (i=0; i<512; i++) fputc(dir[i],floppy);
	}
	else {
		printf("Error: Function not recognized\n");
	}

	fclose(floppy);
	return 0;
}
