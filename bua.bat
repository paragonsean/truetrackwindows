cd include
del a:\include.zip
pkzip25 -add a:\include *.cpp *.h *.rc *.mak *.inl *.ico *.dsp *.dsw
cd..

cd subs
del a:\subs.zip
pkzip25 -add a:\subs *.cpp *.h *.rc *.mak *.inl *.ico *.dsp *.dsw
cd..

cd dundas
del a:\dundas.zip
pkzip25 -add a:\dundas *.cpp *.h *.rc *.mak *.inl *.ico *.dsp *.dsw
cd..

del a:uginc.zip
pkzip25 -add a:\ucinc c:\progra~1\msc6\vc98\include\ug*.h

cd bupres
del a:\bupres.zip
pkzip25 -add a:\bupres *.cpp *.h *.rc *.mak *.inl *.ico *.dsp *.dsw
cd..

cd downtime
del a:\downtime.zip
pkzip25 -add a:\downtime *.cpp *.h *.rc *.mak *.inl *.ico *.dsp *.dsw
cd..

cd editdown
del a:\editdown.zip
pkzip25 -add a:\editdown *.cpp *.h *.rc *.mak *.inl *.ico *.dsp *.dsw
cd..

cd editpart544
del a:\edpar544.zip
pkzip25 -add a:\edpar544 *.cpp *.rc *.h *.bat *.mak *.dsp *.dsw
cd..

cd netsetup
del a:\netsetup.zip
pkzip25 -add a:\netsetup *.cpp *.h *.rc *.mak *.inl *.ico *.dsp *.dsw
cd..

cd emachine
del a:\emachine.zip
pkzip25 -add a:\emachine *.cpp *.h *.rc *.mak *.inl *.ico *.dsp *.dsw
cd..

cd esensor
del a:\esensor.zip
pkzip25 -add a:\esensor *.cpp *.h *.rc *.mak *.inl *.ico *.dsp *.dsw
cd..

cd EventMan
del a:\EventMan.zip
pkzip25 -add a:\EventMan *.cpp *.h *.rc *.mak *.inl *.ico *.dsp *.dsw
cd..

cd fastrak
del a:\fastrak.zip
pkzip25 -add a:\fastrak *.cpp *.h *.rc *.mak *.inl *.ico *.dsp *.dsw
cd..

cd ftclasnt
del a:\ftclasnt.zip
pkzip25 -add a:\ftclasnt *.cpp *.h *.rc *.mak *.inl *.ico *.dsp *.dsw
cd..

cd Install
del a:\Install.zip
pkzip25 -add a:\Install *.cpp *.h *.rc *.mak *.inl *.ico *.dsp *.dsw
cd..

cd monall
del a:\monall.zip
pkzip25 -add a:\monall *.cpp *.h *.rc *.mak *.inl *.ico  *.dsp *.dsw
cd..

cd monedit
del a:\monedit.zip
pkzip25 -add a:\monedit *.cpp *.h *.rc *.mak *.inl *.ico *.dsp *.dsw
cd..

cd PartToV5
del a:\PartToV5.zip
pkzip25 -add a:\PartToV5 *.cpp *.h *.rc *.mak *.inl *.ico *.dsp *.dsw
cd..

cd profile
del a:\profile.zip
pkzip25 -add a:\profile *.cpp *.h *.rc *.mak *.inl *.ico *.dsp *.dsw
cd..

cd shiftrpt
del a:\shiftrpt.zip
pkzip25 -add a:\shiftrpt *.cpp *.h *.rc *.mak *.inl *.ico *.dsp *.dsw
cd..

cd startup
del a:\startup.zip
pkzip25 -add a:\startup *.cpp *.h *.rc *.mak *.inl *.ico *.dsp *.dsw
cd..

cd SureTrak544
del a:st544.zip
pkzip25 -add a:suretrak544 *.cpp *.rc *.h *.bat *.mak *.ico *.dsp *.dsw
cd..

cd v5setups
del a:\v5setups.zip
pkzip25 -add a:\v5setups *.cpp *.h *.rc *.mak *.ico *.bat *.dsp *.dsw
cd..

cd test
del a:\test.zip
pkzip25 -add a:\test *.cpp *.h *.rc *.mak *.inl *.ico *.bat *.dsp *.dsw
cd..

cd template
del a:\template.zip
pkzip25 -add a:\template *.cpp *.h *.rc *.mak *.ico *.bat *.dsp *.dsw
cd..

cd visiedit
del a:\visiedit.zip
pkzip25 -add a:\visiedit *.cpp *.h *.rc *.mak *.ico *.bat *.dsp *.dsw
cd..

copy ball.bat a:\
copy makeall.bat a:\
copy mscvars.bat a:\
copy bua.bat a:\
