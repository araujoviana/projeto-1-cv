# projeto-1-cv

Projeto 1 - Computação Visual (UPM FCI CC), 2026.2.
Ferramenta de processamento de imagens em C + SDL3.

Os textos da interface usam a família **Arial**, distribuída no arquivo
`assets/Arial.ttf`. O programa monta o caminho a partir do diretório do
executável, sem depender de fontes instaladas no sistema.

> Este README é preenchido pelos integrantes do grupo. Os itens abaixo são exigidos
> pela rubrica (arquivo estruturado, com explicações detalhadas). Substitua cada `TODO`.

## Grupo

<!-- TODO: Nome completo + RA de cada integrante -->

## O que é o projeto

<!-- TODO: descrição do software e do que ele faz -->

## Como funciona

<!-- TODO: visão geral do funcionamento (janelas, fluxo, operações de imagem) -->

## Contribuições

<!-- TODO: o que cada integrante implementou -->

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

<!-- TODO: SO + versão, compilador + versão, SDL + versão, editor - de cada integrante -->

## Licença

[MIT](LICENSE)
