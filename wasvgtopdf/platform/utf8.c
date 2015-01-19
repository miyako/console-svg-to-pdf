#ifndef __linux__
#ifndef __APPLE__

#include <string>
#include <vector>
#include <windows.h>
#include <Shlobj.h>

typedef std::vector<std::wstring> wstrings;

#ifndef strings
typedef std::vector<std::string> strings;
#endif

int wchar_to_utf8(std::wstring &wstr, std::string &str){
    int error = 0; 
    int len = WideCharToMultiByte(CP_UTF8, 0, (LPCWSTR)wstr.c_str(), wstr.length(), NULL, 0, NULL, NULL);
    if(len){
        std::vector<char> buf(len + 1);
        if(WideCharToMultiByte(CP_UTF8, 0, (LPCWSTR)wstr.c_str(), wstr.length(), (LPSTR)&buf[0], len, NULL, NULL)){
            str = std::string((const char *)&buf[0]);
        }
    }else{
        str = std::string((const char *)"");
        error = -1;
    }
    return error;
}

int utf8_to_wchar(std::string &str, std::wstring &wstr){
    int error = 0; 
    int len = MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)str.c_str(), str.length(), NULL, 0);
    if(len){
        std::vector<char> buf((len + 1) * sizeof(wchar_t));
        if(MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)str.c_str(), str.length(), (LPWSTR)&buf[0], len)){
            wstr = std::wstring((const wchar_t *)&buf[0]);
        }
    }else{
        wstr = std::wstring((const wchar_t *)L"");
        error = -1;
    }
    return error;
}

void escape_path(std::wstring &path){
    for (unsigned int i = 0; i < path.size(); ++i)
        if (path.at(i) == '\\')
            path.at(i) = L'/';
}

void unescape_path(std::wstring &path){
    for (unsigned int i = 0; i < path.size(); ++i)
        if (path.at(i) == '/')
            path.at(i) = L'\\';
}

void escape_path(std::string &path){
    std::wstring wpath;
    utf8_to_wchar(path, wpath);
    escape_path(wpath);
    wchar_to_utf8(wpath, path);
}

void unescape_path(std::string &path){
	std::wstring wpath;
	utf8_to_wchar(path, wpath);
	unescape_path(wpath);
	wchar_to_utf8(wpath, path);
}

FILE *_fopen(std::string &path, const wchar_t *mode){
    std::wstring wpath;
    utf8_to_wchar(path, wpath);
    return _wfopen(wpath, mode);   
}

void get_subpaths(std::wstring &path, std::string &inputfilename, wstrings *paths, wstrings *posix_paths, size_t absolutePathOffset, bool *isFolder){

    WIN32_FIND_DATA find;	

    HANDLE h = FindFirstFile(path.c_str(), &find);
    
	std::wstring wpath, wposix_path;

    if(h != INVALID_HANDLE_VALUE){
        
        do {
            
            std::wstring relativePath = find.cFileName;	
            
			if(!absolutePathOffset){
				*isFolder = false;
				wchar_to_utf8(relativePath, inputfilename);
			}

            if((!wcscmp(relativePath.c_str(), L"..")) || (!wcscmp(relativePath.c_str(), L".")))
                continue;		
            
            if(find.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY){
                
                if(!absolutePathOffset){
                    *isFolder = true;
                    absolutePathOffset = path.size() - 1;
                    get_subpaths(path + L"\\*", inputfilename, paths, posix_paths, absolutePathOffset, isFolder);
                    
                }else{
                    
					wpath = path.substr(0, path.size() - 1).substr(absolutePathOffset + 2) + relativePath + L"\\";
					wposix_path = wpath;
					escape_path(wposix_path);

                    paths->push_back(wpath);
                    posix_paths->push_back(wposix_path);

                    get_subpaths(path.substr(0, path.size() - 1)  + relativePath + L"\\*", inputfilename, paths, posix_paths, absolutePathOffset, isFolder);		
                    
                }
                
            }else{
                
                if(!absolutePathOffset){
                    
					wpath = relativePath;
					wposix_path = wpath;
					escape_path(wposix_path);

                    paths->push_back(wpath);
                    posix_paths->push_back(wposix_path);
                    
                }else{

					wpath = path.substr(0, path.size() - 1).substr(absolutePathOffset + 2) + relativePath;
					wposix_path = wpath;
					escape_path(wposix_path);

                    paths->push_back(wpath);
                    posix_paths->push_back(wposix_path);

                }
                
            }		
            
        } while (FindNextFile(h, &find));
        
        FindClose(h);
        
    }    

}

void get_subpaths(std::string &path, std::string &inputfilename, strings *paths, strings *posix_paths, int ignore_dot){
    
    std::wstring wpath;
    utf8_to_wchar(path, wpath);
    
	unescape_path(wpath);

    wstrings wpaths, wposix_paths;
    
	wpaths.clear();
    wposix_paths.clear();
    bool isFolder;
	get_subpaths(wpath, inputfilename, &wpaths, &wposix_paths, 0, &isFolder);
    
    unsigned int i;
    
    paths->clear();
    posix_paths->clear();
    
    std::string _path, _posixpath;
    
	std::wstring basepath = wpath;

	if(isFolder){
		if((path.size() > 0) && (path.at(path.size() - 1) != '\\')){
			basepath += L'\\';
		}		
	}

    for(i = 0; i < wpaths.size(); ++i){
        
        wchar_to_utf8(basepath + wpaths.at(i), _path);
        wchar_to_utf8(wposix_paths.at(i), _posixpath);
        
        if(ignore_dot){
            if(!(inputfilename.at(0) == '.')){ 
                if(_posixpath.find("/.") == std::string::npos){
                    paths->push_back(_path);
                    posix_paths->push_back(_posixpath);                  
				}
			}
		}else{
			paths->push_back(_path);
			posix_paths->push_back(_posixpath);        
		}
	}
}

void get_file_name(std::string &relativePath, std::string &filename)
{
    std::wstring wfilename, wrelativePath;
    utf8_to_wchar(relativePath, wrelativePath);
    
    filename = relativePath;
    
    for(unsigned int i = wrelativePath.size(); i > 0; --i){
        
        if(wrelativePath.at(i-1) == '/' && (i != wrelativePath.size())){
            wfilename = wrelativePath.substr(i);
            wchar_to_utf8(wfilename, filename);
            break;
        }
    }
}

void create_parent_folder(std::string &path)
{
    std::wstring wpath;
    utf8_to_wchar(path, wpath);
    unescape_path(wpath);

    wchar_t	fDrive[_MAX_DRIVE],
    fDir[_MAX_DIR],
    fName[_MAX_FNAME],
    fExt[_MAX_EXT];
    
    _wsplitpath_s(wpath.c_str(), fDrive, fDir, fName, fExt);	
    
    std::wstring folderPath = fDrive;
    folderPath += fDir;
    
    SHCreateDirectory(NULL, (PCWSTR)folderPath.c_str());
}

void create_folder(std::string &path)
{
    std::wstring wpath;
    utf8_to_wchar(path, wpath);
    unescape_path(wpath);

    SHCreateDirectory(NULL, (PCWSTR)wpath.c_str());
}

#endif
#endif