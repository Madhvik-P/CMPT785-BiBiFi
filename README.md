# BiBiFi - CMPT785

The encrypted file system will reside on the host file system as a regular file system but with more security. This file system is being used by multiple clients/users since we can't afford to have different storage for each of our users and want to optimally use all of the available storage.

[![forthebadge](http://forthebadge.com/images/badges/built-with-love.svg)](http://forthebadge.com)

## Compile

Docker (preferred):

Build image (dynamic)
```sh
docker build -t fileserver -f dockerfile
```

Build image (static)
```sh
docker build -t fileserver -f dockerfile-static
```

Run container
```sh
docker run --rm -it <image> bash
```

Unix:

Dynamic binary 

**NOTE** - install devel package for openssl first based on your distro - openssl-devel(rpm), libssl-dev(deb)
```sh
g++ -std=c++17 main.cpp -o fileserver -lssl -lcrypto
```
Static binary
```sh
g++ -std=c++17 main.cpp -o fileserver -static -lssl -lcrypto -ldl -Wl,--whole-archive -lpthread -Wl,--no-whole-archive -static-libgcc
```

## User Features
* `cd <directory>`   -  The user provides the directory to move to. It accepts `.` and `..` as current and parent directories respectively and support changing multiple directories at once (cd ../../dir1/dir2). cd / takes you to the current user’s root directory. If a directory doesn't exist, the user stays in the current directory.
* `pwd`   - Print the current directory. Each user has /personal and /shared base directories. 
* `ls`   -  Lists the files and directories in the current directory separated by a new line. Shows the directories `.` and `..` as well. To differentiate between a file and a directory, the output look as follows:
  * d -> .
  * d -> ..
  * d -> directory1
  * f -> file1
* `cat <filename>`   - Reads the actual (decrypted) contents of the file. If the file doesn't exist, it prints "<filename> doesn't exist"
* `share <filename> <username>`   -  Shares the file with the target user which appears under the `/shared` directory of the target user. The files are shared only with read permission. The shared directory is read-only. If the file doesn't exist, it prints "File <filename> doesn't exist". If the user doesn't exist, it prints "User <username> doesn't exist".
* `mkdir <directory_name>`   - Creates a new directory. If a directory with this name exists, it prints "Directory already exists"
* `mkfile <filename> <contents>`   - Creates a new file with the contents. The contents are printable ascii characters. If a file with <filename> exists, it replaces the contents. If the file was previously shared, the target user will see the new contents of the file.
* `exit`   - Terminates the program.

## Admin Features
* Admin has access to read the entire file system with all user features
* `adduser <username>`  - This command creates a keyfile called username_keyfile on the host which is used by the user to access the filesystem. If a user with this name already exists, it prints "User <username> already exists"
  
<h2> Contributing </h2>

Fork it (https://github.com/Madhvik-P/CMPT785-BiBiFi) <br>
Create your feature branch (git checkout -b feature/fooBar) <br>
Commit your changes (git commit -am 'Add some fooBar') <br>
Push to the branch (git push origin feature/fooBar) <br>
Create a new Pull Request <br>
