## Projeto Paint

- Desenvolva um Editor Gráfico em C/C++, estilo o Paint da Microsoft, utilizando as bibliotecas GLUT/OpenGL. Leia os comentários do programa “paint.cpp” e utilize o programa “paint.cpp” como referência. O foco deste trabalho consiste em implementar várias técnicas de Rasterização  e Transformações Geométricas. Para isso, o programa deve atender, no mínimo, os requisitos  listados abaixo:
  * a) Implemente o algoritmo de Bresenham para traçado de linhas, utilizando apenas GL_POINTS como parâmetro da função glBegin(). Esse parâmetro indica que cada vértice deve ser tratado como um ponto simples;
  * b) O Algoritmo de Bresenham implementado só funciona no primeiro octante. Desenvolva funções com os algoritmos para redução ao primeiro octante e sua transformação inversa para que possam ser desenhadas retas em todos os octantes;
  * c) Utilizando o Algoritmo de Bresenham implementado, desenvolva uma função para desenhar quadriláteros, dadas as coordenadas do topo esquerdo e da base direita desse quadrilátero;
  * d) Utilizando o Algoritmo de Bresenham implementado, desenvolva uma função para desenhar triângulos a cada três coordenadas.
  * e) Utilizando o Algoritmo de Bresenham implementado, desenvolva uma função para desenhar polígonos, dadas quatro ou mais coordenadas.
  * f) Implemente funções para realizar transformações geométricas como a translação, escala, cisalhamento, reflexão e rotação das formas geométricas rasterizadas. Obs.: as funções de transformações geométricas da API do OpenGL ou de qualquer outra biblioteca não podem ser utilizadas.
  * g) Implemente uma função com o Algoritmo de Bresenham para rasterização de circunferências. Utilize apenas GL_POINTS como parâmetro da função glBegin(). A função de rasterização receberá como parâmetro as coordenadas do centro da circunferência e o tamanho do raio. 
  * h) Implemente a função de rasterização de polígonos. Nesse caso, a função preencherá as formas geométricas rasterizadas com exceção da circunferência.
  * i) Implemente o algoritmo Flood Fill com vizinhança 4. Pode ser usada a função glReadPixel() do OpenGL para ler a cor do pixel na janela GLUT, ou uma matriz auxiliar para a implementação. Nesse caso, a função preencherá todas as formas geométricas rasterizadas
