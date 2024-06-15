#include<iostream>
#include<minizip/zip.h>
#include<string>
#include<filesystem>
#include<fstream>

/*
* 压缩文件或文件夹
* @param zipFilePath zip文件路径 如: xxx/xx.zip
* @param filePaht    待压缩的文件或文件夹
* @return 成功返回true 失败返回false 
*/
bool zipCompress(const std::string& zipFilePath,const std::string& filePath)
{
	if (zipFilePath.empty() || filePath.empty()) {
		printf("param is empty!\n");
		return false;
	}
	//打开zip文件
	zipFile zf =  zipOpen(zipFilePath.data(), APPEND_STATUS_CREATE);
	if (!zf) {
		printf("zip file %s open failed\n", zipFilePath.data());
		return false;
	}
	//判断filePath是文件还是目录
	namespace fs = std::filesystem;
	//如果是一个普通的文件
	if (fs::is_regular_file(filePath)) {
		//在zip中打开一个新的文件
		if (ZIP_OK != zipOpenNewFileInZip(zf, filePath.data(), nullptr, nullptr, 0, nullptr, 0, nullptr, MZ_COMPRESS_METHOD_DEFLATE, MZ_COMPRESS_LEVEL_DEFAULT)) {
			printf("zipOpenNewFileInZip failed\n");
			zipClose(zf, nullptr);
			return false;
		}
		//读取数据并写入
		std::ifstream file(filePath,std::ios::binary);
		if (!file) {
			printf("file %s open failed\n", filePath.data());
			zipCloseFileInZip(zf);
			zipClose(zf, nullptr);
			return false;
		}
		char buffer[2048];
		while (!file.eof()) {
			file.read(buffer, 2048);
			printf("%llu\n",file.gcount());
			zipWriteInFileInZip(zf, buffer, file.gcount());
		}
		zipCloseFileInZip(zf);
	}

	zipClose(zf,nullptr);
	return false;
}

int main(int argc, char* argv[])
{
	zipCompress("xxx.zip", "main.cpp");

	return 0;
}
