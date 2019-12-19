$ sudo apt-get install language-pack-ko
--> password 입력

$ sudo locale-gen ko_KR.UTF-8

$ locale
--> UTF-8인지 확인

컴파일시
cc snake_game.c -lpthread -lncursesw -o filename