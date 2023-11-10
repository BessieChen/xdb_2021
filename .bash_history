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
