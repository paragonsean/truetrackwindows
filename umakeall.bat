cd subs
del /Q ufree\*.*
nmake -fufree.mak
cd..

cd dundas
del /Q ufree\*.*
nmake -fufree.mak
cd..

cd bupres
del /Q ufree\*.*
nmake -fufree.mak RCFILE="bupres%1.rc"
cd..

cd copylog
del /Q ufree\*.*
nmake -fufree.mak
cd..

cd downtime
del /Q ufree\*.*
nmake -fufree.mak RCFILE="downtime%1.rc"
cd..

cd editdown
del /Q ufree\*.*
nmake -fufree.mak RCFILE="editdown%1.rc"
cd..

cd editpart544
del /Q ufree\*.*
nmake -fufree.mak RCFILE="editpart%1.rc"
cd..

cd emachine
del /Q ufree\*.*
nmake -fufree.mak RCFILE="emachine%1.rc"
cd..

cd esensor
del /Q ufree\*.*
nmake -fufree.mak RCFILE="esensor%1.rc"
cd..

cd eventman
del /Q ufree\*.*
nmake -fufree.mak RCFILE="eventman%1.rc"
cd..

cd fastrak
del /Q ufree\*.*
nmake -fufree.mak
cd..

cd ft2
del /Q ufree\*.*
nmake -fufree.mak RCFILE="ft2%1.rc"
cd..

cd ftclasnt
del /Q ufree\*.*
nmake -fufree.mak
cd..

copy ftclasnt\ufree\ftclass.lib ufreelib

cd Install
del /Q ufree\*.*
nmake -fufree.mak RCFILE="setup%1.rc"
cd..

cd monall
del /Q ufree\*.*
nmake -fufree.mak RCFILE="monall%1.rc"
cd..

cd monedit
del /Q ufree\*.*
nmake -fufree.mak RCFILE="monedit%1.rc"
cd..

cd profile
del /Q ufree\*.*
nmake -fufree.mak RCFILE="profile%1.rc"
cd..

cd shiftrpt
del /Q ufree\*.*
nmake -fufree.mak RCFILE="shiftrpt%1.rc"
cd..

cd startup
del /Q ufree\*.*
nmake -fufree.mak
cd..

cd stddown
del /Q ufree\*.*
nmake -fufree.mak RCFILE="stddown%1.rc"
cd..

cd suretrak544
del /Q ufree\*.*
nmake -fufree.mak RCFILE="suretrak%1.rc"
cd..

cd v5setups
del /Q ufree\*.*
nmake -fufree.mak RCFILE="v5setups%1.rc"
cd..

cd visiedit
del /Q ufree\*.*
nmake -fufree.mak RCFILE="visiedit%1.rc"
cd..

cd vtextrac
del /Q ufree\*.*
nmake -fufree.mak RCFILE="vtextrac%1.rc"
cd..
