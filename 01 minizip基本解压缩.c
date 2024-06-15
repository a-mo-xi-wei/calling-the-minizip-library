#include <stdio.h>
#include <minizip/zip.h>

void test_zipCompress()
{
	//创建一个zip文件
	zipFile zf = zipOpen("data/xxx.zip", APPEND_STATUS_CREATE);
	if (!zf) {
		printf("zipOpen failed\n");
		return;
	}

	//在zip文件中打开一个新的文件
	if (MZ_OK != zipOpenNewFileInZip(zf, "hello.txt", NULL, NULL, 0, NULL, 0, NULL, MZ_COMPRESS_METHOD_DEFLATE, MZ_COMPRESS_LEVEL_DEFAULT))
	{
		printf(" zipOpenNewFileInZip failed\n");
		zipClose(zf, NULL);
		return;
	}
	//向hello.txt中写入数据
	if(MZ_OK != zipWriteInFileInZip(zf, "hello wrold,玩蛇老师", 24)) {
		printf("zipWriteInFileInZip failed\n");
		zipCloseFileInZip(zf);
		zipClose(zf, NULL);
		return;
	}

	zipCloseFileInZip(zf);
	zipClose(zf,NULL);
}

void test_zipUnCompress()
{
	//打开zip文件	
	unzFile uf =  unzOpen("data/xxx.zip");
	if (!uf) {
		printf("open filed\n");
		return;
	}
	//获取当前文件
	unz_file_info file_info;
	char filename[128];
	if (UNZ_OK != unzGetCurrentFileInfo(uf, &file_info, filename, 128, NULL, 0, NULL, 0)) {
		printf("unzGetCurrentFileInfo failed\n");
		unzClose(uf);
		return;
	}
	//打开当前文件
	if (UNZ_OK != unzOpenCurrentFile(uf)) {
		printf("unzOpenCurrentFile failed\n");
		unzClose(uf);
		return;
	}
	//读取当前文件
	char buf[BUFSIZ];
	int len = unzReadCurrentFile(uf, buf, BUFSIZ);
	if (len < 0) {
		printf("unzReadCurrentFile failed\n");
		unzClose(uf);
		return;
	}
	printf("read size:%d buf:%.*s\n", len,len,buf);


	unzClose(uf);
}

int main0(int argc, char* argv[])
{
	test_zipCompress();
	test_zipUnCompress();
	return 0;
}