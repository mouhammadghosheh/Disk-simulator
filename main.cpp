#include <iostream>
#include <vector>
#include <map>
#include <assert.h>
#include <string.h>
#include <math.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

using namespace std;

#define DISK_SIZE 256
int MAXSIZE = 0;
int BLOCKSIZE = 0;
// ============================================================================
class FsFile {

    int file_size;
    int block_in_use;
    int index_block;
    int block_size;

    public:
    FsFile(int _block_size) {
        file_size = 0;
        block_in_use = 0;
        block_size = _block_size;
        index_block = -1;
    }
    //getters and setters for use
    int getfile_size(){
        return file_size;
    }
    void set_file_size(int filesize){
        file_size = filesize;
    }
    int get_block_in_use(){
        return block_in_use;
    }
    void set_block_inuse(int blockinuse){
        block_in_use = blockinuse;
    }
    int get_block_size(){
        return block_size;
    }
    int get_index_block(){
        return index_block;
    }
    void set_index_block(int indexblock){
        index_block = indexblock;
    }

};
// ============================================================================
class FileDescriptor {

    string file_name;
    FsFile* fs_file;
    bool inUse;

    public:

    FileDescriptor(string FileName, FsFile* fsi) {
        file_name = FileName;
        fs_file = fsi;
        inUse = true;
    } //getters and setters for use
    string getname() {
        return file_name;
    }
    FsFile* get_fs_file(){
        return fs_file;
    }
    bool get_in_use(){
        return inUse;
    }
    void set_in_use(bool in_use){
        inUse = in_use;
    }
};
#define DISK_SIM_FILE "DISK_SIM_FILE.txt"
// ============================================================================
class fsDisk {
    FILE *sim_disk_fd;
    bool is_formated;

    // BitVector - "bit" (int) vector, indicate which block in the disk is free
	//              or not.  (i.e. if BitVector[0] == 1 , means that the
	//             first block is occupied.
    int BitVectorSize;
    int *BitVector;

    map <string,FileDescriptor*> MainDir;
    // Structure that links the file name to its FsFile

    map <int,FileDescriptor*> OpenFileDescrifileptors;

    //  when you open a file,
    // the operating system creates an entry to represent that file
    // This entry number is the file descriptor.

    // ------------------------------------------------------------------------
private:
    int get_fd(){ //this function gets the empty fd in OpenFileDescriptors
        int i = 0;
        int fd = OpenFileDescrifileptors.size();
        for(map<int,FileDescriptor*>::iterator it = OpenFileDescrifileptors.begin() ; it != OpenFileDescrifileptors.end() ; ++it){ //iterator to run through map
            if(it->first > i){
                fd = i;
                break;
            }
            i++;
        }
        return fd;

    }
    int get_block(){ //this function returns the next emptyblock according to the bit vector
        for(int i = 0 ; i < BitVectorSize ; i++){
            if(BitVector[i] == 0){
                BitVector[i] = 1;
                return i;
            }
        }
        return -1;
    }

    //this function writes to the specified block and returns how many chars were written
    int Block_Write(int index , int offset , char* buf , int len){
        int return_val = 0;
        fseek(sim_disk_fd,index*BLOCKSIZE+offset,SEEK_SET);
        for(int i = 0 ; i < len && i < BLOCKSIZE-offset; i++){
            fwrite(&buf[i],1,1,sim_disk_fd);
            return_val++;
        }
        return return_val;
    }
    //this function reads from the block specified and returns how many chars were read
    int Block_Read(int index , int offset , char* buf , int len){
        int return_val = 0;
        fseek(sim_disk_fd,index*BLOCKSIZE+offset,SEEK_SET);
        for(int i = 0 ; i < len && i < BLOCKSIZE-offset; i++){
            fread(&buf[i],1,1,sim_disk_fd);
            return_val++;
        }
        return return_val;
    };

    public:
    fsDisk() {
        sim_disk_fd = fopen(DISK_SIM_FILE , "w+");
        assert(sim_disk_fd);
        for (int i=0; i < DISK_SIZE ; i++) {
            int ret_val = fseek ( sim_disk_fd , i , SEEK_SET );
            ret_val = fwrite( "\0" ,  1 , 1, sim_disk_fd);
            assert(ret_val == 1);
        }
        fflush(sim_disk_fd);
        is_formated = false;
    }
    // ------------------------------------------------------------------------
    ~fsDisk(){ //destructor to free memory and allocations

        for(map<string,FileDescriptor*>::iterator it = MainDir.begin(); it != MainDir.end() ; ++it){
            delete it->second->get_fs_file();
            delete it->second;
        }
        MainDir.clear();
        OpenFileDescrifileptors.clear();
        delete[] BitVector;
        fclose(sim_disk_fd);
    }
    // ------------------------------------------------------------------------
    void listAll() {
        int i = 0;
        for (map<string,FileDescriptor*>::iterator it = MainDir.begin(); it != MainDir.end();++it) {
            cout << "index: " << i << ": FileName: " << it->first <<  " , isInUse: " << it->second->get_in_use() << endl;
            i++;
        }
        char bufy;
        cout << "Disk content: '";
        for (i = 0; i < DISK_SIZE; i++)
        {
            cout << "(";
            int ret_val = fseek(sim_disk_fd, i, SEEK_SET);
            ret_val = fread(&bufy, 1, 1, sim_disk_fd);
            cout<<bufy;
            cout << ")";
        }
        cout << "'" << endl;
    }
    // ------------------------------------------------------------------------
    void fsFormat( int blockSize =4 ) {
        if(is_formated){
            for(map<string,FileDescriptor*>::iterator it = MainDir.begin(); it != MainDir.end() ; ++it){ //iterator to run through the disk and delete it's contents if available
                delete it->second->get_fs_file();
                delete it->second;
            }
            MainDir.clear();
            OpenFileDescrifileptors.clear();
            delete[] BitVector;
            fseek(sim_disk_fd,0,SEEK_SET);
            for(int i = 0 ; i < DISK_SIZE ; i++){
                fwrite("\0",1,1,sim_disk_fd);
            }
        }
        BLOCKSIZE = blockSize;
        BitVectorSize = DISK_SIZE/blockSize;;
        BitVector = new int[BitVectorSize]; //reset the bit vector according to the block size and disk size
        for (int i = 0 ; i < BitVectorSize ; i++){
            BitVector[i] = 0;
        }
        is_formated = true;
        MAXSIZE = blockSize*blockSize;
    }
    // ------------------------------------------------------------------------
    int CreateFile(string fileName) {
        //case handling
        if(!is_formated){
            return -1;
        }
        if(MainDir.find(fileName) != MainDir.end()){
            return -1;
        }
        int blockSize = DISK_SIZE / BitVectorSize;
        FsFile* newFsFile = new FsFile(blockSize);
        FileDescriptor* newFile = new FileDescriptor(fileName,newFsFile);
        newFile->set_in_use(true);
        int fd = get_fd();
        MainDir.insert({fileName,newFile}); //inserting the new file to both main dir and openFDS
        OpenFileDescrifileptors.insert({fd,newFile});
        return fd;

    }
    // ------------------------------------------------------------------------
    int OpenFile(string fileName) {
        //case handling
        if(!is_formated){
            return -1;
        }
        if(MainDir.find(fileName) == MainDir.end()){
            return -1;
        }
        FileDescriptor* File = MainDir.at(fileName);
        if(File->get_in_use()){
            return -1;
        }
        int fd = get_fd();
        File->set_in_use(true);
        OpenFileDescrifileptors.insert({fd,File});
        return fd;
    }
    // ------------------------------------------------------------------------
    string CloseFile(int fd) {
        //case handling
        if(!is_formated || OpenFileDescrifileptors.find(fd) == OpenFileDescrifileptors.end()){
            return "-1";
        }
        FileDescriptor* File = OpenFileDescrifileptors.at(fd); //find the fd accordingly and close it and remove it from openFDS
        OpenFileDescrifileptors.erase(fd);
        File->set_in_use(false);
        return File->getname();

    }
    // ------------------------------------------------------------------------
    int WriteToFile(int fd, char *buf, int len ) {
        //case handling
        if(!is_formated || OpenFileDescrifileptors.find(fd) == OpenFileDescrifileptors.end()){
            return -1;
        }
        //initilization for faster use
        FileDescriptor* File = OpenFileDescrifileptors.at(fd);
        FsFile* Fs = File->get_fs_file();
        int B_InUSE = Fs->get_block_in_use();
        int size = Fs->getfile_size();
        int Ind_block = Fs->get_index_block();
        //if len is more than max size write as much as we can
        if(size + len > MAXSIZE){
            len = MAXSIZE - size;
        }
        if(Ind_block == -1){ // get the index block
            Ind_block = get_block();
            if(Ind_block == -1){
                cout<<"No space in disk"<<endl;
                return -1;
            }
            Fs->set_index_block(Ind_block);
        }
        int offset = size % BLOCKSIZE;
        int Block;
        if(offset > 0){ //read the empty block to write in
            char c;
            int check = Block_Read(Ind_block, B_InUSE - 1, &c, 1);
            if(check == 0)
                return -1;
                Block = (int) c;
        }else{
            Block = get_block();
        }
        int i = 0;
        while(len > 0){
            if(Block == -1){
                break;
            }
            if(offset == 0){ //writing to the block if there is no remainder
                char c = (char)Block;
                int check = Block_Write(Ind_block, B_InUSE, &c, 1);
                if(check == 0){
                    return -1;
                }
            }
            int c = Block_Write(Block, offset, buf + i, len);
            len-=c;
            size += c;
            i+= c;
            if(offset > 0){
                offset = 0;
            }else{
                B_InUSE++;
            }
            if(len == 0){
                break;
            }
            Block = get_block();
        }
        //update blocksize and current block in use
        Fs->set_file_size(size);
        Fs->set_block_inuse(B_InUSE);
        return i;
    }
    // ------------------------------------------------------------------------
    int DelFile( string FileName ) {
        //case handling
        if(!is_formated || MainDir.find(FileName) == MainDir.end()){
            return -1;
        }
        FileDescriptor* File = MainDir.at(FileName);
        FsFile* Fs = File->get_fs_file();
        if(File->get_in_use()){
            return -1;
        }
        char clearbuffer [BLOCKSIZE];
        for (int i = 0 ; i <BLOCKSIZE ; i++){ //replace file contents with empty chars
            clearbuffer[i] = '\0';
        }
        //init
        int Ind_Block = Fs->get_index_block();
        int B_InUSE = Fs->get_block_in_use();
        if(Ind_Block != -1){
            for(int i = 0 ; i < B_InUSE ; i++){
                char c;
                int check = Block_Read(Ind_Block, i, &c, 1);
                if(check == 0){
                    return -1;
                }
                int Block = (int)c;
                check = Block_Write(Block, 0, clearbuffer, BLOCKSIZE); //replacing the file with empty chars
                if(check == 0){
                    return -1;
                }
                BitVector[Block] = 0;
            }
            int check = Block_Write(Ind_Block, 0, clearbuffer, BLOCKSIZE);
            if(check == 0){
                return -1;
            }
            BitVector[Ind_Block] = 0;
        }
        delete Fs; //cleaning...
        delete File;
        MainDir.erase(FileName);
        return 1;
    }
    // ------------------------------------------------------------------------
    int ReadFromFile(int fd, char *buf, int len ) {
        buf[0] = '\0';
        //case handling
        if(!is_formated  || OpenFileDescrifileptors.find(fd) == OpenFileDescrifileptors.end()){
            return -1;
        }
        //faster init
        FileDescriptor* File = OpenFileDescrifileptors.at(fd);
        FsFile* Fs = File->get_fs_file();
        int Ind_block = Fs->get_index_block();
        int size = Fs->getfile_size();
        if(len > size){ //if len is more than size we'll read as much as we can
            len = size;
        }
        int blocksToRead = len/BLOCKSIZE; //number of blocks to read
        if(len%BLOCKSIZE > 0){
            blocksToRead++;
        }
        int return_val = len; //how many chars we successfully read
        int Bi = 0;
        int Ci = 0;
        while (len > 0){
            char c;
            int check = Block_Read(Ind_block, Bi, &c, 1);
            if(check == 0){ //if no chars were read
                return -1;
            }
            int Block = (int)c;
            Bi++;
            int count = Block_Read(Block, 0, buf + Ci, len);
            Ci+=count;
            len -= count;
        }
        buf[return_val] = '\0';
        return return_val; //number of chars
    }
};

int main() {

    int blockSize;
	int direct_entries;
    string fileName;
    char str_to_write[DISK_SIZE];
    char str_to_read[DISK_SIZE];
    int size_to_read;
    int _fd;

    fsDisk *fs = new fsDisk();
    int cmd_;
    while(1) {
        cin >> cmd_;
        switch (cmd_)
        {
            case 0:   // exit
				delete fs;
				exit(0);
                break;

            case 1:  // list-file
                fs->listAll();
                break;

            case 2:    // format
                cin >> blockSize;
                fs->fsFormat(blockSize);
                break;

            case 3:    // creat-file
                cin >> fileName;
                _fd = fs->CreateFile(fileName);
                cout << "CreateFile: " << fileName << " with File Descriptor #: " << _fd << endl;
                break;

            case 4:  // open-file
                cin >> fileName;
                _fd = fs->OpenFile(fileName);
                cout << "OpenFile: " << fileName << " with File Descriptor #: " << _fd << endl;
                break;

            case 5:  // close-file
                cin >> _fd;
                fileName = fs->CloseFile(_fd);
                cout << "CloseFile: " << fileName << " with File Descriptor #: " << _fd << endl;
                break;

            case 6:   // write-file
                cin >> _fd;
                cin >> str_to_write;
                fs->WriteToFile( _fd , str_to_write , strlen(str_to_write) );
                break;

            case 7:    // read-file
                cin >> _fd;
                cin >> size_to_read ;
                fs->ReadFromFile( _fd , str_to_read , size_to_read );
                cout << "ReadFromFile: " << str_to_read << endl;
                break;

            case 8:   // delete file
                 cin >> fileName;
                _fd = fs->DelFile(fileName);
                cout << "DeletedFile: " << fileName << " with File Descriptor #: " << _fd << endl;
                break;
            default:
                break;
        }
    }

}