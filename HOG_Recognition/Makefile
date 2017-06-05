build_FRMachine: build_FRMachine.cpp
	g++ -ggdb `pkg-config --cflags --libs opencv` build_FRMachine.cpp -o build_FRMachine
clean:
	rm -rf ./build_FRMachine ./build_FRMachine.dSYM
cleaner:
	rm -rf ./build_FRMachine ./build_FRMachine.dSYM ./training_samples/info.txt
	rm -f ./training_samples/dcface_*
	echo "Number of samples = 0" > ./training_samples/info.txt
