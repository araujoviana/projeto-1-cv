# Projeto 1 de Computação Visual

Projeto 1 - Computação Visual (UPM FCI CC), 2026.2, Prof. André Kishimoto.
Processamento de imagens em C + SDL3.

## Grupo

| Nome completo                | RA       |
|------------------------------|----------|
| Matheus Gabriel Viana Araujo | 10420444 |
| Eduardo Takashi Missaka      | 10417877 |
| Arthur Meneses Neves         | 10425727 |
| João Victor Vidal Barbosa    | 10410165 |

## O que é o projeto

Um programa de linha de comando que recebe o caminho de uma imagem, converte-a para
escala de cinza e a exibe em uma janela junto com uma segunda janela contendo o seu
histograma, estatísticas de brilho e contraste e dois botões de ação. O código parte do
código-base da disciplina e foi reorganizado em módulos.

Funcionalidades:

1. Carregamento de imagem com SDL_image, com mensagens de erro no terminal.
2. Detecção de imagem colorida ou em escala de cinza e conversão com
   `Y = 0.2125 R + 0.7154 G + 0.0721 B`.
3. Duas janelas: principal (1024x768, centralizada) e secundária, filha da principal,
   em tamanho fixo na posição (0,0).
4. Histograma de 256 níveis, com média de intensidade (clara / média / escura) e desvio
   padrão (contraste alto / médio / baixo).
5. Botão "Equalizar" / "Ver original": equaliza o histograma e reverte sem recarregar o
   arquivo.
6. Botão "Resolução original" / "1024x768": alterna o tamanho da imagem e da janela
   principal. A janela é centralizada, exceto quando excede a tela; nesse caso vai para (0,0).
7. Tecla `S` na janela principal: salva a imagem **exibida** (na resolução atual da
   janela) em `output_image.png`, informando no terminal se o arquivo foi criado ou
   sobrescrito.
8. Textos com a fonte **Liberation Sans** (licença livre), incluída em `assets/`.

## Como funciona

```
src/main.c        Ponto de entrada: argumentos, inicialização e encerramento
src/window.c      Wrapper de SDL_Window + SDL_Renderer
src/image.c       Carga, conversão, equalização, estatísticas e salvamento
src/interface.c   Janela principal, janela do histograma, botões e texto
src/event.c       Laço de eventos (mouse, teclado, fechar janela)
```

Fluxo do programa:

1. `main` valida os argumentos, inicializa SDL e SDL_ttf e cria as duas janelas.
2. `Image_load` abre o arquivo com `IMG_Load` e converte a superfície para RGBA32.
3. `Image_convert` verifica se todos os pixels têm R = G = B (imagem já cinza) e, caso
   contrário, gera a versão em cinza (`surfaceFilter`), que passa a ser a base de todas
   as operações. O resultado é informado no terminal.
4. `Image_calculate_statistics` calcula o histograma, a média e o desvio padrão da
   imagem exibida. As classificações usam limiares de média 85/170 e de desvio 30/60.
5. A equalização usa a função de distribuição acumulada do histograma como tabela de
   mapeamento (LUT): `s_k = round(255 * CDF(k) / N)`. O resultado fica em uma superfície
   separada, então voltar ao original é só trocar a superfície exibida.
6. Os botões são retângulos desenhados com primitivas da SDL. Os cliques e o hover são
   tratados em `Event_loop` comparando a posição do mouse com o retângulo. Cores: azul
   (neutro), azul claro (mouse em cima) e azul escuro (clicado).
7. A fonte é procurada em `assets/LiberationSans-Regular.ttf` relativo ao executável
   (`SDL_GetBasePath`), portanto não depende das fontes instaladas no sistema.

## Contribuições

Conforme o histórico do Git:

- **Matheus Gabriel Viana Araujo:** estrutura inicial do projeto (Makefile multiplataforma,
  `.gitignore`, licença), integração dos módulos, itens 4, 5 e 6, janela filha e README.
- **Arthur Meneses Neves:** itens 1 e 2 (carregamento de imagem e conversão para cinza).
- **João Victor Vidal Barbosa:** itens 3 e 4 (interface com duas janelas e histograma).
- **Eduardo Takashi Missaka:** itens 7 e 8 (salvar imagem e fonte).

## Compilação e execução

### Dependências

- `gcc` (C23) e `make`
- SDL3, SDL3_image e SDL3_ttf. No Linux o `Makefile` as localiza por `pkg-config`
  (módulos `sdl3`, `sdl3-image`, `sdl3-ttf`). No Windows são usadas as DLLs e os headers
  incluídos no repositório (`bin/`, `lib/`, `include/`).

No Arch / CachyOS: `sudo pacman -S base-devel sdl3 sdl3_image sdl3_ttf`

No Fedora: `sudo dnf install gcc make pkgconf-pkg-config SDL3-devel SDL3_image-devel SDL3_ttf-devel`

### Build

```sh
make            # gera ./projeto-1-cv (projeto-1-cv.exe no Windows)
make run        # compila e executa
make DEBUG=1    # build com AddressSanitizer / UBSan
make clean
```

### Execução

```sh
./projeto-1-cv caminho/para/imagem.png
make run caminho/para/imagem.png    # equivalente
```

Uma imagem de teste (`dog.jpg`) acompanha o repositório.

- Botão **Equalizar / Ver original**: alterna entre a imagem em cinza e a equalizada.
- Botão **Resolução original / 1024x768**: alterna o tamanho da imagem e da janela.
- Tecla **S** (com foco na janela principal): salva a imagem exibida em `output_image.png`.

### Ambientes usados pelo grupo

| Integrante                   | SO                     | Compilador                        | Editor      |
|------------------------------|------------------------|-----------------------------------|-------------|
| Matheus Gabriel Viana Araujo | Fedora Linux 42 (WSL2) | gcc 15.2.1                        | Emacs       |
| Arthur Meneses Neves         | Windows                | gcc (versão a informar)           | Notepad++   |
| Eduardo Takashi Missaka      | Windows                | gcc (versão a informar)           | Notepad++   |
| João Victor Vidal Barbosa    | Windows                | gcc (versão a informar)           | VSCode      |

Versões das bibliotecas:

| Biblioteca | Linux (ambiente de desenvolvimento) | Windows (headers/DLLs do repositório) |
|------------|-------------------------------------|----------------------------------------|
| SDL3       | 3.4.8                               | 3.4.x (headers 3.4.16)                 |
| SDL3_image | 3.4.0                               | 3.4.6 (headers)                        |
| SDL3_ttf   | 3.2.2                               | 3.3.0 (headers)                        |

## Licença

[MIT](LICENSE). A fonte Liberation Sans é distribuída sob a SIL Open Font License.
