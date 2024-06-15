#include<iostream>
#include<minizip/zip.h>
#include<string>
#include<filesystem>
#include<fstream>


static bool zipCompress(zipFile zf,const std::string& filePath)
{
	//在zip中打开一个新的文件
	if (ZIP_OK != zipOpenNewFileInZip(zf, filePath.data(), nullptr, nullptr, 0, nullptr, 0, nullptr, MZ_COMPRESS_METHOD_DEFLATE, MZ_COMPRESS_LEVEL_DEFAULT)) {
		printf("zipOpenNewFileInZip failed\n");
		zipClose(zf, nullptr);
		return false;
	}
	//读取数据并写入
	std::ifstream file(filePath, std::ios::binary);
	if (!file) {
		printf("file %s open failed\n", filePath.data());
		zipCloseFileInZip(zf);
		zipClose(zf, nullptr);
		return false;
	}
	char buffer[2048];
	while (!file.eof()) {
		file.read(buffer, 2048);
		//printf("%llu\n",file.gcount());
		zipWriteInFileInZip(zf, buffer, file.gcount());
	}
	zipCloseFileInZip(zf);
	return true;
}

/*
* 压缩文件或文件夹
* @param zipFilePath zip文件路径 如: xxx/xx.zip
* @param filePaht    待压缩的文件或文件夹
* @return 成功返回true 失败返回false
*/
bool zipCompress(const std::string& zipFilePath, const std::string& filePath)
{
	if (zipFilePath.empty() || filePath.empty()) {
		printf("param is empty!\n");
		return false;
	}
	//打开zip文件
	zipFile zf = zipOpen(zipFilePath.data(), APPEND_STATUS_CREATE);
	if (!zf) {
		printf("zip file %s open failed\n", zipFilePath.data());
		return false;
	}
	//判断filePath是文件还是目录
	namespace fs = std::filesystem;
	//如果是一个普通的文件
	if (fs::is_regular_file(filePath)) {
		zipCompress(zf, filePath);
	}
	else if (fs::is_directory(filePath)) {
		for (auto& entry : fs::recursive_directory_iterator(filePath)) {
			//跳过目录
			if (entry.is_directory())
				continue;
			std::string file_path = entry.path().string();
			//std::cout << file_path << std::endl;
			zipCompress(zf, file_path);
		}
	}
	zipClose(zf, nullptr);
	return true;
}

bool unzipCompress(const std::string& zipFilePath, const std::string& dir = {})
{
	if (zipFilePath.empty())
		return false;

	unzFile uf = unzOpen(zipFilePath.data());
	if (!uf) {
		printf("open zip file failed\n");
		return false;
	}


	unz_file_info file_info;
	char filename[128];
	do {
		if (ZIP_OK != unzGetCurrentFileInfo(uf, &file_info, filename, 128, nullptr, 0, nullptr, 0)) {
			printf("unzGetCurrentFileInfo failed\n");
			return false;
		}
		if (ZIP_OK != unzOpenCurrentFile(uf)) {
			printf("unzOpenCurrentFile failed\n");
			return false;
		}

		namespace fs = std::filesystem;
		fs::path ph(dir);
		if (!fs::create_directory(dir) && !fs::is_directory(ph) || ph.empty()) {
			auto str = fs::path(zipFilePath).filename().string();
			ph = str.substr(0, str.rfind('.'));
		}
		ph.append(filename);
		if (!fs::exists(ph.parent_path()) &&
			!fs::create_directories(ph.parent_path())) {
			printf("create dir <%s> failed!\n", ph.string().data());
			return false;
		}

		std::cout << ph.generic_string() << std::endl;
		char buffer[4096];
		int len;
		std::ofstream file(ph.string());
		while ((len = unzReadCurrentFile(uf, buffer, 4096)) != ZIP_EOF) {
			if (!file) {
				printf("open file %s fialed\n", ph.string().data());
				unzClose(uf);
				return false;
			}
			file.write(buffer, len);
		}
		file.close();
	} while (UNZ_OK == unzGoToNextFile(uf));
	
	unzClose(uf);
	return true;
}

int main(int argc, char* argv[])
{
	//zipCompress("xxx.zip", "xxx");
	unzipCompress("xxx.zip","maye");
	return 0;
}
