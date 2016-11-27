NAME = libcm
TARGETS = $(NAME).info $(NAME).dvi $(NAME).ps
TEX_BYPRODUCTS = $(NAME).aux $(NAME).cp $(NAME).fn $(NAME).ky $(NAME).log $(NAME).pg $(NAME).toc $(NAME).tp $(NAME).vr missfont.log

all: info ps
info: $(NAME).info
ps: $(NAME).ps

$(NAME).info: $(NAME).texi
	makeinfo $(NAME).texi -o $(NAME).info

$(NAME).dvi: $(NAME).texi
	texi2dvi $(NAME).texi

$(NAME).ps: $(NAME).dvi
	dvips -t a4 -N0 $(NAME).dvi -o $(NAME).ps

print: $(NAME).ps
	psselect -p1,_1,2-_2 $(NAME).ps | psnup -pa4 -2 | lp

clean:
	rm -f $(NAME).info $(NAME).dvi $(NAME).ps $(TEX_BYPRODUCTS) *~

