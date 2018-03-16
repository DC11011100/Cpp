build_FRMachine: build_FRMachine.cpp
	g++ -stdlib=libc++ -Wc++11-extensions -ggdb `pkg-config --cflags --libs opencv` build_FRMachine.cpp -o build_FRMachine
build_revision: revision.cpp
	g++ -std=c++11 -stdlib=libc++ -Wc++11-extensions -ggdb `pkg-config --cflags --libs opencv` revision.cpp -o revision
clean:
	rm -rf ./build_FRMachine ./build_FRMachine.dSYM ./revision ./revision.dSYM ./myface_SVMdetector.yml
cleaner:
	rm -rf ./build_FRMachine ./build_FRMachine.dSYM ./revision ./revision.dSYM ./myface_SVMdetector.yml ./training_samples/info.txt
	rm -f ./training_samples/*.png
	printf "Number of p-samples = 0\nNumber of n-samples = 0\n" > ./training_samples/info.txt
