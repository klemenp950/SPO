# Naloga 1:
- Napišite v C-ju program tee1, ki deluje podobno kot tee (GNU utility).
- Standardni vhod prepiše na standardni izhod in v datoteko, ki jo podamo kot argument v ukazni vrstici.
- Če dodamo pred datoteko še opcijo -a, se standardni vhod doda k vsebini datoteke.
- Uporabite sistemske klice open, read, write in close.
# Naloga 2:
- Napišite C program rep, ki izpiše zadnjih n vrstic besedilne datoteke, katere ime je podano kot vhodni argument.
- Deluje podobno kot program (GNU utility) tail, a z manj opcijami.
- Primer klica programa: rep –n 3 a.txt
- Privzeta vrednost za n naj bo 5.
- Opomba: privzemite, da je datoteka lahko zelo velika, zato ne shranjujte celotne datoteke v pomnilnik (buffer).
- Nasvet: Pojdite najprej na konec z lseek, potem pa nazaj in iščite znake ’\n’.
- Uporabite le sistemske klice (in ne funkcij iz stdio).
