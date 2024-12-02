# Naloga
Z metodo Monte Carlo izračunajte približek integrala 0∫𝜋 sin 𝑥 𝑑𝑥.
- Preberite iz ukazne vrstice število N in ustvarite toliko niti
- Vsaka nit lokalno računa približek integrala
    - Izvedite P (npr. 1e6) lokalnih (tj., v eni niti) poskusov:
        - Izberite naključno število x med 0 in 
        - Če je naključen y manjši od sin(x), inkrementirajte lokalni števec zadetkov Z
    - Lokalna Z in P prištejte globalnima: Zs += Z in Ps += P
        - Zs je skupno št. zadetkov, Ps pa skupno št. poskusov
        - globalne spremenljivke varujte z mutexom
        - Ko je Ps >= Ps_MAX (npr. 1e9), se nit konča, sicer se vrne na izvajanje P poskusov
Čas:
- gettime uporablja struct timespec, ki vsebuje tv_sec in tv_nsec
    - pretečen čas = (konec.tv_sec – zacetek.tv_sec) + (konec.tv_nsec – zacetek.tv_nsec)*1e-9
Preizkusite, koliko niti je najbolje izbrati