build_FRMachine: build_FRMachine.cpp
	g++ -ggdb `pkg-config --cflags --libs opencv` build_FRMachine.cpp -o build_FRMachine
clean:
	rm -rf ./build_FRMachine ./build_FRMachine.dSYM
cleaner:
	rm -rf ./build_FRMachine ./build_FRMachine.dSYM ./training_samples/info.txt
	rm -f ./training_samples/dcface_*
	printf "Number of p-samples = 0\nNumber of n-samples = 0\n" > ./training_samples/info.txt
