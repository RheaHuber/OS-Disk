Directions for using filesys
A simple program demonstrating a solution to the 'filesys' semaphore problem

COMPILE AND RUN (with make and g++):
1. In the directory containing "filesys.c" and "Makefile", use the command 'make'
2. In the directory containing "filesys", use the command './filesys'

COMPILE AND RUN (with g++, without make):
1. In the directory containing "smokers.c", use the command 'gcc -o filesys filesys.c'
2. In the directory containing "filesys", use the command './filesys'

HOW TO USE:
The program has 5 functions which are run by command-line arguments

INCLUDED FUNCTIONS:

NAME        | FUNCTION               | DESCRIPTION

Read Image  | ./filesys              | Prints out the image map and disk directory
List Files  | ./filesys L            | Lists all stored files and free space remaining on disk
Print File  | ./filesys P [filename] | If the file is found and printable, prints the file contents
Create File | ./filesys M [filename] | If the filename is not already in use and disk space is available, requests a string and stores it as a text file with the given name
Delete File | ./filesys D [filename] | If the file is found, deletes it from the disk
