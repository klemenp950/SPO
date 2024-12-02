# Naloga 1
Napišite program, ki:
- prvih 5 sekund lovi signal SIGINT (rokovalna funkcija za signal nekaj izpiše in ponovno požene časovnik za neprespani čas – glej Primer 2),
- naslednjih 5 sekund ga ignorira (Primer 3),
- zadnjih 5 sekund pa izvaja privzeto akcijo za ta signal (Primer 4).
Signal SIGINT pošiljamo preko tipkovnice (CTRL+C). Opomba: Ni potrebno, da v teku prvih 5 minut lovi več kot en SIGINT.
Različni scenariji:

## Primer 1 (ni signala):
> Lovim signal SIGINT ...<br>
>Alarm v casu t = 5 ...<br>
>Ignoriram signal SIGINT ...<br>
>Alarm v casu t = 10 ...<br>
>Privzeta akcija za SIGINT ...<br>
>Alarm v casu t = 15 ...<br>

## Primer 2 (ulovljen signal):
>Lovim signal SIGINT ...<br>
>^CPrejel sem signal 2.<br>
>unslept_time = 3.<br>
>Alarm v casu t = 5 ...<br>
>Ignoriram signal SIGINT ...<br>
>Alarm v casu t = 10 ...<br>
>Privzeta akcija za SIGINT ...<br>
>Alarm v casu t = 15 ...<br>

## Primer 3 (ignoriran signal):
>Lovim signal SIGINT ...<br>
>Alarm v casu t = 5 ...<br>
>Ignoriram signal SIGINT ...<br>
>^CAlarm v casu t = 10 ...<br>
>Privzeta akcija za SIGINT ...<br>
>Alarm v casu t = 15 ...<br>

## Primer 4 (privzeta akcija):
>Lovim signal SIGINT ...<br>
>Alarm v casu t = 5 ...<br>
>Ignoriram signal SIGINT ...<br>
>Alarm v casu t = 10 ...<br>
>Privzeta akcija za SIGINT ...<br>
>^C

# Naloga 2
Svojo lupino (iz prejšnje naloge) razširite tako, da bo znala izvajati ukaze, ki so med sabo povezani preko cevi (pipe).
- Namesto znaka | bomo za cev uporabili niz =>
- Primer: `ps -ef => grep root` <br>
- Izvede se program ps z argumentom –ef in vse, kar izpiše na standardni izhod, se preusmeri na pisalni konec cevi. Bralni konec cevi pa se preveže na standardni vhod, iz katerega bere program grep.
- Omejite se na eno cev, torej: <ukaz1> => <ukaz2>
- Potrebovali bomo tri procese:
    - P1: izvaja zanko branja ukazov
    - P2: je otrok procesa P1 in izvede levo stran ukaza (<ukaz1>)
    - P3: je otrok procesa P2 in izvede desno stran ukaza (<ukaz2>)
- Cev naj obstaja med procesoma P2 in P3
- Uporabite dup2, da preusmerite std. vhod in izhod.