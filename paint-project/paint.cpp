/*
 * Computacao Grafica
 * Codigo Exemplo: Rasterizacao de Segmentos de Reta com GLUT/OpenGL
 * Autor: Pedro Felipe
 */

// Bibliotecas utilizadas pelo OpenGL
#ifdef __APPLE__
#define GL_SILENCE_DEPRECATION
#include <GLUT/glut.h>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#endif

#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <forward_list>
#include "glut_text.h"

using namespace std;

// Variaveis Globais
#define ESC 27

//Enumeracao com os tipos de formas geometricas
enum tipo_forma {LIN = 1, TRI = 2, RET = 3, POL, CIR}; // Linha, Triangulo, Retangulo, Poligono, Circulo

//Verifica se foi realizado o primeiro clique do mouse
bool click1 = false;

bool drawTri1 = false, drawTri2 = false;

bool drawRet = false;

//Coordenadas da posicao atual do mouse
int m_x, m_y;

//Coordenadas do primeiro clique e do segundo clique do mouse
int x_1, y_1, x_2, y_2, x_3, y_3;

//Indica o tipo de forma geometrica ativa para desenhar
int modo = LIN;

//Largura e altura da janela
int width = 512, height = 512;

// Definicao de vertice
struct vertice
{
	int x;
	int y;
};

// Definicao das formas geometricas
struct forma
{
	int tipo;
	forward_list<vertice> v; //lista encadeada de vertices
};

// Lista encadeada de formas geometricas
forward_list<forma> formas;

// Funcao para armazenar uma forma geometrica na lista de formas
// Armazena sempre no inicio da lista
void pushForma(int tipo)
{
	forma f;
	f.tipo = tipo;
	formas.push_front(f);
}

// Funcao para armazenar um vertice na forma do inicio da lista de formas geometricas
// Armazena sempre no inicio da lista
void pushVertice(int x, int y)
{
	vertice v;
	v.x = x;
	v.y = y;
	formas.front().v.push_front(v);
}

//Fucao para armazenar uma Linha na lista de formas geometricas
void pushLinha(int x1, int y1, int x2, int y2)
{
	pushForma(LIN);
	pushVertice(x1, y1);
	pushVertice(x2, y2);
}

void pushRet(int x1, int y1, int x2, int y2)
{
	pushForma(RET);
	pushVertice(x1, y1);
	pushVertice(x2, y2);
}

void pushTri(int x1,int y1, int x2,int y2, int x3, int y3){
	pushForma(TRI);
	pushVertice(x1, y1);
	pushVertice(x2, y2);
	pushVertice(x3, y3);
}

/*
 * Declaracoes antecipadas (forward) das funcoes (assinaturas das funcoes)
 */
void init(void);
void reshape(int w, int h);
void display(void);
void menu_popup(int value);
void keyboard(unsigned char key, int x, int y);
void mouse(int button, int state, int x, int y);
void mousePassiveMotion(int x, int y);
void drawPixel(int x, int y);
// Funcao que percorre a lista de formas geometricas, desenhando-as na tela
void drawFormas();
// Funcao que implementa o Algoritmo Imediato para rasterizacao de segmentos de retas
void bresenham(int x1, int y1, int x2, int y2);
void RETbresenham(int x1, int y1, int x2, int y2);
void TRIbresenham(int x1, int y1, int x2, int y2, int x3, int y3);
/*
 * Funcao principal
 */
int main(int argc, char** argv)
{
	glutInit(&argc, argv); // Passagens de parametro C para o glut
	glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB); //Selecao do Modo do Display e do Sistema de cor
	glutInitWindowSize (width, height);  // Tamanho da janela do OpenGL
	glutInitWindowPosition (100, 100); //Posicao inicial da janela do OpenGL
	glutCreateWindow ("Computacao Grafica: Paint"); // Da nome para uma janela OpenGL
	init(); // Chama funcao init();
	glutReshapeFunc(reshape); //funcao callback para redesenhar a tela
	glutKeyboardFunc(keyboard); //funcao callback do teclado
	glutMouseFunc(mouse); //funcao callback do mouse
	glutPassiveMotionFunc(mousePassiveMotion); //fucao callback do movimento passivo do mouse
	glutDisplayFunc(display); //funcao callback de desenho

	// Define o menu pop-up
	glutCreateMenu(menu_popup);
	glutAddMenuEntry("Linha", LIN);
   glutAddMenuEntry("Retangulo", RET);
   glutAddMenuEntry("Triangulo", TRI);
   glutAddMenuEntry("Limpar", -1);
	glutAddMenuEntry("Sair", 0);
	glutAttachMenu(GLUT_RIGHT_BUTTON);


	glutMainLoop(); // executa o loop do OpenGL
	return EXIT_SUCCESS; // retorna 0 para o tipo inteiro da funcao main();
}

/*
 * Inicializa alguns parametros do GLUT
 */
void init(void)
{
	glClearColor(1.0, 1.0, 1.0, 1.0); //Limpa a tela com a cor branca;
}

/*
 * Ajusta a projecao para o redesenho da janela
 */
void reshape(int w, int h)
{
	// Muda para o modo de projecao e reinicializa o sistema de coordenadas
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	// Definindo o Viewport para o tamanho da janela
	glViewport(0, 0, w, h);

	width = w;
	height = h;
	glOrtho (0, w, 0, h, -1 , 1);

	// muda para o modo de desenho
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

}

/*
 * Controla os desenhos na tela
 */
void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT); //Limpa o buffer de cores e reinicia a matriz
	glColor3f (0.0, 0.0, 0.0); // Seleciona a cor default como preto
	drawFormas(); // Desenha as formas geometricas da lista
	//Desenha texto com as coordenadas da posicao do mouse
	draw_text_stroke(0, 0, "(" + to_string(m_x) + "," + to_string(m_y) + ")", 0.2);
	glutSwapBuffers(); // manda o OpenGl renderizar as primitivas

}

/*
 * Controla o menu pop-up
 */
void menu_popup(int value)
{
	if (value == 0) exit(EXIT_SUCCESS);
	if(value == -1){
		glClear(GL_COLOR_BUFFER_BIT);
		formas.clear(); // Remove todas as formas da list
	}
	modo = value;
	// printf("%d", modo);
}


/*
 * Controle das teclas comuns do teclado
 */
void keyboard(unsigned char key, int x, int y)
{
	switch (key)   // key - variavel que possui valor ASCII da tecla precionada
	{
	case ESC:
		exit(EXIT_SUCCESS);
		break;
	}
}

/*
 * Controle dos botoes do mouse
 */
void mouse(int button, int state, int x, int y)
{
	switch (button)
	{
	case GLUT_LEFT_BUTTON:
		switch(modo)
		{
		case LIN:
			if (state == GLUT_DOWN)
			{
				if(click1)
				{
					x_2 = x;
					y_2 = height - y - 1;
					printf("Clique 2(%d, %d)\n", x_2, y_2);
					pushLinha(x_1, y_1, x_2, y_2);
					click1 = false;
					glutPostRedisplay();
				}
				else
				{
					click1 = true;
					x_1 = x;
					y_1 = height - y - 1;
					printf("Clique 1(%d, %d)\n", x_1, y_1);
				}
			}
			break;
		case RET:
			if (state == GLUT_DOWN)
			{
				if(drawRet)
				{
					x_2 = x;
					y_2 = height - y - 1;
					printf("Clique 2(%d, %d)\n", x_2, y_2);
					pushRet(x_1, y_1, x_2, y_2);
					drawRet = false;
					glutPostRedisplay();
				}
				else
				{
					drawRet = true;
					x_1 = x;
					y_1 = height - y - 1;
					printf("Clique 1(%d, %d)\n", x_1, y_1);
				}
			}
			break;
		case TRI:
			if (state == GLUT_DOWN)
			{
				if(drawTri1)
				{
					x_2 = x;
					y_2 = height - y - 1;
					printf("Clique 2(%d, %d)\n", x_2, y_2);
					// pushRet(x_1, y_1, x_2, y_2);
					drawTri1 = false;
					drawTri2 = true;
 					// glutPostRedisplay();
				}else if(drawTri2){
					x_3 = x;
					y_3 = height - y - 1;
					printf("Clique 3(%d, %d)\n", x_3, y_3);
					pushTri(x_1, y_1, x_2, y_2, x_3, y_3);
					drawTri1 = false;
					drawTri2 = false;
					glutPostRedisplay();	
				}else
				{
					drawTri1 = true;
					x_1 = x;
					y_1 = height - y - 1;
					printf("Clique 1(%d, %d)\n", x_1, y_1);
				}
			}
			break;
		}
		break;

//        case GLUT_MIDDLE_BUTTON:
//            if (state == GLUT_DOWN) {
//                glutPostRedisplay();
//            }
//        break;
//
//        case GLUT_RIGHT_BUTTON:
//            if (state == GLUT_DOWN) {
//                glutPostRedisplay();
//            }
//        break;

	}
}

/*
 * Controle da posicao do cursor do mouse
 */
void mousePassiveMotion(int x, int y)
{
	m_x = x;
	m_y = height - y - 1;
	glutPostRedisplay();
}

/*
 * Funcao para desenhar apenas um pixel na tela
 */
void drawPixel(int x, int y)
{
	glBegin(GL_POINTS); // Seleciona a primitiva GL_POINTS para desenhar
	glVertex2i(x, y);
	glEnd();  // indica o fim do ponto
}

/*
 *Funcao que desenha a lista de formas geometricas
 */
void drawFormas()
{
	//Apos o primeiro clique, desenha a reta com a posicao atual do mouse
	if(click1) bresenham(x_1, y_1, m_x, m_y);
	if(drawRet) RETbresenham(x_1, y_1, m_x, m_y);
	if(drawTri1) bresenham(x_1, y_1, m_x, m_y);
	if(drawTri2) TRIbresenham(x_1, y_1, x_2, y_2 ,m_x, m_y);

	//Percorre a lista de formas geometricas para desenhar
	for(forward_list<forma>::iterator f = formas.begin(); f != formas.end(); f++)
	{
		int i=0 ,  x[3], y[3];
		switch (f->tipo)
		{
			case LIN:
				
				// i = 0;
				//Percorre a lista de vertices da forma linha para desenhar
				for(forward_list<vertice>::iterator v = f->v.begin(); v != f->v.end(); v++, i++)
				{
					x[i] = v->x;
					y[i] = v->y;
				}
				//Desenha o segmento de reta apos dois cliques
				bresenham(x[0], y[0], x[1], y[1]);
				break;
			case RET:
				// i = 0;
				//Percorre a lista de vertices da forma linha para desenhar
				for(forward_list<vertice>::iterator v = f->v.begin(); v != f->v.end(); v++, i++)
				{
					x[i] = v->x;
					y[i] = v->y;
				}
				//Desenha o quadrilatero apos dois cliques
				RETbresenham(x[0], y[0], x[1], y[1]);
				break;
			case TRI:
				for(forward_list<vertice>::iterator v = f->v.begin(); v != f->v.end(); v++, i++)
				{
					x[i] = v->x;
					y[i] = v->y;
				}
				//Desenha o triangulo apos dois cliques
				TRIbresenham(x[0], y[0], x[1], y[1], x[2], y[2]);
		}
	}
} 

void bresenham(int x1, int y1, int x2, int y2)
{	
	bool declive = false, simetrico = false;
    int aux = 0, aux2 = 0;
    
    if((x2 - x1) * (y2 - y1) < 0){
        y1 = y1 * -1;
        y2 = y2 * -1;
        simetrico = true;
    }if(abs((x2 - x1)) < abs((y2 - y1))){
        aux = x1;
        x1 = y1;
        y1 = aux;
        
        aux2 = x2;
        x2 = y2;
        y2 = aux2;
        declive = true;
    }if(x1 > x2){
        aux = x1, 
		aux2 = y1;
		
        x1 = x2 ; 
		y1 = y2;
		
        x2 = aux; 
		y2 = aux2;
    }
	
	int d = (2 * (y2 - y1)) - (x2 - x1);
	int incE = 2 * (y2 - y1);
	int incNE = 2 * ((y2 - y1) - (x2 - x1));

	drawPixel(x1, y1);

	while(x1 < x2)
	{
		if(d <= 0)
		{
			d += incE;
		}
		else
		{
			d += incNE;
			y1 += 1;
		}
		x1 += 1;
		if(declive && simetrico){
			drawPixel(y1, x1 * -1);
		}
		if(declive && !simetrico){
			drawPixel(y1, x1);
		}
		if(!declive && simetrico){
			drawPixel(x1, y1 * -1);			
		}
		if(!declive && !simetrico){
			drawPixel(x1, y1);
		}

	}
	drawPixel(x2, y2);
}

void RETbresenham(int x1, int y1, int x2, int y2)
{	
	bresenham(x1, y1, x2, y1);
	bresenham(x1, y2, x2, y2);
	bresenham(x1, y1, x1, y2);
	bresenham(x2, y1, x2, y2);	
}

void TRIbresenham(int x1, int y1, int x2, int y2, int x3, int y3)
{	
	bresenham(x1, y1, x2, y2);
	bresenham(x2, y2, x3, y3);
	bresenham(x3, y3, x1, y1);

}

/*
 * Funcao que implementa o Algoritmo de Rasterizacao da Reta Imediata
 */
// void retaImediata(double x1, double y1, double x2, double y2){
//     double m, b, yd, xd;
//     double xmin, xmax,ymin,ymax;
//
//     drawPixel((int)x1,(int)y1);
//     if(x2-x1 != 0){ //Evita a divisao por zero
//         m = (y2-y1)/(x2-x1);
//         b = y1 - (m*x1);
//
//         if(m>=-1 && m <= 1){ // Verifica se o declive da reta tem tg de -1 a 1, se verdadeira calcula incrementando x
//             xmin = (x1 < x2)? x1 : x2;
//             xmax = (x1 > x2)? x1 : x2;
//
//             for(int x = (int)xmin+1; x < xmax; x++){
//                 yd = (m*x)+b;
//                 yd = floor(0.5+yd);
//                 drawPixel(x,(int)yd);
//             }
//         }else{ // Se tg menor que -1 ou maior que 1, calcula incrementado os valores de y
//             ymin = (y1 < y2)? y1 : y2;
//             ymax = (y1 > y2)? y1 : y2;
//
//             for(int y = (int)ymin + 1; y < ymax; y++){
//                 xd = (y - b)/m;
//                 xd = floor(0.5+xd);
//                 drawPixel((int)xd,y);
//             }
//         }
//
//     }else{ // se x2-x1 == 0, reta perpendicular ao eixo x
//         ymin = (y1 < y2)? y1 : y2;
//         ymax = (y1 > y2)? y1 : y2;
//         for(int y = (int)ymin + 1; y < ymax; y++){
//             drawPixel((int)x1,y);
//         }
//     }
//     drawPixel((int)x2,(int)y2);
// }