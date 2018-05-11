# Secure Interactive Shell

## Environment
* Run only on Linux
* Must use c++11 to compile
* Using xinetd to run

## How to Use
```
make clean && make
make install
cp xinetd.cfg /etc/xinetd.d/sish
systemctl restart xinetd
```
* xinetd.cfg is xinetd config file and copy it to /etc/xinetd.d/
* make -> generate exec file "shell"
* make install -> copy shell to /home/user/tmproot/bin but this path must exist
* make clean -> rm all *.o file and exec file that this Makefile generating

## Command 
```
cat {file}:              Display content of {file}.
cd {dir}:                Switch current working directory to {dir}.
chmod {mode} {file/dir}: Change the mode (permission) of a file or directory.
                         {mode} is an octal number.
echo {str} [filename]:   Display {str}. If [filename] is given,
                         open [filename] and append {str} to the file.
exit:                    Leave the shell.
find [dir]:              List files/dirs in the current working directory
                         or [dir] if it is given.
                         Outputs contatin file type, Permission, size, and name.
help:                    Display help message.
id:                      Show current euid and egid number.
mkdir {dir}:             Create a new directory {dir}.
pwd:                     Print the current working directory.
rm {file}:               Remove a file.
rmdir {dir}:             Remove an empty directory.
stat {file/dir}:         Display detailed information of the given file/dir.
                         Output is similar to stat on bash
touch {file}:            Create {file} if it does not exist,
                         or update its access and modification timestamp.
umask {mode}:            Change the umask of the current session.
```