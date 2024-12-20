# (01) 3DG - 19/12/2024

Autoria:
> Diego Guerra<br> 
> RA: 11201810534<br>
> Juan Oliveira de Carvalho<br> 
> RA: 11201810997<br>

## Objetivos

O objetivo deste projeto é desenvolver uma aplicação simples com gráficos 3D, utilizando-se de técnicas e bibliotecas de computação gráfica, incluindo uso de gráficos 3D, interação, animação, iluminação e texturização.
Este projeto é uma evolução da [versão anterior](https://github.com/DiegoGuerra00/3DG), acrescentando iluminação e texturização.

## Motivação

Este projeto foi desenvolvido tendo em mente o ensino de movimentos circulares e pêndulos, podendo ser utilizado para demonstrar o 
movimento circular de um pêndulo ligado a uma corda de comprimento e ângulo variável.

## Live demo

Uma versão compilada em WebAssembly está disponível para ser executada em qualquer navegador web neste [link](https://juanocv.github.io/3DG/pendulum/).

Também é possível compilar o projeto localmente, seguindo as instruções de [instalação do ABCg](https://hbatagelo.github.io/cg/config.html), clonando o repositório e executando os scripts
```build.sh``` ou ```build.bat```, caso esteja no Linux ou Windows respectivamente. Os scripts irão compilar o projeto e criar um executável no diretório ```build```.

Nos [releases](https://github.com/juanocv/3DG/releases/tag/v1.0.0) do repositório providenciamos um binário já compilado para Linux. Para usá-lo basta descompactar o arquivo baixado, navegar para a pasta extraida, rodar o comando ```chmod +x pendulum``` no terminal e por fim
executar o programa com ```./pendulum```.

## Como Usar

O programa foi pensado para ser utilizado apenas em tela cheia, devido à necessidade de captura de inputs de mouse e teclado na janela 
do ABCg. Com o programa em tela cheia é possível utilizar os seguintes controles:

-   ```W A S D``` para se movimentar para frente, trás e para os lados
-   ```Caps Lock``` para destravar o mouse da janela
-   ```Mouse``` para controlar a direção da câmera, ou configurar o pêndulo no menu com o mouse destravado
-   ```ESC``` para sair de tela cheia no navegador

As configurações do sistema do pêndulo, como comprimento e ângulo da corda, podem ser ajustadas a partir da janela pop-up no programa.

## Funcionalidades

O programa consiste em um simulador de um pêndulo cônico preso a uma corda fixada em uma haste fixa. É possível utilizar os controles da tela
para alterar configurações do pêndulo, como:

-   Comprimento da corda
-   Ângulo da inclinação
-   Velocidade da animação
-   Cor da esfera do pêndulo

Adicionalmente, é possível controlar a iluminação geral através das seguintes funcionalidades:
-   Ângulo de rotação da iluminação ao longo do eixo Y (Yaw)
-   Ângulo de rotação da iluminação ao longo do eixo X (Pitch)

## Implementação

A aplicação foi implementada utilizando a biblioteca [ABCg](https://github.com/hbatagelo/abcg), desenvolvida pelo professor Harlen Batagelo para o curso de Computação Gráfica na UFABC, e o pipeline gráfico do OpenGL.

### Estrutura do Código

Para facilitar a navegação e separar o código da biblioteca ABCg, os arquivos relacionados ao projeto estão concentrados no diretório [```pendulum```](https://github.com/juanocv/3DG/tree/main/pendulum) do repositório. No subdiretório
```pendulum/assets``` estão todos os arquivos relacionados a objetos 3D e texturização.

-  ```main.cpp```: Classe principal do programa, onde é iniciada a janela da aplicação
-  ```window.cpp```: É responsável por toda a lógica de renderização, desde os modelos 3D da esfera até a texturização dos objetos
    -  Aqui também é configurada a UI do programa e é realizada a captura e processamento dos inputs de mouse e teclado, responsáveis pela interatividade
-  ```line.cpp```: Aqui é definida a estrutura utilizada como base para a corda e para o mastro do pêndulo
-  ```sphere.cpp```: É responsável pela classe que define a esfera do pêndulo, nela o objeto 3D é criado e carregado
-  ```physics.cpp```: É responsável pelas funções que definem os cálculos utilizados no posicionamento da luz, comprimento da corda, velocidade angular e tamanho da corda (todos em pixels)
-  ```fragment_shader.glsl``` e ```vertex_shader.glsl```: São usados para definir os shaders e matriz de projeção da esfera, respectivamente
-   ```majoras_mask_moon``` e ```snow_02_diff_4k```: São os arquivos de textura utilizados na esfera e chão, e foram obtidos [aqui](https://www.textures-resource.com/nintendo_64/thelegendofzeldamajorasmask/texture/1897/) e [aqui](https://polyhaven.com/a/snow_02), respectivamente.

### Texturização

Tanto o objeto da esfera quanto o chão foram texturizados utilizando a técnica de mapeamento 
2D, com recurso ```GL_TEXTURE_2D``` do OpenGL.

## Desafios Enfrentados

### Composição visual da corda e mastro do pêndulo
> Um dos desafios foi definir como seria feita a composição da corda e mastro que compõe o pêndulo. Inicialmente foi tentado utilizar uma estrutura de cilindro, porém as movimentações ficaram desalinhadas em relação à esfera.
Por isso foi decidido utilizar uma linha, que apesar de possuir limitações (a espessura máxima no navegador por exemplo), permitiu o alinhamento com as animações.

### Cálculo da velocidade angular
> Outro desafio que tivemos foi realizar o cálculo da velocidade angular da esfera corretamente. Nas primeiras tentativas notamos que a velocidade da esfera não se comportava como devia ao variar o comprimento da corda.
Isto foi resolvido revisando os cálculos de velocidade baseados no comprimento da corda. A maior dificuldade para atingir este objetivo ocorreu pois normalmente o cálculo da velocidade angular normalmente se dá utilizando
o comprimento da corda em metros, enquanto aqui utilizamos o comprimento em pixels.

### Implementação da Iluminação
> Enquanto foi tranquilo implementar as texturas baixadas, tivemos alguns pequenos problemas com a iluminação, mais relacionados ao percentual atribuido à variável que define a iluminação geral do ambiente, o que tornava o ambiente como um todo muito escuro apesar das mudanças de pitch e yaw.

### Re-clonagem de repositório
> Outro problema enfrentado, apesar de não relacionado aos assuntos de computação gráfica, foi de não conseguir realizar outro fork do repositório original do ABCg, devido a limitação do Github na quantidade de forks por conta em cada
repositório e ao fato de estarmos realizando o projeto em dupla.
