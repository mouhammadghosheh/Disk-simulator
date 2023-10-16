# Disk-simulator
Disk Simulator


==Description==
This code simulates a file System simulator, it has a file that identifies as the disk,
the disk stores its files using the indexed allocation method, which stores for each file
an index block that points accordingly to its file in the disk, the disk is
initialized with empty characters, and given a disk size, it creates the file with that
many free bits. In the simulator, we must keep track of how many free and used blocks we have
so the simulator knows where to put file contents in the block accordingly, we use the
bit vector size for that just like we learnt in indexed allocation. the data structures
in the simulator are 3 parts, fsFIle: stores info about the file according to the indexed allocation method
FileDescriptor: this class stores info about the file descriptor and tells us if it is
in use and what is the filename for the fd
fsDisk: stores information about the disk we created, and initializes the bit vector and has all the functions
we will use which I will mention below

functions:
getter and setter functions in classes:so we can access their properties

private:
	int get_fd(): this function finds the empty fd in the open files 
descriptor and returns the number
(it iterates through the map)
	int get_block(): this function finds the next empty place in the bit
vector which also indicates to the next
empty block to write in
	int Block_Write(int index, char* buf, int len): this function
takes the index block to use it to write
buf in the right place according to len
	int Block_Read(int index, char* buf, int len):this function recieves
an index to read from that block so we
use what we read later in other functions
	fsFormat(blocksize): this function formats the disk,
it iterates through the map of the main directory and deletes everything and
clears it, it also initializes some
important information about the disk 
such as maxsize
	int CreateFile(string filename): this fucntion checks some cases like if the file is already formatted and if the filename does not already exist, if true then it initilizes the file and gives it some properties like it is in use and inserts it to the main dir map and to the open fds
	int OpenFile(string filename):
this function also checks some case handling, and checks if the file exists and it is not in use (closed) then it opens it and returns it's fd
	string CloseFile(int fd): if file is available in the open fds then return -1, this function sets in use to false(closes the file) and returns the file name.
	int WriteToFile(int fd,char* buf, int len): checks same cases (most importantly if file is open!!!) then does 
some operation of read to know what blocks are empty to write in and uses private functions for this, it also checks the buffer for it's length and if
we need an offset to write more in more
blocks and returns -1 if fails
	int DelFile(string FileName): deletes the file and replaces it's 
blocks with '\0', we keep reading and writing until all of the file is 
emptied then we remove it from maindir
	int ReadFromFile(int fd, char *buf, int len): checks for cases if false then return -1, else, counts how many blocks we must access in order to read
the asked string and then call the Block_read function and keep giving it
our indexes and offset until we read len characters, this function returns the 
amount of characters that were read.
	


==Program Files==
main.cpp, main file
Makefile-to compile the program.
==How to compile?==
g++ main.cpp -o final
./final

==Input:==
0-exit
1-listAll
2-formats the disk
3-creates a file
4-opens file
5-closes file
6-write file
7-reads file
8-deletes file

==Output:==
according to the input
