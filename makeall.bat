cd subs
del /Q free\*.*
nmake -ffree.mak
cd..

cd dundas
del /Q free\*.*
nmake -ffree.mak
cd..

cd bupres
del /Q free\*.*
nmake -ffree.mak RCFILE="bupres%1.rc"
cd..

cd copylog
del /Q free\*.*
nmake -ffree.mak RCFILE="copylog%1.rc"
cd..

cd demo
del /Q free\*.*
nmake -ffree.mak
cd..

cd downtime
del /Q free\*.*
nmake -ffree.mak RCFILE="downtime%1.rc
cd..

cd dsbackup
del /Q free\*.*
nmake -ffree.mak RCFILE="dsbackup%1.rc"
cd..

cd dsrestor
del /Q free\*.*
nmake -ffree.mak RCFILE="dsrestor%1.rc"
cd..

cd editdown
del /Q free\*.*
nmake -ffree.mak RCFILE="editdown%1.rc"
cd..

cd editpart544
del /Q free\*.*
nmake -ffree.mak RCFILE="editpart%1.rc"
cd..

cd emachine
del /Q free\*.*
nmake -ffree.mak RCFILE="emachine%1.rc"
cd..

cd esensor
del /Q free\*.*
nmake -ffree.mak RCFILE="esensor%1.rc"
cd..

cd eventman
del /Q free\*.*
nmake -ffree.mak RCFILE="eventman%1.rc"
cd..

cd fastrak
del /Q free\*.*
nmake -ffree.mak
cd..

cd ft2
del /Q free\*.*
nmake -ffree.mak RCFILE="ft2%1.rc"
cd..

cd ft95
del /Q free\*.*
nmake -ffree.mak
cd..

cd ftclas95
del /Q free\*.*
nmake -ffree.mak
cd..

cd ftclasnt
del /Q free\*.*
nmake -ffree.mak
cd..

cd Install
del /Q free\*.*
nmake -ffree.mak RCFILE="setup%1.rc"
cd..

cd monall
del /Q free\*.*
nmake -ffree.mak RCFILE="monall%1.rc"
cd..

cd monedit
del /Q free\*.*
nmake -ffree.mak RCFILE="monedit%1.rc"
cd..

cd netsetup
del /Q free\*.*
nmake -ffree.mak RCFILE="netsetup%1.rc"
cd..

cd profile
del /Q free\*.*
nmake -ffree.mak RCFILE="profile%1.rc"
cd..

cd shiftrpt
del /Q free\*.*
nmake -ffree.mak RCFILE="shiftrpt%1.rc"
cd..

cd startup
del /Q free\*.*
nmake -ffree.mak
cd..

cd stddown
del /Q free\*.*
nmake -ffree.mak RCFILE="stddown%1.rc"
cd..

cd SureTrak544
del /Q free\*.*
nmake -ffree.mak RCFILE="suretrak%1.rc"
cd..

cd v5setups
del /Q free\*.*
nmake -ffree.mak RCFILE="v5setups%1.rc"
cd..

cd visiedit
del /Q free\*.*
nmake -ffree.mak RCFILE="visiedit%1.rc"
cd..

cd vtextrac
del /Q free\*.*
nmake -ffree.mak RCFILE="vtextrac%1.rc"
cd..