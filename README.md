# VUT FIT BIT Základy programování 2018

[![Build Status](https://travis-ci.com/Roman3349/FIT-BIT-IZP-2018.svg?token=7VpMcGETTy8vt99girhh&branch=master)](https://travis-ci.com/Roman3349/FIT-BIT-IZP-2018)

## Upozornění

- Zdrojové kódy slouží pouze ke studijním účelům - **plagiátorství je na VUT FIT přísně trestáno**.
- Zdrojové kódy jsou v repozitáři nahrány ve stavu, ve kterém byly při odevzdání. Tomu odpovídá i jejich kvalita, proto rozhodně nemohou sloužit jako ukázka způsobu řešení.

## Hodnocení

### [První projekt](https://wis.fit.vutbr.cz/FIT/st/cwk.php?title=Projekt1&csid=681530&id=12816) (4,7/5,0)

| Počet bodů | Stav   | Popis                   |
| :--------: | :----: | ----------------------- |
| 2.5        |        | hodnoceni obhajoby      |
| 0.1        | ok     | prikaz insert1 (i)      |
| 0.1        | ok     | prikaz insert2 (i)      |
| 0.1        | ok     | prikaz next1 (n)        |
| 0.2        | ok     | prikaz next2 (n)        |
| 0.1        | ok     | prikaz quit (q)         |
| 0.1        | ok     | prikaz delete1 (d)      |
| 0.1        | ok     | prikaz delete2 (d)      |
| 0          | spatne | prikaz after (a)        |
| 0          | spatne | prikaz before (b)       |
| 0.2        | ok     | prikaz goto (g)         |
| 0.1        | ok     | prikaz removeEOL (r)    |
| 0          | spatne | prikaz substituce1 (s)  |
| 0          | spatne | prikaz substituce2 (s)  |
| 0          | spatne | prikaz substituce3 (S)  |
| 0.5        | ok     | prikaz appendEOF (e)    |
| 0          | spatne | prikaz find (f)         |
| 0          | spatne | prikaz cond_goto (c)    |
| 0          | chyba  | detekce cyklu1          |
| 0          | chyba  | detekce cyklu2          |
| 0.4        | ok     | detekce cyklu3          |
| 0.05       | ok     | reakce na napovedu      |
| 0.05       | ok     | reakce na napovedu      |
| 0.1        | ok     | chybny prikaz v souboru |

### [Druhý projekt](https://wis.fit.vutbr.cz/FIT/st/cwk.php?title=Projekt2&csid=681530&id=12816) (6,9/7,0)

| Počet bodů | Stav   | Popis                                                       |
| :--------: | :----: | ----------------------------------------------------------- |
| 3.5        |        | hodnoceni obhajoby                                          |
| 0.3        | ok     | prime volani funkce taylor_log pro x=0.5,1,2 a n=1,2,10     |
| 0.3        | ok     | prime volani funkce cfrac_log pro x=0.5,1,2 a n=1,2,10      |
| 0.3        | ok     | --log 1.131401114526 40                                     |
| 0.2        | ok     | --log 1.0 40                                                |
| 0.2        | ok     | --log 1e-6 40                                               |
| 0.2        | ok     | --log 1e+6 40                                               |
| 0.0875     | ok     | --log 0.0 5                                                 |
| 0.0875     | ok     | --log inf 5                                                 |
| 0.3        | ok     | prime volani funkce taylor_pow pro x,y=0.5,1,2 a n=1,2,10   |
| 0.3        | ok     | prime volani funkce taylorcf_pow pro x,y=0.5,1,2 a n=1,2,10 |
| 0.3        | ok     | --pow 1.23 4.2 40                                           |
| 0.2        | ok     | --pow 1.0 1.0 40                                            |
| 0          | spatne | --pow 1e-6 4.2 40                                           |
| 0.2        | ok     | snizena presnost                                            |
| 0.2        | ok     | --pow 1e+6 4.2e1 40                                         |
| 0.0875     | ok     | --pow inf 1.0 4                                             |
| 0.0875     | ok     | --pow 1.1 inf 4                                             |
| 0          | chyba  | reakce na neocekavany argument                              |
| 0          | chyba  | reakce na nekompletni argumenty                             |
| 0.05       | ok     | reakce na chybne argumenty                                  |

### [Druhý projekt](https://wis.fit.vutbr.cz/FIT/st/cwk.php?id=12816&csid=681530) (/10,0)


## Licence

Repozitář je šířen pod licencí [GPLv2](LICENSE).