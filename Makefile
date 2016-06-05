
main: 
	gcc -Wall Pong.c -o Pong 
	@echo "Program built."

debug:
	gcc -Wall -D DEBUG Pong.c -o Pong
	@echo "Program built under debug control"

strict: 
	gcc -Wall -Werror -O3 Pong.c -o Pong 
	@echo "Program built under strict control"


