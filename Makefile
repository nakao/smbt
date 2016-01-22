default:
	cd ./src/rsdic/lib/; make
	cd ./src/lib/; make
	cd ./src/mbt/; make
	cd ./src/smbt+vla; make
	cd ./src/smbt+trie; make
	cd ./src/; make
	mv ./src/smbt-build ./prog/
	mv ./src/smbt-search ./prog/

clean:
	cd ./src/rsdic/lib/; make clean
	cd ./src/lib/; make clean
	cd ./src/mbt/; make clean
	cd ./src/smbt+vla/; make clean
	cd ./src/smbt+trie/; make clean
	cd ./src/; make clean
	rm -f ./prog/smbt-build
	rm -f ./prog/smbt-search
