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
#include <vector>
#include <array>

using namespace std;

// Variaveis Globais
#define ESC 27
#define ENTER 13
#define SPACE 32
#define C 67 


//Enumeracao com os tipos de formas geometricas
enum tipo_forma {LIN = 1, TRI = 2, RET = 3, POL = 4, CIR}; // Linha, Triangulo, Retangulo, Poligono, Circulo

//Verifica se foi realizado o primeiro clique do mouse
bool click1 = false;

// Verifica se a funcao desenhar um novo poligono e verdadeiro
bool drawPol = false;

//Verifica se foi realizado o primeiro e segundo clique do mouse para desenhar um triangulo
bool drawTri1 = false, drawTri2 = false;

//Verifica se foi realizado o primeiro clique do mouse para desenhar um retangulo
bool drawRet = false;

//Verifica se o espaco foi pressionado 
bool isSpacePressed = false;

//Verifica se "c" foi pressionado 
bool isCPressed = false;

//Verifica se "r" foi pressionado 
bool isRPressed = false;

//Verifica se "e" foi pressionado 
bool isEPressed = false;

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
	float cx, cy;
	forward_list<vertice> v; //lista encadeada de vertices
};

// Lista encadeada de formas geometricas
forward_list<forma> formas;

void updateCentroide(forma& f)
{
    if (f.v.empty()) return;
    float cx = 0, cy = 0; // centro da forma	
    for (auto it = f.v.begin(); it != f.v.end(); ++it) {
        vertice& v = *it;
        cx += v.x;
        cy += v.y;
    }
    int size = distance(f.v.begin(), f.v.end());
    cx /= size;
    cy /= size;
    f.cx = cx;
    f.cy = cy;
}

void pushForma(int tipo)
{
    forma f;
    f.tipo = tipo;
    f.cx = 0;
    f.cy = 0;
    formas.push_front(f);
}

void pushVertice(int x, int y)
{
    vertice v;
    v.x = x;
    v.y = y;
    formas.front().v.push_front(v);

    // Recalcular o centroide
    updateCentroide(formas.front());
}


//Funcao para armazenar uma Linha na lista de formas geometricas
void pushLinha(int x1, int y1, int x2, int y2)
{
	pushForma(LIN);
	pushVertice(x1, y1);
	pushVertice(x2, y2);
}

//Funcao para armazenar um Retangulo na lista de formas geometricas
void pushRet(int x1, int y1, int x2, int y2)
{
	pushForma(RET);
	pushVertice(x1, y1);
	pushVertice(x2, y2);
}

//Funcao para armazenar um Triangulo na lista de formas geometricas
void pushTri(int x1,int y1, int x2,int y2, int x3, int y3){
	pushForma(TRI);
	pushVertice(x1, y1);
	pushVertice(x2, y2);
	pushVertice(x3, y3);
}

//Fucao para armazenar um Poligono na lista de formas geometricas
void pushPOL(std::vector<vertice> &pts){
	pushForma(POL);
    for (std::vector<vertice>::iterator v = pts.begin(); v != pts.end(); ++v) {
        pushVertice(v->x, v->y);
    }
}
// 
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
// void POLbresenham(vector<vertice> &pts);
void POLbresenham(std::vector<int>& x, std::vector<int>& y);
void translateFormaGeometrica(int tx, int ty, forma& f);
void special(int key, int x, int y);
void scaleFormaGeometrica(float sx, float sy, forma& f);
void shearFormaGeometrica(float shx, float shy, forma& f);
void rotateFormaGeometrica(float theta, forma& f);
void updateCentroide(forma& f);
void POLbresenham(std::vector<std::vector<vertice> >& polList);
void refleteFormaGeometrica(char eixo, forma& f);

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
	glutSpecialFunc(special);
	glutMouseFunc(mouse); //funcao callback do mouse
	glutPassiveMotionFunc(mousePassiveMotion); //fucao callback do movimento passivo do mouse
	glutDisplayFunc(display); //funcao callback de desenho

	// Define o menu pop-up
	glutCreateMenu(menu_popup);
	glutAddMenuEntry("Linha", LIN);
	glutAddMenuEntry("Retangulo", RET);
	glutAddMenuEntry("Triangulo", TRI);
	glutAddMenuEntry("Poligono", POL);
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
		// polList.clear();
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
	case ENTER:
		drawPol = false;
		printf ("ENTER\n");
		break;
	case SPACE:
		printf ("ESPACO\n");
    	isSpacePressed = !isSpacePressed;
    	break;
    case 67:
 	case 99:
		printf ("C\n");
    	isCPressed = !isCPressed;
    	break;
    case 114:
 	case 82:
    	isRPressed = !isRPressed;
    	printf(isRPressed ? "R: true" : "R: false"); // prints true
    	break;
    case 69:
 	case 101:
    	isEPressed = !isEPressed;
    	printf(isEPressed ? "E: true" : "E: false"); // prints true
    	break;
	}
}

void special(int key, int x, int y)
{
    switch(key)
    {
    case GLUT_KEY_UP:
		if(!isSpacePressed) translateFormaGeometrica(0, 1, formas.front());
		else scaleFormaGeometrica(1, 1.1, formas.front());
        break;  
    case GLUT_KEY_DOWN: 
        if(!isSpacePressed) translateFormaGeometrica(0, -1, formas.front());
		else scaleFormaGeometrica(1, 0.9, formas.front());   
        break;
    case GLUT_KEY_LEFT:
        if(isSpacePressed) scaleFormaGeometrica(0.9, 0.9, formas.front());
        else if(isEPressed) refleteFormaGeometrica( 'y' , formas.front());
    	else if(isRPressed) rotateFormaGeometrica(45, formas.front());
        // else if(isCPressed) shearFormaGeometrica(0.7, 0.0, formas.front());
		else translateFormaGeometrica(-1, 0 , formas.front());   
        break;
    case GLUT_KEY_RIGHT:
        if(isSpacePressed) scaleFormaGeometrica(1.1, 1.1, formas.front());
        else if(isRPressed) rotateFormaGeometrica(315, formas.front());	
		else if(isEPressed) refleteFormaGeometrica( 'x' , formas.front());	
        // else if(isCPressed) shearFormaGeometrica(0.0, 0.7, formas.front());
		else translateFormaGeometrica(1, 0, formas.front());   
        break;
    }

    glutPostRedisplay();
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
		case POL:
			if (state == GLUT_DOWN){
				vertice vi = {x, height - y - 1};
				vector<vertice> pol; // lista encadeada de v?rtices para o pol?gono atual
				if(!drawPol){ // enter falso  // indica o inicio de um poligono  
                    pol.push_back(vi);
                    pushPOL(pol);
                    printf("Clique (%d, %d)\n", x, height - y - 1);
                    drawPol = true;
				}else{ // enter verdadeiro
					// Continua atualizando o pol?gono em constru??o
                    formas.front().v.push_front(vi);
                    updateCentroide(formas.front());
                    pol.clear();
                    glutPostRedisplay();	
				}
			}
			break;
		}
		break;
       // case GLUT_MIDDLE_BUTTON:
       //     if (state == GLUT_DOWN) {
       //     		printf("MIDDLE");
       //     		printf("Clique (%d, %d)\n", x, height - y - 1);
       //     		translateFormaGeometrica(1, 0, formas.front());
       //         glutPostRedisplay();
       //     }
       // break;
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
	else if(drawRet) RETbresenham(x_1, y_1, m_x, m_y);
	else if(drawTri1) bresenham(x_1, y_1, m_x, m_y);
	else if(drawTri2) TRIbresenham(x_1, y_1, x_2, y_2 ,m_x, m_y);
	else if(drawPol) bresenham(formas.front().v.front().x, formas.front().v.front().y, m_x, m_y);

	//Percorre a lista de formas geometricas para desenhar
	for(forward_list<forma>::iterator f = formas.begin(); f != formas.end(); f++)
	{
		int i=0;
		std::vector<int> x;
	    std::vector<int> y;
		switch (f->tipo)
		{
			case LIN:
				//Percorre a lista de vertices da forma linha para desenhar
				for(forward_list<vertice>::iterator v = f->v.begin(); v != f->v.end(); v++, i++)
				{
					x.push_back(v->x); 
					y.push_back(v->y); 
				}
				//Desenha o segmento de reta apos dois cliques
				bresenham(x[0], y[0], x[1], y[1]);
				break;
			case RET:
				//Percorre a lista de vertices da forma linha para desenhar
				for(forward_list<vertice>::iterator v = f->v.begin(); v != f->v.end(); v++, i++)
				{
					x.push_back(v->x); 
					y.push_back(v->y); 
				}
				//Desenha o quadrilatero apos dois cliques
				RETbresenham(x[0], y[0], x[1], y[1]);
				break;
			case TRI:
				for(forward_list<vertice>::iterator v = f->v.begin(); v != f->v.end(); v++, i++)
				{
					x.push_back(v->x); 
					y.push_back(v->y); 
				}
				//Desenha o triangulo apos dois cliques
				TRIbresenham(x[0], y[0], x[1], y[1], x[2], y[2]);
			case POL:
				for(forward_list<vertice>::iterator v = f->v.begin(); v != f->v.end(); v++, i++)
				{
					x.push_back(v->x); 
					y.push_back(v->y); 
					// printf("x: %d, y: %d \n",v->x, v->y);
				}
				//Desenha um poligono apos dois cliques
				POLbresenham(x, y);
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

void POLbresenham(std::vector<int>& x, std::vector<int>& y) {
    int n = x.size();

    for (int i = 0; i < n - 1; ++i) {
        bresenham(x[i], y[i], x[i+1], y[i+1]);
    }

    // Desenhar a ?ltima linha conectando o ?ltimo v?rtice com o primeiro
    bresenham(x.back(), y.back(), x.front(), y.front());
}

void translateFormaGeometrica(int tx, int ty, forma& f) {
	if(f.v.empty()) return;
	for (auto it = f.v.begin(); it != f.v.end(); ++it) {
    	vertice& v = *it;
    	v.x += tx;
    	v.y += ty;
	}
	// updatePolList();
	glutPostRedisplay();
	
}

void scaleFormaGeometrica(float sx, float sy, forma& f)
{
    if (f.v.empty()) return;
	printf("centroide (%f, %f)\n", f.cx, f.cy); // Imprime as coordenadas do centróide da forma
    for (auto it = f.v.begin(); it != f.v.end(); ++it) {
        vertice& v = *it;
        // Calcula a distância do ponto em relação ao centróide
        float dx = v.x - f.cx;
        float dy = v.y - f.cy;
        float dist = sqrt(dx * dx + dy * dy);
        // Verifica se a escala ultrapassa o limiar definido
        if((sx > 1 && dist >= 420) || (sx < 1 && dist <= 60)){sx = 1;sy = 1;} 
        // Aplica a transformação de escala aos pontos da forma
        v.x = round((v.x - f.cx) * sx + f.cx);
		v.y = round((v.y - f.cy) * sy + f.cy);
    }
    // Atualiza as coordenadas do centróide após a transformação
    updateCentroide(f);
    // Redesenha a forma na tela
    glutPostRedisplay();
}

void rotateFormaGeometrica(float theta, forma& f) {
    if (f.v.empty()) return;
    theta = theta * 3.14 / 180.0f; // converte para radianos
    float cosT = cos(theta);
    float sinT = sin(theta);
    for (auto it = f.v.begin(); it != f.v.end(); ++it) {
        vertice& v = *it;
        float dx = v.x - f.cx;
        float dy = v.y - f.cy;
        v.x = round(f.cx + dx * cosT - dy * sinT);
        v.y = round(f.cy + dy * cosT + dx * sinT);
    }
    glutPostRedisplay();
}

void refleteFormaGeometrica(char eixo, forma& f) {
    if (f.v.empty()) return;
    printf("char: %c\n", eixo);
    float sinal = (eixo == 'x' ? -1.0 : -1.0); // determina o sinal da reflexão
    for (auto it = f.v.begin(); it != f.v.end(); ++it) {
        vertice& v = *it;
        // Translada para a origem
        v.x -= f.cx;
        v.y -= f.cy;
        // Realiza a reflexão
        if (eixo == 'x') {
        	printf("invertendo eixo x\n");
            v.y *= sinal;
        } else {
        	printf("invertendo eixo y\n");
            v.x *= sinal;
        }
        // Translada de volta para a posição original
        v.x += f.cx;
        v.y += f.cy;
    }
    glutPostRedisplay();
}

