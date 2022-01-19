czerwien = load("czerwien.txt");
zolty = load("zolty.txt");
zielony = load("zielen.txt");
niebieski = load("niebieski.txt");
fioletowy = load("fioletowy.txt");
blekit = load("blekit.txt");
y = [fioletowy(10),niebieski(10),blekit(10),zielony(10),zolty(10),czerwien(10)];
x = [400,450,486,500,580,650];
p = polyfit(x,y,2);
Yn = polyval(p,x);
plot(x,Yn);
xlabel('Dlugosc fali [nm]');
ylabel('Nate�enie �wiat�a [lux]');
title('Wykres natezenia swiatla w zaleznosci od dlugosci fali');