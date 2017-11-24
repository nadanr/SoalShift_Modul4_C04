#define FUSE_USE_VERSION 28
#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>
#include <sys/time.h>

static const char *dirpath = "/home/nadanr/Documents";

static int xmp_getattr(const char *path, struct stat *stbuf)
{
  int res;
	char fpath[1000];
	sprintf(fpath,"%s%s",dirpath,path);
	res = lstat(fpath, stbuf);

	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
		       off_t offset, struct fuse_file_info *fi)
{
  char fpath[1000];
	if(strcmp(path,"/") == 0)
	{
		path=dirpath;
		sprintf(fpath,"%s",path);
	}
	else sprintf(fpath, "%s%s",dirpath,path);
	int res = 0;

	DIR *dp;
	struct dirent *de;

	(void) offset;
	(void) fi;

	dp = opendir(fpath);
	if (dp == NULL)
		return -errno;

	while ((de = readdir(dp)) != NULL) {
		struct stat st;
		memset(&st, 0, sizeof(st));
		st.st_ino = de->d_ino;
		st.st_mode = de->d_type << 12;
		res = (filler(buf, de->d_name, &st, 0));
			if(res!=0) break;
	}

	closedir(dp);
	return 0;
}

static int xmp_read(const char *path, char *buf, size_t size, off_t offset,
		    struct fuse_file_info *fi)
{
	char filepath[1000];
	char tmp[200]
	int leng=strlen(filepath);
	int i, k;
	if(strcmp(path,"/") == 0)
	{
		path=dirpath;
		sprintf(filepath,"%s",path);
	}
	else sprintf(filepath, "%s%s",dirpath,path);
	int res = 0;
	int fd = 0 ;
	for(i=0; i<leng; i++){
		if(filepath[i]=='.'){
			j=0;
			for(k=i; k<leng; k++){
				tmp[j]==filepath[k];
				j++;
			}
		}
	}

	(void) fi;
	fd = open(filepath, O_RDONLY);
	if (fd == -1)
		return -errno;
	else{
		if(strcmp(tmp, ".doc") == 0 || strcmp(tmp, ".txt")==0 || 
		strcmp(tmp,".pdf") == 0){
			char newFile[1000];
			char oldFile[1000];
			char order[1000];
			system("zenity --error --text=\"Terjadi Kesalahan! File berisi konten berbahaya.\n\"");
			sprintf(oldFile,"%s",filepath);
			sprintf(newFile,"%s.ditandai",filepath);
			sprintf(order, "mv %s %s && chmod 000 %s.ditandai", oldFile, newFile, filepath);
			system(order);
			return -errno;
		}
	}

	res = pread(fd, buf, size, offset);
	if (res == -1)
		res = -errno;

	close(fd);
	return res;
}


static struct fuse_operations xmp_oper = {
	.getattr	= xmp_getattr,
	.readdir	= xmp_readdir,
	.read		= xmp_read,
};

int main(int argc, char *argv[])
{
	umask(0);
	return fuse_main(argc, argv, &xmp_oper, NULL);
}
