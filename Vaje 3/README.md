# Naloga 1
- Napišite C program, ki uporablja različne velikosti medpomnilnika (‘bufferja’) pri izpisu podatkov v datoteko.
- Program naj s klicem funkcije fopen() ustvari dve datoteki za pisanje, "izhod1" in "izhod2".
- Pred pisanjem v datoteko "izhod1" nastavite velikost medpomnilnika na 10 bajtov. Uporabite funkcijo setvbuf(). Medpomnjenje naj bo polno (_IOFBF). Nato v to datoteko velikokrat (5 milijon-krat) izpišite vaš najljubši stavek.
- Podobno storite z drugo datoteko. Pred pisanjem vanjo spremenite velikost medpomnilnika na 100 bajtov. Zapišite enako količino podatkov kot v prvo datoteko.
- Merite čas zapisovanja v obe datoteki s funkcijo time(NULL) in izpišite rezultate. Kako velikost medpomnilnika vpliva na izvajalni čas?
- Kot vedno: obvezno preverjajte pravilnost izvršenih klicev funkcij in izpisujte morebitne napake.

# Naloga 2
- S pomočjo funkcij iz knjižnice stdio napišite preprost vrstični urejevalnik besedil (line editor) kamned (editor iz kamene dobe), ki podpira naslednje ukaze:
  - ./kamned <filename> -a - doda (append) na konec datoteke vrstice, ki jih uporabnik vnaša na stdin
  - ./kamned <filename> -d <n> - izbriše (delete) n-to vrstico (vrstice se začnejo z 1)
  - ./kamned <filename> -i <n> - vstavi (insert) n-to vrstico (vrstice se začnejo z 1)
- Uporabite funkcije fopen, fclose, fgets, fputs, ...
- Lahko tudi unlink in rename, če boste pisali v drugo datoteko (lahko pa tudi v isto)
