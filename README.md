# Projeto 1 de Computação Visual

Projeto 1 - Computação Visual (UPM FCI CC), 2026.2.
Processamento de imagens em C + SDL3.


> Este README é preenchido pelos integrantes do grupo. Os itens abaixo são exigidos
> pela rubrica (arquivo estruturado, com explicações detalhadas). Substitua cada `TODO`.

## Grupo


## O que é o projeto


## Como funciona


## Contribuições


## Compilação e execução

### Dependências

- `gcc` (C23) e `make`
- SDL3 e SDL_image (via `pkg-config`)

No CachyOS / Arch: `sudo pacman -S base-devel sdl3 sdl3_image`

### Build

```sh
make            # gera ./projeto-1-cv
make run        # compila e executa
make DEBUG=1    # build com AddressSanitizer / UBSan
make clean
```

### Execução

```sh
./projeto-1-cv caminho/para/imagem.png
```

Durante a execução, pressione `S` com foco na janela principal para salvar a
imagem atualmente exibida em `output_image.png`. O terminal informa se o arquivo
foi criado ou sobrescrito.

### Ambientes usados pelo grupo


## Licença

[MIT](LICENSE)
