CC=g++
ARGS= -I./lib/portaudio/include ./lib/portaudio/lib/.libs/libportaudio.a -lrt -lasound -ljack -pthread \
	-I./lib/fftw-3.3.10/api -lfftw3
EXEC=build/run

main:main.cpp
	mkdir -p build
	$(CC) -o $(EXEC) $^ $(ARGS)

install-deps:install-portaudio 
.PHONY:install-deps

uninstall-deps:uninstall-portaudio 
.PHONY:uninstall-deps

install-portaudio:
	mkdir -p lib

	curl https://files.portaudio.com/archives/pa_stable_v190700_20210406.tgz | tar -zx -C lib
	cd lib/portaudio && ./configure && $(MAKE) -j
.PHONY: install-portaudio

uninstall-portaudio:
	cd lib/portaudio $$ $(MAKE) uninstall
	rm -rf lib/portaudio
.PHONY: uninstall-portaudio

install-fftw:
	mkdir -p lib

	curl https://www.fftw.org/fftw-3.3.10.tar.gz | tar -zx -C lib
	cd lib/fftw-3.3.10 && ./configure && $(MAKE) -j && sudo $(MAKE) install
.PHONY: install-fftw

uninstall-fftw:
	cd lib/fftw-3.3.10 $$ $(MAKE) uninstall
	rm -rf lib/fftw-3.3.10
.PHONY: uninstall-fftw

run:
	./build/run 2>/dev/null

clean:
	rm build/*