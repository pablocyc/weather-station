# UPDATE THIS MAKE FILE FOR BETTER USE ON WEATHER PROYECT

# Uncomment lines below if you have problems with $PATH
#SHELL := /bin/bash
#PATH := /usr/local/bin:$(PATH)

all:
	pio -c vim run

upload:
	pio -c vim run --target upload

clean:
	pio -c vim run --target clean

program:
	pio -c vim run --target program

uploadfs:
	pio -c vim run --target uploadfs

update:
	pio -c vim update
