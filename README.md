# ČAS ZA OBRAČUN


## Ideja igre:

- Dogajanje se odvija na hodnikih Vegove, natančneje v prvem nadstropju. 
- Ravno je odzvonil zvonec in začel se je odmor za malico. 
- Vsi so lačni, vendar se izkaže, da je škatla z malico samo ena. Oh, ne!<br>
- Na eni strani hodnika ze zberejo profesorji, na drugi strani dijaki, vsaka od teh skupin pa želi imeti malico zase. 
- Prične se vnet boj za škatlo malice, ki jo zmagovalna ekipa odnese v svojo učilnico oz. zbornico, in se nasiti. 
- Ekipa poražencev pa mora iti po malico k najboljšemu sosedu, v Lidl ali Maksi.



## Delovanje:

- Uporabljena bo arhitektura server-client (strežnik-odjemalec)
- **Server** od clientov pridobiva podatke o njihovih dejanjih in vse skupaj procesira. Vodi tudi potek igre in njene faze. Clientom pošilja trenutno sliko, ki jo vidi določen igralec. 
- **Clienti** berejo vnose igralcev s tipkovnice in miške in pošljejo serverju. Nazaj dobijo vse podatke, ki so potrebni za prikaz ustrezne slike.
- Komunikacija med odjemalci in strežnikom bo (predvideno) izvedena preko protokola TCP. 



## Možnosti igralca:

- **Premikanje** s tipkovnico. Do neke mere bo možno spreminjanje nastavitev (npr. tipk za premikanje)
- **Gledanje okrog sebe** z miško. Z levim klikom vrže predmet proti nasprotniku (kreda ali svinčnik, odvisno kateri ekipi pripada)
- ***Mogoče:*** Ob desnem kliku na miško se prikaže črtkana črta, ki pomaga pri ciljanju
- **Vidno polje** Igralec lahko vidi povsod okrog sebe, razen če mu pogled zakriva objekt, ki stoji med njim in, na primer, drugim igralcem. Tako se bo možno skrivati pred nasprotniki.