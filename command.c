/*  Author: Qi Han
	Date: 2020-06-03*/

#include "command.h"
#include <dirent.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/types.h>    
#include <sys/stat.h>    
#include <fcntl.h>

/*for the ls command*/
void listDir(){ 
	
	DIR *dir;
	struct dirent *dirent;
	dir = opendir(".");

	// empty directory
	if(dir == NULL){	
		write(2, "Error! Unable to open directory. \n", 35);						
		return;
	}
	while((dirent = readdir(dir)) != NULL){										

		write(1, dirent->d_name, strlen(dirent->d_name));							
		write(1,"\n",1);
	}
	closedir(dir);

}

/*for the pwd command*/
void showCurrentDir(){ 

	char directory[100];
	write(1, getcwd(directory, sizeof(directory)), 									
		strlen(getcwd(directory, sizeof(directory))));
	write(1,"\n",1);
					
}

/*for the mkdir command*/
void makeDir(char *dirName){ 

	// missing operand
	if(dirName == NULL){															
		write(1, "mkdir: missing operand\n", 23);
	}else{
		mkdir(dirName,0777);													
	}
}

/*for the cd command*/
void changeDir(char *dirName){ 
	
	DIR *dir;
	struct dirent *dirent;
	dir = opendir(".");
	int check = 1;

	// make sure the current directory is not empty
	if(dir == NULL){																
		write(2, "Error! Unable to open directory. \n", 35);
		return;
	}

	// execute cd command with valid input
	while((dirent = readdir(dir)) != NULL){											
		if(strcmp(dirent->d_name,dirName) == 0 || strcmp(dirName, "..") == 0 
			|| strcmp(dirName, ".") == 0){

			chdir(dirName);
			check = 0;		
			break;
		}
	}

	// directory not found
	if(check == 1){																	
		write(1,"No such directory found\n", 24);
	}	
	closedir(dir);

}

/*for the cp command*/
void copyFile(char *sourcePath, char *destinationPath){ 
	char *content, *targetFile = NULL;
	size_t size = 1048;
	int socFile, dstFile, line, closeSocFile, closeDstFile;
	content = (char *)malloc(size * sizeof(char));
	socFile = open(sourcePath, O_RDONLY, S_IRUSR);								// open source file
	dstFile = open(destinationPath, O_WRONLY | O_CREAT, S_IRUSR);				// open target file

	// destination path is also a file and in the same directory
	if(socFile != -1 && dstFile != -1){											
																				
		line = read(socFile, content, sizeof(content));							// read source file 
		while(line > 0){
			write(dstFile, content, line);										// cope to the target file
			line = read(socFile, content, sizeof(content));	
		}

		close(socFile);
		close(dstFile);
		free(content);

	}else if(socFile == -1){													// source cannot be found
		write(2, "cp : cannot stat", 16);
		write(2, sourcePath, sizeof(sourcePath));
		write(2, ": No such file or directory \n", 29);
		return;
	}else{																		// when destination path is another directory
		char current_Destination[200];
		getcwd(current_Destination, sizeof(current_Destination));				// record current path
		line = read(socFile, content, sizeof(content));							// read from source file
		chdir(destinationPath);													// switch to the target directory
		targetFile = strrchr(sourcePath,'/') + 1;								// create target file
		dstFile = open(targetFile, O_WRONLY | O_CREAT | O_TRUNC, 0644);
		while(line > 0){
			write(dstFile, content, line);										// cope to the target file
			line = read(socFile, content, sizeof(content));	
		}
		write(1,"\n",1);
												
		chdir(current_Destination);												// go back to the current director
	
		close(socFile);
		close(dstFile);
		free(content);
	}
	

}


/*for the mv command*/
void moveFile(char *sourcePath, char *destinationPath){ 

	rename(sourcePath,destinationPath);

}


/*for the rm command*/
void deleteFile(char *filename){ 
	
	int file; 
	file = open(filename, O_RDONLY, S_IRUSR);	

	if(file == -1){
		write(2, "rm: cannot remove", 17);
		write(2, filename, strlen(filename));
		write(2, ": No such file or directory\n", 28);
		return;
	}else{
		unlink(filename);
	}
}


/*for the cat command*/
void displayFile(char *filename){ 

	int file, line; 
	size_t size = 1024;
	char *content = (char *)malloc(size * sizeof(char));

	file = open(filename, O_RDONLY, S_IRUSR);	

	if(file == -1){
		write(2, "cat: ", 5);
		write(2, filename, strlen(filename));
		write(2, ": No such file or directory\n", 28);
		return;
	}else{
		
		line = read(file, content, sizeof(content));
		
		while(line > 0){
	
			write(1, content, line);
			line = read(file, content, sizeof(content));	
		}	
		write(1,"\n", 1);	
		free(content);
		return;
	}
}