
The file `hp3000Sim.zip` is downloaded from
https://bigdanzblog.wordpress.com/2018/05/21/instructions-for-emulating-an-hp3000-with-simh/
https://drive.google.com/file/d/1-bVJwoXlVK5CG7eFpQ3-K24ajyTeoPu4/view

Follow these instructions to build the hp3000 simulator:

```
wget http://simh.trailing-edge.com/hp/releases/HP3000-source-release-12.zip
unzip HP3000-source-release-12.zip
cd SCP
make -f makefile hp3000
cd ..
unzip hp3000Sim.zip
cd hp3000Sim
mv ../hp3000 .
./hp3000 hp.sim
```

(you should see a colon prompt; hit Ctrl+E)
(you should see a 'scp>' prompt; hit Ctrl+E)

```
attach ms0 advent.tap
go

HELLO MANAGER.SYS
FILE T;DEV=7
RESTORE *T;@.PUB.SYS;OLDDATE;SHOW;KEEP
(you should see a "Y/N?" prompt on the previous line; hit Ctrl+A)
(you should see a "=" prompt now)
REPLY 15,Y
BASIC
GET ADVENT
RUN
```
