chinese.seg.txt: chinese.raw.txt ictclas
	./ictclas < chinese.raw.txt > chinese.seg.txt

ictclas: demo/main.cpp lib/ictclas.a
	g++ -o ictclas -I./inc demo/main.cpp lib/ictclas.a

lib/ictclas.a: src/*.cpp
	cd src; g++ -I../inc -c *.cpp
	cd src; ar -r ictclas.a *.o
	rm -rf lib
	mkdir  lib
	mv src/ictclas.a lib/

.PHONY : clean
clean :
	rm -f ictclas
	rm -f chinese.seg.txt
	rm -f source/*.o
	rm -rf lib
	find ./ -name '*~'           -exec rm -rf {} \;
	find ./ -name '*.swp'        -exec rm -rf {} \;
	find ./ -name '.*.d*'        -exec rm -rf {} \;
	find ./ -name 'cscope.*'     -exec rm -rf {} \;
	find ./ -name 'ncscope.*'    -exec rm -rf {} \;
	find ./ -name '*.o'          -exec rm -rf {} \;
	find ./ -type f -name 'tags' -exec rm -rf {} \;
