default:
	cd ./src/rsdic/lib/; make
	cd ./src/lib/; make
	cd ./src/mbt/; make
	cd ./src/smbt+vla; make
	cd ./src/smbt+trie; make
	cd ./src/; make
	mv ./src/smbt-build ./prog/
	mv ./src/smbt-search ./prog/
	mv ./src/smbt-searchd ./prog/

clean:
	cd ./src/rsdic/lib/; make clean
	cd ./src/lib/; make clean
	cd ./src/mbt/; make clean
	cd ./src/smbt+vla/; make clean
	cd ./src/smbt+trie/; make clean
	cd ./src/; make clean
	rm -f ./prog/smbt-build
	rm -f ./prog/smbt-search
	rm -f ./prog/smbt-searchd
	rm -f index index.1 index.2 index.3

check:
	./prog/smbt-build -mode 1 ./dat/fingerprints.dat index.1
	./prog/smbt-build -mode 2 ./dat/fingerprints.dat index.2
	./prog/smbt-build -mode 3 ./dat/fingerprints.dat index.3
	./prog/smbt-search -similarity 0.8 index.1 ./dat/fingerprints.1.dat
	./prog/smbt-search -similarity 0.8 index.2 ./dat/fingerprints.1.dat
	./prog/smbt-search -similarity 0.8 index.3 ./dat/fingerprints.1.dat
