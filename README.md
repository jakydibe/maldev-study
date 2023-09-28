# maldev-study

1) Compile.bat e' solamente il file per compilare easy_dropper.cpp
   
2) xorencrypt.py e' uno scriptino in python che cripta il payload craftato da msfvenom Xorandolo con una chiave e ritorna un array che si puo' copiare e incollare in un linguaggio C-style

3) easy_dropper.cpp e' il codice sorgente del malware di test

Il malware funziona cosi': 
  1)Alloca memoria in Read Write
  2) Copia il Payload nella memoria allocata
  3) Rende la memoria allocata eseguibile
  4)Starta un Thread che esegue lo shellcode   (LPTHREAD_START_ROUTINE) exec_mem,

Per evitare controlli degli AV sono implementati i seguenti metodi:
  1) Allocare la memoria prima in Read-Write e poi in Executable, perche' tutto insieme insospettisce gli AV
  2) Il payload lo salviamo XORato, evitando cosi' che l' analisi statica degli AV rilevi un noto payload (meterpreter) e poi lo decriptiamo a runtime
  3) Le 3 funzioni piu' 'sospette' [1)"VirtualAlloc" 2)"VirtualProtect" 3)"RtlMoveMemory"]  non le utilizziamo direttamente per evitare che compaiano nella Import Address Table
     Ma troviamo un puntatore a quelle funzioni con la funzione GetProcAddress() e le chiamiamo usando il puntatore. ----------> Questo metodo e' detto Function Call Obfuscation
