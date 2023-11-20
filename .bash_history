#1699597342
cd test202311/app
#1699597344
pwd
#1699597374
g++ -o demo01 demo01.cpp /home/xdb/test202311/tools/public.cpp
#1699597378
./demo01
#1699597410
cd test202311/tools
#1699597412
ll
#1699597426
g++ -c -o libpublic.a public.cpp
#1699597427
;;
#1699597429
ll
#1699597475
g++ -o demo01 demo01.cpp /home/xdb/test202311/tools/libpublic.a 
#1699597479
./demo01 
#1699597528
g++ -o demo01 demo01.cpp -L/home/xdb/test202311/tools -lpublic
#1699597537
./demo01
#1699597620
pwd
#1699597635
g++ -fPIC -shared -o libpublic.so public.cpp
#1699597636
ll
#1699597648
rm libpublic.a
#1699597687
g++ -o demo01 demo01.cpp /home/xdb/test202311/tools/libpublic.so 
#1699597690
./demo01
#1699597741
g++ -o demo01 demo01.cpp -L/home/xdb/test202311/tools -lpublic
#1699597751
./demo01
#1699597762
echo $LD_LIBRARY_PATH
#1699597805
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/home/xdb/test202311/tools
#1699597808
echo $LD_LIBRARY_PATH
#1699597817
./demo01
#1699597938
./demo01
#1699597964
g++ -fPIC -shared -o libpublic.so public.cpp
#1699597969
./demo01
#1699605767
git config -l --show-origin
#1699605780
git
#1699605784
git -v
#1699606137
ll
#1699606141
ll -a
#1699606154
cd .git
#1699606156
ll -a
#1699606163
cd config
#1699606174
vim config
#1699606193
git config --global user.name "BessieChen"
#1699606198
git config --global user.email bc2535@nyu.edu
#1699606425
pwd
#1699830681
gdb -v
#1699830880
cd /home/xdb/test202311/app
#1699830882
ll
#1699830915
g++ -o demo02 demo02.cpp
#1699830923
./demo2 a b c
#1699830931
./demo02 a b c
#1699830949
./demo02 a b 2
#1699830963
g++ -o demo02 demo02.cpp
#1699831016
gdb demo02
#1699831047
g++ -o demo02 demo02.cpp -g
#1699831053
gdb demo02
#1699834002
g++ -o demo02 demo02.cpp -g
#1699834008
cd /home/xdb/test202311/app
#1699834009
g++ -o demo02 demo02.cpp -g
#1699834018
gdb demo2
#1699834160
pwd
#1699834164
g++ -o demo02 demo02.cpp -g
#1699834169
gdb demo02
#1699851282
g++ -o d3 d3.cpp -g
#1699851287
cd /home/xdb/test202311/app
#1699851288
g++ -o d3 d3.cpp -g
#1699851298
cd /home/xdb/test202311/app
#1699851299
g++ -o d3 d3.cpp -g
#1699851309
./d3
#1699851315
ulimit -a
#1699851537
ulimit -c unlimited 
#1699851539
ulimit -a
#1699851543
./d3
#1699851546
ll
#1699851566
gdb d3 core.29128
#1699851873
g++ -o d3 d3.cpp -g
#1699851877
./d3
#1699851879
ll
#1699851889
gdb d3 core.
#1699851907
gdb d3 core.31636 
#1699852356
clear
#1699852364
g++ -o d4 d4.cpp -g
#1699852387
grep ps | -ef d4
#1699852435
ps -ef | grep d4
#1699852447
gdb d4 2891
#1699852469
gdb d4 -p 2891
#1699852369
./d4
#1699920280
cd /tmp
#1699920287
ll
#1699920298
sudo tar zxvf project_20230727.tgz
#1699920309
sudo tar zxvf project_20230727.tgz
#1699920962
cd /home/xdb/project_final
#1699920994
cd /home/xdb/project_final/public/demo
#1699920998
./demo10
#1699922757
./d
#1699922760
./demo20
#1699930231
./demo22
#1699930383
./demo23
#1699920213
su
#1699920348
su
#1700093681
cd /home/xdb/project_final/public/demo/demo24.cpp
#1700093687
cd /home/xdb/project_final/public/demo
#1700093690
./demo24
#1700093960
./demo26
#1700094865
./demo28
#1700094899
./demo29
#1700096945
cd /home/xdb/project_final/public/demo
#1700096945
ll
#1700096996
cd /home/xdb/project_final/public/demo
#1700096998
ll
#1700097038
chmod 757 demo31
#1700097047
chmod 757 demo30.cpp
#1700097087
su vim demo30.cpp
#1700096957
su
#1700097096
vim demo30.cpp
#1700097190
cd /home/xdb/project_final/public/demo
#1700097199
./demo30
#1700097203
cd /tmp
#1700097206
ll
#1700100998
rm aaa
#1700101002
rm -f aaa
#1700101018
rm -rf aaa
#1700101020
rm -rf 111
#1700101029
cd /home/xdb/project_final/public/demo
#1700101046
./demo34 /home/xdb/project_final/public "*.h"
#1700101522
cd /home/xdb/test202311/app
#1700101546
g++ -o demo36 demo36.cpp 
#1700101581
g++ -o demo36 demo36.cpp 
#1700101650
g++ -o demo36 demo36.cpp /home/xdb/project_final/public/_public.cpp
#1700101675
cd /tmp/data
#1700101676
ll
#1700101680
ll
#1700101671
./demo36
#1700101730
ll
#1700102497
cat girl.xml
#1700102734
cd /home/xdb/project_final/public/demo
#1700102737
./demo37
#1700103893
./demo38
#1700103905
ll
#1700103908
cat /tmp/data/girl.dat
#1700104057
./demo39
#1700104321
cd /tmp/log
#1700104284
./demo42
#1700104322
ll
#1700104325
ll
#1700104339
head demo42.log 
#1700104366
cd ..
#1700104367
ll
#1700104374
rm -rf data log
#1700104375
ll
#1700107745
g++ -g -o demo42 demo42. cpp ../_public.cpp -lpthread
#1700107753
g++ -g -o demo42 demo42.cpp ../_public.cpp -lpthread
#1700107809
cd /home/xdb/test202311/app
#1700107810
g++ -g -o demo42 demo42.cpp ../_public.cpp -lpthread
#1700107823
g++ -g -o demo42 demo42.cpp /home/xdb/project_final/public/_public.cpp -lpthread
#1700107853
g++ -g -o demo42 demo42.cpp /home/xdb/project_final/public/_public.cpp -lpthread
#1700116093
ll
#1700116105
g++ -g -o demo42 demo42.cpp /home/xdb/project_final/public/_public.cpp -lpthread
#1700116215
cd /home/xdb/project_final/public/demo
#1700116223
ll *42.cpp
#1700116434
chmod 766 demo42.cpp
#1700116438
sudo chmod 766 demo42.cpp
#1700116449
su chmod 766 demo42.cpp
#1700116819
cd /home/xdb/project_final/public/demo/demo42.cpp
#1700116827
cd /home/xdb/project_final/public/demo
#1700116850
g++ -g -o demo42 demo42.cpp ../_public.cpp -lpthread
#1700116964
g++ -o demo42 demo42.cpp ../_public.cpp -lpthread
#1700116994
g++ -o demo422 demo42.cpp ../_public.cpp -lpthread
#1700117048
ls
#1700117083
g++ demo42.cpp ../_public.cpp -o demo422 -lpthread
#1700117104
g++ demo42.cpp ../_public.cpp -o demo422
#1700117440
g++ -o demo42 demo42.cpp ../_public.cpp -lpthread
#1700117444
cd /home/xdb/project_final/public/demo
#1700117447
g++ -o demo42 demo42.cpp ../_public.cpp -lpthread
#1700117520
ll
#1700117522
cd log
#1700117523
ll
#1700117539
ll
#1700117543
ll
#1700117545
ll
#1700117546
ll
#1700117504
./demo42
#1700117579
ll
#1700117581
ll
#1700117656
ll
#1700117689
rm demo42.log 
#1700117692
ll
#1700117707
g++ -g -o demo42 demo42.cpp ../_public.cpp -lpthread
#1700117830
gdb ./demo42
#1700118028
g++ -g -o demo42 demo42.cpp ../_public.cpp -lpthread
#1700118041
pwd
#1700118042
ll
#1700118048
ll
#1700118054
ll
#1700118035
./demo42
#1700122856
ll
#1700128071
rm *
#1700128076
g++ -o demo42 demo42.cpp ../_public.cpp -lpthread
#1700128080
./demo42
#1700128098
rm *
#1700128110
g++ -o demo42 demo42.cpp ../_public.cpp -lpthread
#1700128151
ll
#1700128114
./demo42
#1700128220
ll
#1700128227
wc *
#1700128304
pwd
#1700128307
rm *
#1700128354
pwd
#1700128402
g++ -o demo42 demo42.cpp ../_public.cpp -lpthread
#1700128410
./demo42
#1700128491
ll
#1700128497
wc *
#1700130420
cat demo42.log 
#1700130535
g++ -o demo42 demo42.cpp ../_public.cpp -lpthread
#1700130574
g++ -o demo42 demo42.cpp ../_public.cpp -lpthread
#1700130586
rm *
#1700130604
ll
#1700130437
vim demo42.log 
#1700130623
ll
#1700130624
rm (
#1700130627
rm *
#1700130635
./demo42
#1700130878
ll
#1700130892
head demo42.log.20231116183111
#1700131425
g++ -o demo42 demo42.cpp ../_public.cpp -lpthread
#1700131431
ll
#1700131458
rm *
#1700131492
ll
#1700131560
ll
#1700131470
./demo42
#1700131569
wc *
#1700131575
wc *
#1700131751
rm *
#1700131766
g++ -o demo42 demo42.cpp ../_public.cpp -lpthread
#1700131769
./demo42
#1700131888
ll
#1700131891
wc *
#1700131927
vi demo42.log 
#1700131980
rm *
#1700179004
clear
#1700179016
./demo44
#1700179037
./demo45
#1700179042
clear
#1700179052
cd /home/xdb/project_final/public/demo
#1700179056
./demo45
#1700179024
./demo44 192.168.150.128 5005
#1700179081
./demo44 192.168.150.128 5005
#1700179060
./demo45 5005
#1700179138
ps -ef | grep
#1700179140
ps -ef
#1700179163
clear
#1700179167
ipconfig
#1700179169
ip
#1700179207
clear
#1700179228
./demo44 127.0.0.1 5005
#1700179217
./demo45 5005
#1700182386
clear
#1700182400
./demo46 127.0.0.1 5005
#1700182381
./demo47 5005
#1700186862
cd /home/xdb/test202311/idc/cpp
#1700186866
make
#1700186893
clear
#1700186894
make
#1700186922
make
#1700186943
clear
#1700186945
make
#1700186959
make
#1700186997
./crtsurfdata 
#1700187267
cd /home/xdb/project
#1700187502
cd /home/xdb/project/idc/cpp
#1700187504
make
#1700187554
cd ../bin
#1700187559
./crtsurfdata /home/xdb/project/idc/ini/stcode.ini /tmp/idc/surfdata /log/idc/crtsurfdata.log
#1700187574
./crtsurfdata 
#1700187628
cd /tmp/idc
#1700187634
cd /log
#1700187681
killall crtsurfdata
#1700187658
./crtsurfdata /home/xdb/project/idc/ini/stcode.ini /tmp/idc/surfdata /home/xdb/project/idc/log/idc/crtsurfdata.log
#1700187750
cd /home/xdb/project/idc/cpp
#1700187757
make clean
#1700189393
make
#1700189423
cd /home/xdb/project/idc/log
#1700189425
rm *
#1700189433
ll
#1700189443
cd idc
#1700189446
pwd
#1700189451
ll
#1700189453
rm *
#1700189462
cd cd /home/xdb/project/idc/cpp
#1700189467
cd /home/xdb/project/idc/cpp
#1700189469
make
#1700189492
./crtsurfdata /home/xdb/project/idc/ini/stcode.ini /tmp/idc/surfdata /home/xdb/project/idc/log/idc/crtsurfdata.log
#1700190086
make
#1700190092
./crtsurfdata /home/xdb/project/idc/ini/stcode.ini /tmp/idc/surfdata /home/xdb/project/idc/log/idc/crtsurfdata.log
#1700190178
make
#1700190181
./crtsurfdata /home/xdb/project/idc/ini/stcode.ini /tmp/idc/surfdata /home/xdb/project/idc/log/idc/crtsurfdata.log
#1700191632
wc /home/xdb/project/idc/log/idc/crtsurfdata.log
#1700192495
make
#1700192499
./crtsurfdata /home/xdb/project/idc/ini/stcode.ini /tmp/idc/surfdata /home/xdb/project/idc/log/idc/crtsurfdata.log
#1700206398
make
#1700206413
rm *
#1700206416
ll
#1700206430
cd /home/xdb/project/idc/log/idc
#1700206431
ll
#1700206433
rm *
#1700206436
./crtsurfdata /home/xdb/project/idc/ini/stcode.ini /tmp/idc/surfdata /home/xdb/project/idc/log/idc/crtsurfdata.log
#1700206451
./crtsurfdata /home/xdb/project/idc/ini/stcode.ini /tmp/idc/surfdata /home/xdb/project/idc/log/idc/crtsurfdata.log csv,xml,json
#1700206529
./crtsurfdata /home/xdb/project/idc/ini/stcode.ini /tmp/idc/surfdata /home/xdb/project/idc/log/idc/crtsurfdata2.log json
#1700206823
pwd
#1700206824
make
#1700206840
cd /tmp/idc/surfdata
#1700206840
ll
#1700206844
rm *
#1700206845
pwd
#1700206871
./crtsurfdata /home/xdb/project/idc/ini/stcode.ini /tmp/idc/surfdata /home/xdb/project/idc/log/idc/crtsurfdata.log csv,xml,json
#1700206902
ll
#1700206924
head *.csv
#1700206941
head *.json
#1700206963
head *.xml
#1700209718
clear
#1700209721
cd /home/xdb/project
#1700209726
mkdir tools
#1700209729
cd tools
#1700209735
mkdir cpp bin log
#1700209942
cd ../cpp
#1700209946
cd ..
#1700209955
cd tools/cpp
#1700209958
make
#1700210001
./procctl 
#1700210081
make
#1700210087
./procctl 
#1700210093
/home/xdb/project/tools/bin/procctl 10 /usr/bin/tar zcvf /tmp/tmp.tgz /usr/include
#1700210097
cd /tmp
#1700210098
ll
#1700210168
ll tmp.tgz
#1700210179
rm tmp.tgz ; ll tmp.tgz 
#1700210182
rm tmp.tgz ; ll tmp.tgz 
#1700210196
ll tmp.tgz 
#1700210316
rm tmp.tgz 
#1700210326
rm -rf idc log
#1700210327
ll
#1700210340
cd /home/xdb/project/tools/cpp
#1700210353
./procctl 60 /home/xdb/project/idc/bin/crtsurfdata /home/xdb/project/idc/ini/stcode.ini /tmp/idc/surfdata /home/xdb/project/tools/log/idc/crtsurfdata.log csv,xml,json
#1700210454
ps -ef | grep procctl
#1700210466
kill -9 6143
#1700210471
kill -9 8151
#1700210473
ps -ef | grep procctl
#1700210558
cd /tmp/idc/surfdata
#1700210559
ll
#1700210829
cd /tmp
#1700210830
ll
#1700210839
rm -rf idc
#1700210841
ll
#1700116452
su
#1700117412
su
#1700270820
ps -ef | grep xdb
#1700270853
ps -ef | grep crtsurfdata
#1700270869
killall crtsurfdata
#1700270884
kill -9 31067
#1700270911
ps -ef | grep tar
#1700270940
ps -ef | grep tar
#1700270956
ps -ef | grep crtsurfdata
#1700270972
kill -9 crtsurfdata
#1700270978
killall crtsurfdata
#1700271003
kill -9 32106
#1700271554
cd /home/xdb/project/tools/cpp
#1700271557
./procctl 
#1700271618
g++ -o server server1.cpp
#1700271633
./procctl 10 ./server
#1700271652
ps -ef | grep server
#1700271665
ps -ef | grep server
#1700271696
ps -ef | grep server
#1700271714
ps -ef | grep server | grep xdb
#1700271735
kill -9 5016
#1700271737
kill -9 5020
#1700271742
ps -ef | grep server
#1700271753
rm server
#1700271775
g++ -g jialiping server1.cpp
#1700271785
g++ -o jialiping server1.cpp
#1700271797
./procctl 5 ./jialiping 
#1700271802
ps -ef | grep jialiping 
#1700271818
ps -ef | grep jialiping 
#1700271845
ps -ef | grep jialiping 
#1700271855
kill -9 6639
#1700271857
kill -9 6640
#1700271859
ps -ef | grep jialiping 
#1700271867
rm jialiping 
#1700364663
su -
#1700443412
cd /home/bftp
#1700445808
cd /tmp/list
#1700445810
ll
#1700445814
cat tmp.list
#1700445836
ftp 139.159.187.232
#1700445990
ftp 139.159.187.232
#1700466123
cd /home/xdb/project/public/demo
#1700466124
ll
