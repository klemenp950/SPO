# Naloga 1
- Napišite v C-ju poenostavljeno verzijo ukaza ls -l, ki izpiše imena vsehdatotek v trenutnem imeniku ("."), kakor tudi tip datoteke (običajna,imenik, vtičnica, znakovna, bločna, itd.), dostopna dovoljenja (mode) in število trdih povezav (st_nlink).
- Ne pozabite na S_ISUID, S_ISGID in S_ISVTX.
- Uporaba "exec ls –l" ni dovoljena.
- Primer izpisa:  
*-rwsr-Sr-x 1 lsl   
-rw-r--r-- 1 lsl.c  
drwxr-xr-t 2 .  
drwxr-xr-x 6 ..  
-rw------T 1 stevila*

# Naloga 2
- Napišite program mojChmod, ki bo deloval podobno kot terminalski ukaz chmod.
- Ob zagonu programa podamo dva argumenta:
  - nova dovoljenja (mode) in
  - ime datoteke, katere dovoljenja spreminjamo.
- Dovoljenja podamo kot osmiško število (npr. 0777, 0644, 4700) ali pa kot tri-črkovni niz. Dovolimo zgolj štiri oblike nizov:
  - u+s (postavi bit "Set-User-ID")
  - u-s (pobriši bit "Set-User-ID")
  - g+s (postavi bit "Set-Group-ID")
  - g-s (pobriši bit "Set-Group-ID")
- Program naj spremeni dovoljenja podane datoteke in ob izhodu izpiše nova dovoljenja.
- Primer delovanja programa:  
*$ ls -la test  
-rwxr----- 1 student student 0 2022-11-05 10:47 test**  
*$ ./mojChmod 0664 test  
Nova dovoljenja: rw-rw-r--  
$ ./mojChmod g+s test  
Nova dovoljenja: rw-rwSr--  
$ ./mojChmod 4700 test  
Nova dovoljenja: rws------  
$ ./mojChmod u-s test  
Nova dovoljenja: rwx------**  
