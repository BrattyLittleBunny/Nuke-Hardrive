/*
This code will find the memory locations of files then write over them
obviously use pointers. 
will possibly read from a file with file paths.
it should write over the locations with random alpha numeric charactars
this combined with like a batch file to then delete all the files it read or delete them with C(?)
don't forget to delete and overwrite the list as well :3

NEEDED:
-------
[ ]Way to access Disk drive to delete the data
[ ]Detects hard disk vs Solid State Drive
	[X]Windows
	[X]Linix/Unix
	[ ]Macs
[~]Detect OS system - OS check should be before disk check to optimize what it's searching.
	-We're checking the OS system so we know what header is needed to actually mess with the hard drive itself
[ ]Reads files from list
[ ]Overwrites data with multiple passes
	[ ]1st pass 0x00
	[ ]2nd pass 0xFF
	[ ]3rd+ random values (anything up to 256 'cause 1 byte is 8 bits)
[ ]Sets files to 0 bytes size in case of references
[ ]Deletes files. maybe delete before the pass in case there is a way to see...

Wanted (optional Functions):
--------
[ ]Choice of delete files or whole Disk
[ ]delete logs
[ ]Set number of pass throughs (might just keep default to 7)
[ ]Encrypts hardrive afterwards
[ ]GUI for choices or file that hold choices to set and forget
[ ]Open port for remote detonation that requires a password(?) (probably not)
[ ]make each OS system have it's own files to declutter the code
[ ]Use only the standard libraries

Wanted (cosmetic):
---------
[ ]Nuke noise
[ ]Alarm blaring
[ ]maybe a video of bomb falling or psuedo code scrolling across screen
[ ]Loading bar(?)

*/

#include <stdio.h>
//#include <windows.h>; //for windows API :3 obviously
#include <stdlib.h>
#include <dirent.h>
#include <string.h>

#define DEFAULT_PASS_THROUGHS 7 //7 is the minimum that DoD prefers.
#define NIX_BLOCK_PATH "/sys/block/" //location for what type of Hard drive is in the *nix's


struct computer_system { 
	//to record the system info
	//using structures because i learned about them and am learning =:3
	//all chars so 1 byte size because there shouldn't be any need to have anything large in them
	char systemOS; //what the OS is. A windows, B Mac, C Linux/Unix, F Exit
	char systemHD; //what the hard drive is. A NVMe, B SSD, C HDD, F Exit
	char filesDeletionMethod; //wether files or entire disk.
	char encryptionMethod; //should it be just the files stated, the larger file, or the whole drive?
};

//Declarations
char osCheck(); //code to check OS. 
char hdCheck(char OS); //code to check hard drive type
void overWriting(); //code to overwrite the data. probably need a section to find the data...
void cleanUp(); //free up the memory, change the file size and delete, exit condition.	
void clear_stdin();

int main(){
	char answers[4];
	printf("Welcome, I'm gonnna annoy you by asking every instance to see if you want to continue :3\nfor safety\n\ndo you want to continue? (type \"YES\" to continue)");
	fgets(answers, sizeof(answers), stdin);
	answers[strcspn(answers, "\n")] = '\0';
	clear_stdin();
	
	if (strcmp(answers,"YES") != 0) {
		printf("\n\nYou did not say \"YES\". Exiting now\n\n");
		printf("%s\n\n", answers);
		return 0;
	}
	
	struct computer_system this_computer;
	
	this_computer.systemOS = osCheck();
	if (this_computer.systemOS == 'F') return 0;
	printf("osCheck is done. Do you wish to continue?");
	fgets(answers, sizeof(answers), stdin);
	answers[strcspn(answers, "\n")] = '\0';
	clear_stdin();
	
	if (strcmp(answers, "YES") != 0) {
		printf("\n\nYou did not say \"YES\". Exiting now");
		return 0;
	}
	
	this_computer.systemHD = hdCheck(this_computer.systemOS);
	if (this_computer.systemHD == 'F') return 0; //if, can;t find hardrive, it's gonna fuck off :3
	
	
	return 0;
}

void clear_stdin() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

char osCheck(){
	//#ifdef command checking for what OS is operating using the macro. 
	//Should be checking after compilation. if wanted to check before just add #ifdef #elif instead of if/else :3
	
	#ifdef _WIN32_
		//sets value to windows. win32 is on all windows.
		printf("\nThis device is running Windows\n");
		return 'A';
		
	#elif (__APPLE__ || __MACH__)
		//sets value to apple, there's 2 different one and idk if they both show up
		printf("\nThis device is running MAC\n");
		return 'B';
	
	#elif (__unix__ || __linux)
		//unix/linux 
		printf("\nThis device is running Unix/Linux\n");
		return 'C';
	
	#else 
		printf("\nNo idea what the OS is. that sucks.\nGonna close the program now :3\nUwU bye byezzzzzz~\n");
		return 'F';
	#endif

}

char hdCheck(char OS){
	//Checking if it's using SSD, Disk Drive, or NVME drives.
	//Each is gonna be OS specific, might be best to break them into their own files.	
	
	//Windows
	if (OS == 'A'){ 
		HANDLE hDevice = createFileA( //opening a handle to the drive, Handles are how windows labels it's things. kinda like an object or structure from what i can see 
		"\\\\.\\PhysicalDrive0", //this is the name for first hardrive, should increment by 1 for each additional if there are. but that's too much for me to care to do ;3
		GENERIC_READ | GENERIC_WRITE, //requesting read/write privlidges for the drive
		FILE_SHARE_READ | FILE_SHARE_WRITE, //allows other processes to read/write
		NULL, //this is the security attribute, we're stating no attribute, which is the default.
		OPEN_EXISTING, //making sure it exists
		0, //special flags. idk what they are
		NULL //no template file. again idk what they are. should probably research it eventually... 
		); 
		
		if (hDevice == INVALID_HANDLE_VALUE){ //Invalid Handle Value means it can't access the drive, so we're just gonna fuck off :3
			printf("Can not access the drive.\nI probably made an oopsie woopsie somewhere D: \npwobably all my fault ;~;\ngonna exit the pwogwam now :3");
			return 'F';
		}
	
		STORAGE_PROPERTY_QUERY wDriveType; //storage information we're quuerying for :3
		STORAGE_DESCRIPTION_HEADER wHeader = {0}; //it's 0 to make sure it's not reading unitialized data
		DWORDS wBytesReturned; //the size of the query
		
		wDriveType.PropertyID = StorageDeviceProperty; //basic device properties
		wDriveType.QueryType = PropertyStandardQuery; //standard query for static devices
	
		//DeviceIoControl is a windows API for interacting with devices
		if (!DeviceIoControl(hDevice, IOCTL_STORAGE_QUERY_PROPERTY,
							 &wDriveType, sizeof(wDriveType),
							 &wHeader, sizeof(wHeader),
							 $wBytesReturned, NULL)) { //we're basically just double checking to make sure everything works :3
			printf("\nDeviceIOControl failed. Well shit.\n");
			CloseHandle(hDevice); //closing the handle to free up memory and shit (i think... just... just doing cleanup at least :3)
			return 'F'; //F IS FOR FAILURE!!!!! Q~Q
		}
	
		BYTE *buffer = (BYTE*)malloc(wHeader.Size); //DONT FORGET TO FREE THE BUFFER!!!! I KNOW YOU!!!!
		
		//The only difference here is checking with the buffer values added :3
		if (!DeviceIoControl(hDevice, IOCTL_STORAGE_QUERY_PROPERTY,
							 &wDriveType, sizeof(wDriveType),
							 buffer, wHeader.Size,
							 $wBytesReturned, NULL)) {
			printf("\n2nd DeviceIOControl failed.\nAt this point, i think it's you >:3.\n");
			free(buffer); //this is only freeing when it fails, don't forget when it succeeds
			CloseHandle(hDevice);
			return 'F'; //This is a comment UwU it has not intrinsic value to add to this program i just wanted to put it >;3
		}
		
		STORAGE_DEVICE_DESCRIPTOR *desc = (STORAGE_DEVICE_DESCRIPTOR*)buffer;

		if (desc->BusType == BusTypeNvme) {
			printf("Drive Type: NVMe SSD\n");
			free(buffer);
			CloseHandle(hDevice);
			return 'A';
		} 
		else if (desc->BusType == BusTypeAta || desc->BusType == BusTypeSata) {
			printf("Drive Type: SATA Drive (HDD or SSD)\nChecking which now.\n");
			free(buffer); 
			
			DISK_GEOMETRY_EX diskGeometry; //finding the rotational value to see if HDD or SSD
			
			if (!DeviceIoControl(hDevice, IOCTL_DISK_GET_DRIVE_GEOMETRY_EX,
								 NULL, 0,
								 &diskGeometry, sizeof(diskGeometry),
								 &wBytesReturned, null)){
				printf("\n3rd DeviceIOControl failed.\nI wonder if i know what i'm doing?.\n");	
				CloseHandle(hDevice);
				return 'F';
			}
			
				
			 if (diskGeometry.Geometry.MediaType == FixedMedia) {
        		if (diskGeometry.DiskSize.QuadPart > 0) { // Ensure valid disk
					DWORD rotationRate = diskGeometry.Geometry.BytesPerSector; 
					if (rotationRate == 0) { //SSD's do not rotate :3
						printf("Drive Type: SSD (Solid State Drive)\n");
						CloseHandle(hDevice);
						return 'B';
					} 
					else {
						printf("Drive Type: HDD (Hard Disk Drive)\n");
						CloseHandle(hDevice);
						return 'C';
					}
				} 
				 else {
					printf("Unknown Drive Type\n");
					CloseHandle(hDevice);
					return 'F';
				}
			} 
			else {
				printf("Removable or unknown media type\nDon't know what to do with this :3\n");
			 	CloseHandle(hDevice);
				return 'F';
			}	
			
		} 
		else {
			printf("Drive Type: Unknown\n");
			free(buffer);
			CloseHandle(hDevice);
			return 'F';
		}

		
	}//END OF WINDOWS
	
	
	//Mac
	if (OS == 'B'){
	printf("To be added :3 UwU i'm just a little bunny");
	}//END OF MAC
	
	//Unix/Linux
	if (OS == 'C'){
		
		struct dirent *entry;
		DIR *dp = opendir(NIX_BLOCK_PATH);
		
		if (!dp) {
			perror("\nfailed to access /sys/block/\nending the program\n");
			return 'F';
		}
		
		while ((entry = readdir(dp)) != NULL) {
			
			if (entry->d_name[0] != '.') {
				char nixPath[256];
				snprintf(nixPath, sizeof(nixPath), "%s%s/queue/rotational", NIX_BLOCK_PATH, entry->d_name);
				
				FILE *file = fopen(nixPath, "r");
				if (!file){
					perror("\nCouldn't get the device info\n");
					continue;
				}
				
				char buffer[2];
				if (fgets(buffer, sizeof(buffer), file) != NULL) {
					printf("Drive: %s - Type: %s\n", entry->d_name, buffer[0] == '0' ? "SSD" : "HDD");
					if (buffer[0] == '0'){
						fclose(file);
						closedir(dp);
					//	return 'B';
					}
					else if (buffer[0] == '1'){
						fclose(file);
						closedir(dp);
					//	return 'C';
					}
					else {
						printf("\nno idea what type of drive lol :3\ngoodbye\n");
						fclose(file);
						closedir(dp);
						
					}
				}
				
			}
			
		}
		
		
	}//END OF UNIX/LINUX
		
		
	printf("well idk, something went wrong in the hard drive check function. UwU");
	return 'F';
} //end of hdCheck()
// UwU nya~