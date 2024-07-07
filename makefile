
names = main liquid_production_line pill_production_line opengl

files:
	gcc  main.c -o main 
	gcc liquid_production_line.c -o liquid_production_line
	gcc pill_production_line.c -o pill_production_line
	gcc opengl.c -o opengl -lglut -lGLU -lGL -lm


run:
	./opengl

clean:
	rm -f $(names)
	

all:
	make clean
	make files
	clear
	make run
	



